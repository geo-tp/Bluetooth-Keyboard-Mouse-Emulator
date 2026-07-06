#include "imuMouse.h"
#include <M5Cardputer.h>
#include <math.h>

namespace {
// Accelerometer values are in g. Treat tilt as a 2D joystick vector:
// hold a tilt to keep moving in that direction, return to level to stop.
// Tune these if the real device feels too slow/fast, drifts, or an axis is inverted.
constexpr float TILT_DEADZONE_G = 0.10f;
constexpr float TILT_FULL_SCALE_G = 0.75f;
constexpr float SPEED_CURVE = 1.7f;
constexpr float FILTER_ALPHA = 0.28f;
constexpr int MAX_DELTA = 28;

float filteredX = 0.0f;
float filteredY = 0.0f;
bool imuReady = false;

float clampFloat(float value, float minValue, float maxValue)
{
    if (value < minValue) return minValue;
    if (value > maxValue) return maxValue;
    return value;
}

int8_t clampToMouseDelta(float value)
{
    if (value > MAX_DELTA) return MAX_DELTA;
    if (value < -MAX_DELTA) return -MAX_DELTA;
    return static_cast<int8_t>(value);
}
} // namespace

void setupImuMouse()
{
    imuReady = M5.Imu.isEnabled();
    filteredX = 0.0f;
    filteredY = 0.0f;
}

MouseDelta readImuMouseDelta()
{
    MouseDelta delta = {0, 0};

    if (!imuReady) {
        imuReady = M5.Imu.isEnabled();
        if (!imuReady) {
            return delta;
        }
    }

    float ax = 0.0f;
    float ay = 0.0f;
    float az = 0.0f;
    if (!M5.Imu.getAccel(&ax, &ay, &az)) {
        return delta;
    }

    // Cardputer-Adv held in normal landscape orientation.
    // Use ax for horizontal and ay for vertical; X is inverted to match the real device feel.
    // Change the signs here if the real device direction feels inverted.
    float stickX = -ax;
    float stickY = -ay;

    // Circular deadzone keeps diagonal/all-direction movement natural.
    float magnitude = sqrtf((stickX * stickX) + (stickY * stickY));
    float rawX = 0.0f;
    float rawY = 0.0f;

    if (magnitude > TILT_DEADZONE_G) {
        float usable = (magnitude - TILT_DEADZONE_G) / (TILT_FULL_SCALE_G - TILT_DEADZONE_G);
        usable = clampFloat(usable, 0.0f, 1.0f);

        float speed = powf(usable, SPEED_CURVE) * MAX_DELTA;
        float directionX = stickX / magnitude;
        float directionY = stickY / magnitude;

        rawX = directionX * speed;
        rawY = directionY * speed;
    }

    filteredX = (FILTER_ALPHA * rawX) + ((1.0f - FILTER_ALPHA) * filteredX);
    filteredY = (FILTER_ALPHA * rawY) + ((1.0f - FILTER_ALPHA) * filteredY);

    delta.x = clampToMouseDelta(filteredX);
    delta.y = clampToMouseDelta(filteredY);
    return delta;
}
