/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "DifferentialPressureSensor.h" // Include the sensor header
#include "adxl343.h" // Include the ADXL343 library

extern "C" {
    #include "driver_mpu9250.h"
}

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

// Define the desired sample rate in Hz
#define SAMPLE_RATE 8000

// Forward declarations for I2C and utility functions
//uint8_t i2c_init(void);
//uint8_t i2c_deinit(void);
//uint8_t i2c_read(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len);
//uint8_t i2c_write(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len);

//void delay_ms(uint32_t ms);
//void debug_print(const char *const fmt, ...);

// Forward declarations for other functions
void readSensorData();
void buttonHandler(system_event_t event, int data);
int startReadingFromAPI(String command);
int stopReadingFromAPI(String command);
void cleanup();
int resetDevice(String command);

ADXL343 accelerometer;
//mpu9250_handle_t mpuHandle;
Timer accelerometerTicker(2, readSensorData);

// ---- I²C addresses & regs ----
#define MPU_ADDRESS       0x68
#define MPU_PWR_MGMT_1    0x6B
#define MPU_SMPLRT_DIV    0x19
#define MPU_CONFIG        0x1A
#define MPU_GYRO_CONFIG   0x1B
#define MPU_ACCEL_CONFIG  0x1C
#define MPU_ACCEL_XOUT_H  0x3B

unsigned long ms, msStart;
float accelX = 0.0f;
float accelY = 0.0f;
float accelZ = 0.0f;

// Variable to store the last sampled pressure
float lastPressure = 0.0;

// Create a buffer to store the data
char buffer[100];

// If you don't hit the setup button to stop recording, this is how long to go before turning it
// off automatically. The limit really is only the disk space available to receive the file.
const unsigned long MAX_RECORDING_LENGTH_MS = 30000;

// This is the IP Address and port that the server.js node server is running on.
IPAddress serverAddr = IPAddress(192,168,4,188); // **UPDATE THIS**
int serverPort = 7123;

TCPClient client;
unsigned long recordingStart;

enum State { STATE_WAITING, STATE_CONNECT, STATE_RUNNING, STATE_FINISH };
State state = STATE_WAITING;

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

    // Initialize the MPU9250 handle
    /*DRIVER_MPU9250_LINK_INIT(&mpuHandle, mpu9250_handle_t);
    DRIVER_MPU9250_LINK_IIC_INIT(&mpuHandle, i2c_init);
    DRIVER_MPU9250_LINK_IIC_DEINIT(&mpuHandle, i2c_deinit);
    DRIVER_MPU9250_LINK_IIC_READ(&mpuHandle, i2c_read);
    DRIVER_MPU9250_LINK_IIC_WRITE(&mpuHandle, i2c_write);
    DRIVER_MPU9250_LINK_DELAY_MS(&mpuHandle, delay_ms);
    DRIVER_MPU9250_LINK_DEBUG_PRINT(&mpuHandle, debug_print);

    // Set the interface to I2C
    if (mpu9250_set_interface(&mpuHandle, MPU9250_INTERFACE_IIC) != 0) {
        Log.error("Failed to set MPU9250 interface to I2C.");
        return;
    }

    // Set the I2C address
    if (mpu9250_set_addr_pin(&mpuHandle, MPU9250_ADDRESS_AD0_LOW) != 0) {
        Log.error("Failed to set MPU9250 I2C address.");
        return;
    }

    delay_ms(100); // Wait for the sensor to stabilize

    // Initialize the MPU9250
    if (mpu9250_init(&mpuHandle) != 0) {
        Log.error("Failed to initialize MPU9250.");
    } else {
        Log.info("MPU9250 initialized successfully.");
    }

    delay_ms(100); // Wait for the sensor to stabilize*/

    // Initialize the differential pressure sensor
    initSensor();

    // Initialize the ADXL343
    Log.info("Initializing ADXL343...");
    if (!accelerometer.begin()) {
        Log.error("Failed to initialize ADXL343.");
    }

    msStart = millis();

    // Register handler to handle clicking on the SETUP button
    System.on(button_click, buttonHandler);

    // Register Particle functions to start/stop reading via API
    Particle.function("startReading", startReadingFromAPI);
    Particle.function("stopReading", stopReadingFromAPI);
    Particle.function("resetDevice", resetDevice);

    // Blue D7 LED indicates recording is on
    pinMode(D7, OUTPUT);
    digitalWrite(D7, LOW);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The main loop can handle other tasks while the Timer runs in the background
  switch(state) {
    case STATE_WAITING:
    //Log.info("Waiting for button press...");
      // Waiting for the user to press the SETUP button. The setup button handler
      // will bump the state into STATE_CONNECT
      break;
  
    case STATE_CONNECT:
      // Ready to connect to the server via TCP
      if (client.connect(serverAddr, serverPort)) {
        // Connected
        Log.info("starting");
  
        recordingStart = millis();
        digitalWrite(D7, HIGH);
  
        state = STATE_RUNNING;
      }
      else {
        Log.info("failed to connect to server");
        state = STATE_WAITING;
      }
      break;
  
    case STATE_RUNNING:
        if (!accelerometerTicker.isActive()) {
            accelerometerTicker.start(); // Start the Timer
        }

        if (millis() - recordingStart >= MAX_RECORDING_LENGTH_MS) {
            state = STATE_FINISH;
        }
        break;
  
    case STATE_FINISH:
      digitalWrite(D7, LOW);
      client.stop();
      Log.info("stopping");

      if (accelerometerTicker.isActive()) {
          accelerometerTicker.stop(); // Stop the Timer
      }

      //cleanup();

      state = STATE_WAITING;
      break;
    }
}

