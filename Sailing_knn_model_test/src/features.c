#include "features.h"

// Function to calculate the mean of an array
float mean_c(const float* samples, size_t length) {
    float sum = 0.0f;
    for (size_t i = 0; i < length; i++) {
        sum += samples[i];
    }
    return sum / (float)length;
}

// Function to calculate the minimum of an array
float min_c(const float* samples, size_t length) {
    float min_val = samples[0];
    for (size_t i = 1; i < length; i++) {
        if (samples[i] < min_val) {
            min_val = samples[i];
        }
    }
    return min_val;
}

// max
float max_c(const float* samples, size_t length) {
    float max_val = samples[0];
    for (size_t i = 1; i < length; i++) {
        if (samples[i] > max_val) max_val = samples[i];
    }
    return max_val;
}

// Function to calculate the median of an array
float median_c(const float* samples, size_t length) {
    float sorted[length];
    memcpy(sorted, samples, length * sizeof(float));
    for (size_t i = 0; i < length - 1; i++) {
        for (size_t j = i + 1; j < length; j++) {
            if (sorted[i] > sorted[j]) {
                float temp = sorted[i];
                sorted[i] = sorted[j];
                sorted[j] = temp;
            }
        }
    }
    if (length % 2 == 0) {
        return (sorted[length / 2 - 1] + sorted[length / 2]) / 2.0f;
    } else {
        return sorted[length / 2];
    }
}

// Function to calculate the energy of an array
float energy_c(const float* samples, size_t length) {
    float energy = 0.0f;
    for (size_t i = 0; i < length; i++) {
        energy += samples[i] * samples[i];
    }
    return energy / length;
}

// Function to calculate the Signal Magnitude Area (SMA)
float calculate_sma(const float* accX, const float* accY, const float* accZ, size_t length) {
    float sma = 0.0f;
    for (size_t i = 0; i < length; i++) {
        sma += fabsf(accX[i]) + fabsf(accY[i]) + fabsf(accZ[i]);
    }
    return sma / length;
}

// Function to calculate the number of pressure peaks
float calculate_pressure_peaks(const float* pressure, size_t length) {
    float peak_count = 0.0f;
    for (size_t i = 1; i < length; i++) {
        if (pressure[i] > 0.5 && pressure[i - 1] <= 0.5) {
            peak_count += 1.0f;
        }
    }
    return peak_count;
}

// Function to extract all features
void extract_features(const float* accY, const float* accZ, const float* pressure, const float* gyroY, const float* gyroZ, size_t length, Features_t* features) {
    features->y_mean = mean_c(accY, length);
    features->z_mean = mean_c(accZ, length);
    features->pressure_mean = mean_c(pressure, length);
    features->gyroY_mean = mean_c(gyroY, length);
    features->gyroZ_mean = mean_c(gyroZ, length);

    features->pressure_min = min_c(pressure, length);
    features->y_min = min_c(accY, length);

    features->y_max = max_c(accY, length);
    features->pressure_max = max_c(pressure, length);

    features->pressure_median = median_c(pressure, length);

    features->y_energy = energy_c(accY, length);
    features->pressure_energy = energy_c(pressure, length);

    features->acc_gyro_ratio_y = features->y_mean / (features->gyroY_mean + 1e-6f);
    features->acc_gyro_ratio_z = features->z_mean / (features->gyroZ_mean + 1e-6f);
}