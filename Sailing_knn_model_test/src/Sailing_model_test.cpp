/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "features.h"
#include "k_neighbor_model.h"

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

// Function prototypes
void readSensorAndBuffer();
void processWindow();

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(SEMI_AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

// setup() runs once, when the device is first turned on
void setup() {
  // set up sensors
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  readSensorAndBuffer();
  processWindow();
}

// Example feature calculation: mean
double calculateMean(const double* buffer, int size) {
    double sum = 0.0;
    for (int i = 0; i < size; ++i) {
        sum += buffer[i];
    }
    return sum / size;
}

void readSensorAndBuffer() {
    // Replace with your actual sensor readings
    SensorSample sample;
    sample.accX = 1; // Example, will be replaced with real sensor code
    sample.accY = 1;
    sample.accZ = 1;
    sample.pressure = 1;
    sample.gyroX = 1;
    sample.gyroY = 1;
    sample.gyroZ = 1;

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
    // Helper arrays for feature extraction
    float accY[WINDOW_SIZE], accZ[WINDOW_SIZE], pressure[WINDOW_SIZE], gyroY[WINDOW_SIZE], gyroZ[WINDOW_SIZE];

    if (bufferAFull) {
        // Extract relevant fields from bufferA
        for (int i = 0; i < WINDOW_SIZE; ++i) {
            accY[i] = bufferA[i].accY;
            accZ[i] = bufferA[i].accZ;
            pressure[i] = bufferA[i].pressure;
            gyroY[i] = bufferA[i].gyroY;
            gyroZ[i] = bufferA[i].gyroZ;
        }
        Features_t featuresA;
        extract_features(accY, accZ, pressure, gyroY, gyroZ, WINDOW_SIZE, &featuresA);

        // Predict using k neighbor model
        int prediction = k_neighbor_model_predict((float*)&featuresA, WINDOW_SIZE);
        Log.info("Prediction: %d", prediction);

        bufferAFull = false;
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
        Features_t featuresB;
        extract_features(accY, accZ, pressure, gyroY, gyroZ, WINDOW_SIZE, &featuresB);

        // Predict using k neighbor model
        int prediction = k_neighbor_model_predict((float*)&featuresB, WINDOW_SIZE);
        Log.info("Prediction: %d", prediction);

        bufferBFull = false;
    }
}