void readSensorData() {
    // Add timestamp in millis
    ms = millis() - msStart;

    // Read accelerometer data
    accelerometer.readAccelerationG(&accelX, &accelY, &accelZ);

    // Read gyroscope data from MPU9250
    //int16_t gyroRaw[1][3]; // Array for raw gyroscope data
    //float gyroDps[1][3];   // Array for gyroscope data in degrees per second

    /*if (mpu9250_read(&mpuHandle, NULL, NULL, gyroRaw, gyroDps, NULL, NULL, NULL) != 0) {
        Log.error("Failed to read gyroscope data.");
    } else {
        Log.info("Gyro: X: %.2f, Y: %.2f, Z: %.2f", gyroDps[0][0], gyroDps[0][1], gyroDps[0][2]);
    }*/

    // Read the differential pressure from the sensor
    lastPressure = readDifferentialPressure();

    // Print the sampled data to the terminal (optional for debugging)
    //Log.info("Pressure: %.2f, Gyro: X: %.2f, Y: %.2f, Z: %.2f", lastPressure, gyroDps[0][0], gyroDps[0][1], gyroDps[0][2]);
    //Log.info("Accel: X: %.3f g, Y: %.3f g, Z: %.3f g", accelX, accelY, accelZ);

    // 3) MPU-9250 accel+gyro
    uint8_t bufI2C[14];
    Wire.beginTransmission(MPU_ADDRESS);
    Wire.write(MPU_ACCEL_XOUT_H);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDRESS, (uint8_t)14);
    for(int i=0; i<14; ++i) bufI2C[i] = Wire.read();

    int16_t rawAx = (bufI2C[0]<<8) | bufI2C[1];
    int16_t rawAy = (bufI2C[2]<<8) | bufI2C[3];
    int16_t rawAz = (bufI2C[4]<<8) | bufI2C[5];
    int16_t rawGx = (bufI2C[8]<<8) | bufI2C[9];
    int16_t rawGy = (bufI2C[10]<<8)| bufI2C[11];
    int16_t rawGz = (bufI2C[12]<<8)| bufI2C[13];

    float ax2 = rawAx/16384.0;
    float ay2 = rawAy/16384.0;
    float az2 = rawAz/16384.0;
    float gx2 = rawGx;//131.0;
    float gy2 = rawGy;//131.0;
    float gz2 = rawGz;//131.0;

    //Log.info("gyro: X: %.2f, Y: %.2f, Z: %.2f", gx2, gy2, gz2);

    if (state == STATE_RUNNING && client.connected()) {
        // Clear the buffer before using snprintf
        

        // Send the buffer over the TCP connection
        client.write((const uint8_t*)buffer, strlen(buffer));
    } else if (!client.connected()) {
        Log.error("TCP connection lost");
        state = STATE_FINISH;
    }
    // Prepare the data to be sent
    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "%lu,%d,%d,%d,%d,%d,%d,%d\n",
                 ms,
                 int(100000 * accelX), int(100000 * accelY), int(100000 * accelZ),
                 int(lastPressure),
                 int(gx2), int(gy2), int(gz2)
                 /*int(1000 * gyroDps[0][0]), int(1000 * gyroDps[0][1]), int(1000 * gyroDps[0][2])*/);
}

// Function to handle API calls
int startReadingFromAPI(String command) {
    if (state == STATE_WAITING && WiFi.ready()) {
        state = STATE_CONNECT;
        return 1; // Success
    }
    return -1; // Failed (e.g., already running or Wi-Fi not ready)
}

int stopReadingFromAPI(String command) {
    if (state == STATE_RUNNING) {
        state = STATE_FINISH; // Stop reading
        return 1; // Success
    }
    return -1; // Failed (e.g., not currently running)
}

int resetDevice(String command) {
    System.reset();
    return 1; // Success
}

// Button handler for the SETUP button
void buttonHandler(system_event_t event, int data) {
    switch (state) {
    case STATE_WAITING:
        if (WiFi.ready()) {
            state = STATE_CONNECT;
        }
        break;

    case STATE_RUNNING:
        state = STATE_FINISH;
        break;
    }
}
/*
void cleanup() {
    if (mpu9250_deinit(&mpuHandle) != 0) {
        Log.error("Failed to deinitialize MPU9250.");
    }
}

uint8_t i2c_init(void) {
    Wire.begin(); // Initialize I2C
    return 0;     // Return 0 for success
}

uint8_t i2c_deinit(void) {
    Wire.end(); // Deinitialize I2C
    return 0;   // Return 0 for success
}

uint8_t i2c_read(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len) {
    Wire.beginTransmission(addr); // Start transmission to the specified I2C address
    Wire.write(reg);              // Write the register address
    if (Wire.endTransmission(false) != 0) { // End transmission but keep the bus active
        return 1;                 // Return error
    }

    Wire.requestFrom(addr, len); // Request `len` bytes from the device
    for (uint16_t i = 0; i < len; i++) {
        if (Wire.available()) {
            buf[i] = Wire.read(); // Read a byte
        } else {
            return 1;             // Return error if no data is available
        }
    }
    return 0; // Return 0 for success
}

uint8_t i2c_write(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len) {
    Wire.beginTransmission(addr); // Start transmission to the specified I2C address
    Wire.write(reg);              // Write the register address
    for (uint16_t i = 0; i < len; i++) {
        Wire.write(buf[i]);       // Write the data
    }
    if (Wire.endTransmission() != 0) { // End transmission
        return 1;                 // Return error
    }
    return 0; // Return 0 for success
}

void delay_ms(uint32_t ms) {
    delay(ms);
}

void debug_print(const char *const fmt, ...) {
    char buffer[128];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    Log.info("%s", buffer);
}*/