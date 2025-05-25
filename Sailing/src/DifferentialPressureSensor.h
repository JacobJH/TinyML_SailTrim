#ifndef DIFFERENTIAL_PRESSURE_SENSOR_H
#define DIFFERENTIAL_PRESSURE_SENSOR_H

#include "Particle.h"

// Initializes the differential pressure sensor
void initSensor();

// Reads the differential pressure value
float readDifferentialPressure();

#endif // DIFFERENTIAL_PRESSURE_SENSOR_H