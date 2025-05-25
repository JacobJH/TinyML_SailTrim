#include "Particle.h"
#include "features.h"
#include "random_forest_model_int16.h"
#include "DifferentialPressureSensor.h" // Include the sensor header
#include "adxl343.h" // Include the ADXL343 library

// Struct for sensor data
struct SensorSample {
    float accX;
    float accY;
    float accZ;
    float pressure;
    float gyroX;
    float gyroY;
    float gyroZ;
};

// Constants for window and step size
const int WINDOW_SIZE = 900;
const int STEP_SIZE = 450;

// Two overlapping buffers for SensorSample
SensorSample bufferA[WINDOW_SIZE];
SensorSample bufferB[WINDOW_SIZE];
int indexA = 0;
int indexB = 0;
bool bufferAFull = false;
bool bufferBFull = false;
int sampleCount = 0;

// Global temporary arrays for feature extraction
float accY[WINDOW_SIZE];
float accZ[WINDOW_SIZE];
float pressure[WINDOW_SIZE];
float gyroY[WINDOW_SIZE];
float gyroZ[WINDOW_SIZE];

// Function prototypes
void readSensorAndBuffer();
void processWindow();
const char* predToStr(int pred);

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(SEMI_AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

static ADXL343 accelerometer;
Timer SensorTicker(2, readSensorAndBuffer);


// ---- I²C addresses & regs ----
#define MPU_ADDRESS       0x68
#define MPU_PWR_MGMT_1    0x6B
#define MPU_SMPLRT_DIV    0x19
#define MPU_CONFIG        0x1A
#define MPU_GYRO_CONFIG   0x1B
#define MPU_ACCEL_CONFIG  0x1C
#define MPU_ACCEL_XOUT_H  0x3B

// setup() runs once, when the device is first turned on
void setup() {
  // Initialize I2C
    Wire.begin();
    Wire.setClock(400000); // Set I2C speed to 400 kHz

    // MPU-9250 init sequence
    Wire.beginTransmission(MPU_ADDRESS);
    Wire.write(MPU_PWR_MGMT_1); Wire.write(0x00); Wire.endTransmission();
    Wire.beginTransmission(MPU_ADDRESS);
    Wire.write(MPU_SMPLRT_DIV);
    Wire.write(0x01); // Sample rate = 1 kHz / (1 + 1) = 500 Hz
    Wire.endTransmission();
    Wire.beginTransmission(MPU_ADDRESS);
    Wire.write(MPU_CONFIG);     Wire.write(0x06); Wire.endTransmission();
    Wire.beginTransmission(MPU_ADDRESS);
    Wire.write(MPU_GYRO_CONFIG);Wire.write(0x18); Wire.endTransmission();
    Wire.beginTransmission(MPU_ADDRESS);
    Wire.write(MPU_ACCEL_CONFIG);Wire.write(0x00); Wire.endTransmission();
    Log.info("MPU-9250 ready");

    // Initialize the differential pressure sensor
    initSensor();

    // Initialize the ADXL343
    Log.info("Initializing ADXL343...");
    if (!accelerometer.begin()) {
        Log.error("Failed to initialize ADXL343.");
    }
    Log.info("Accelerometer object address: %p", &accelerometer);

    SensorTicker.start();
    Log.info("Sensor ticker started.");
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  processWindow();
}

void readSensorAndBuffer() {
    SensorSample sample;

    // Read accelerometer data
    accelerometer.readAccelerationG(&sample.accX, &sample.accY, &sample.accZ);

    // Read the differential pressure from the sensor
    sample.pressure = readDifferentialPressure();


    //MPU-9250 accel+gyro | accel redundant data
    uint8_t bufI2C[14];
    Wire.beginTransmission(MPU_ADDRESS);
    Wire.write(MPU_ACCEL_XOUT_H);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDRESS, (uint8_t)14);
    for(int i=0; i<14; ++i) bufI2C[i] = Wire.read();

    sample.gyroX = ((bufI2C[8] << 8) | bufI2C[9]);
    sample.gyroY = ((bufI2C[10] << 8) | bufI2C[11]);
    sample.gyroZ = ((bufI2C[12] << 8) | bufI2C[13]);

    // Fill bufferA
    bufferA[indexA++] = sample;
    if (indexA == WINDOW_SIZE) {
        bufferAFull = true;
        indexA = 0;
    }

    // Fill bufferB, start after STEP_SIZE samples
    if (sampleCount >= STEP_SIZE) {
        bufferB[indexB++] = sample;
        if (indexB == WINDOW_SIZE) {
            bufferBFull = true;
            indexB = 0;
        }
    }

    sampleCount++;
}

void processWindow() {
    if (bufferAFull) {
        // Extract relevant fields from bufferA
        for (int i = 0; i < WINDOW_SIZE; ++i) {
            accY[i] = bufferA[i].accY;
            accZ[i] = bufferA[i].accZ;
            pressure[i] = bufferA[i].pressure;
            gyroY[i] = bufferA[i].gyroY;
            gyroZ[i] = bufferA[i].gyroZ;
        }

        bufferAFull = false;
        sampleCount = 0; // Reset sampleCount after processing bufferA

        Features_t featuresA;
        extract_features(accY, accZ, pressure, gyroY, gyroZ, WINDOW_SIZE, &featuresA);

        // Predict using the random forest model
        int prediction = random_forest_model_int16_predict((int16_t*)&featuresA, WINDOW_SIZE);
        Log.info("Buffer A Prediction: %d (%s)", prediction, predToStr(prediction));
    }

    if (bufferBFull) {
        // Extract relevant fields from bufferB
        for (int i = 0; i < WINDOW_SIZE; ++i) {
            accY[i] = bufferB[i].accY;
            accZ[i] = bufferB[i].accZ;
            pressure[i] = bufferB[i].pressure;
            gyroY[i] = bufferB[i].gyroY;
            gyroZ[i] = bufferB[i].gyroZ;
        }
        bufferBFull = false;
        Features_t featuresB;
        extract_features(accY, accZ, pressure, gyroY, gyroZ, WINDOW_SIZE, &featuresB);

        // Predict using the random forest model
        int prediction = random_forest_model_int16_predict((int16_t*)&featuresB, WINDOW_SIZE);
        Log.info("Buffer B Prediction: %d (%s)", prediction, predToStr(prediction));
    }
    // Reset sample count after processing both buffers
    if (bufferAFull || bufferBFull) {
        sampleCount = 0;
    }
}

const char* predToStr(int pred) {
    switch (pred) {
        case 0: return "ret";
        case 1: return "lav";
        case 2: return "hoj";
        default: return "ukendt";
    }
}