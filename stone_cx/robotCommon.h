#pragma once

// Forward declarations
class MotorI2C;

// Functions
void driveMotorFromCPU1(MotorI2C &motor, float steerThreshold, bool display = false);