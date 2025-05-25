#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "features.h"

#define MAX_SAMPLES 10000
#define MAX_LINE_LENGTH 256

typedef struct {
    double timestamp;
    float accX, accY, accZ, pressure, gyroX, gyroY, gyroZ;
} SensorData;

int main(int argc, char *argv[]) {
    printf("---------------- Feature calculation in C -----------------\n");
    printf("\t Usage:\n");
    printf("\t  ./features <file.csv> \n\n\n");

    if (argc != 2) {
        printf("Usage: %s <csv_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        printf("Error opening file\n");
        return 1;
    }

    SensorData data[MAX_SAMPLES];
    char line[MAX_LINE_LENGTH];
    int n = 0;

    // Skip header
    fgets(line, MAX_LINE_LENGTH, file);

    // Read data
    while (fgets(line, MAX_LINE_LENGTH, file) && n < MAX_SAMPLES) {
        sscanf(line, "%lf,%f,%f,%f,%f,%f,%f,%f",
               &data[n].timestamp, &data[n].accX, &data[n].accY, &data[n].accZ, &data[n].pressure, &data[n].gyroX, &data[n].gyroY, &data[n].gyroZ);
        n++;
    }
    fclose(file);

    // Separate arrays for each sensor
    float *accX_data = (float *)malloc(n * sizeof(float));
    float *accY_data = (float *)malloc(n * sizeof(float));
    float *accZ_data = (float *)malloc(n * sizeof(float));
    float *pressure_data = (float *)malloc(n * sizeof(float));
    float *gyroX_data = (float *)malloc(n * sizeof(float));
    float *gyroY_data = (float *)malloc(n * sizeof(float));
    float *gyroZ_data = (float *)malloc(n * sizeof(float));

    for (int i = 0; i < n; i++) {
        accX_data[i] = data[i].accX;
        accY_data[i] = data[i].accY;
        accZ_data[i] = data[i].accZ;
        pressure_data[i] = data[i].pressure;
        gyroX_data[i] = data[i].gyroX;
        gyroY_data[i] = data[i].gyroY;
        gyroZ_data[i] = data[i].gyroZ;
    }

    // We'll store the extracted features in this struct
    Features_t features;

    // ...existing code...
        // Call the feature extraction function
        extract_features(accY_data, accZ_data, pressure_data, gyroY_data, gyroZ_data, n, &features);
    
        // Print all features
        printf("=== Feature Extraction from %d samples ===\n", n);
        printf("y_mean: %f\n", features.y_mean);
        printf("z_mean: %f\n", features.z_mean);
        printf("pressure_mean: %f\n", features.pressure_mean);
        printf("gyroY_mean: %f\n", features.gyroY_mean);
        printf("gyroZ_mean: %f\n", features.gyroZ_mean);
        printf("pressure_min: %f\n", features.pressure_min);
        printf("y_min: %f\n", features.y_min);
        printf("y_max: %f\n", features.y_max);
        printf("pressure_max: %f\n", features.pressure_max);
        printf("pressure_median: %f\n", features.pressure_median);
        printf("y_energy: %f\n", features.y_energy);
        printf("pressure_energy: %f\n", features.pressure_energy);
        printf("acc_gyro_ratio_y: %f\n", features.acc_gyro_ratio_y);
        printf("acc_gyro_ratio_z: %f\n", features.acc_gyro_ratio_z);
    // ...existing code...

    // Free allocated memory
    free(accX_data);
    free(accY_data);
    free(accZ_data);
    free(pressure_data);
    free(gyroX_data);
    free(gyroY_data);
    free(gyroZ_data);

    return 0;
}