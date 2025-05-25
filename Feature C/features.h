#ifndef FEATURES_H
#define FEATURES_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef struct {
    float y_mean;
    float z_mean;
    float pressure_mean;
    float gyroY_mean;
    float gyroZ_mean;
    float pressure_min;
    float y_min;
    float y_max;
    float pressure_max;
    float pressure_median;
    float y_energy;
    float pressure_energy;
    float acc_gyro_ratio_y;
    float acc_gyro_ratio_z;
} Features_t;

// Function prototypes
float mean_c(const float* samples, size_t length);
float min_c(const float* samples, size_t length);
float median_c(const float* samples, size_t length);
float energy_c(const float* samples, size_t length);
float calculate_sma(const float* accX, const float* accY, const float* accZ, size_t length);
float calculate_pressure_peaks(const float* pressure, size_t length);
void extract_features(const float* accY, const float* accZ, const float* pressure, const float* gyroY, const float* gyroZ, size_t length, Features_t* features);

#endif