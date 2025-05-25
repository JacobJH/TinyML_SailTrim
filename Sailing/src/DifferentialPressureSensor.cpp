#include "DifferentialPressureSensor.h"

// Define the ADC pin for the sensor
#define SENSOR_ADC_PIN A0

// Reference voltage for the ADC
#define ADC_REF_VOLTAGE 3.3

// ADC resolution
#define ADC_RESOLUTION 4096.0

// Voltage at zero differential pressure
#define ZERO_DIFF_VOLTAGE 1.5

// Sensitivity of the sensor (volts per unit pressure)
#define SENSOR_SENSITIVITY 0.5

void initSensor() {
    // Configure the ADC pin as input
    pinMode(SENSOR_ADC_PIN, INPUT);
}

float readDifferentialPressure() {
    // Read the raw ADC value
    int rawValue = analogRead(SENSOR_ADC_PIN);

    // Convert the raw ADC value to a voltage
    float voltage = (rawValue / ADC_RESOLUTION) * ADC_REF_VOLTAGE;

    // Calculate the differential pressure
    float differentialPressure = (voltage - ZERO_DIFF_VOLTAGE) / SENSOR_SENSITIVITY;

    return rawValue;
}
