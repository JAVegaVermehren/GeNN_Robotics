#pragma once

//------------------------------------------------------------------------
// RobotParameters
//------------------------------------------------------------------------
namespace RobotParameters
{
    const float joystickDeadzone = 0.25f;
    const int64_t targetTickMicroseconds = (int64_t)(20.0 * 1000.0) - 10;
}
