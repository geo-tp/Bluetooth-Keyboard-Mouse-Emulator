#ifndef IMU_MOUSE_H
#define IMU_MOUSE_H

#include <Arduino.h>

struct MouseDelta {
    int8_t x;
    int8_t y;
};

void setupImuMouse();
MouseDelta readImuMouseDelta();

#endif // IMU_MOUSE_H
