#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>

struct AppSettings {
    bool usbMode = true;
    bool mouseMode = true;
    uint8_t mouseSpeed = 3;     // 1..10
    uint8_t mouseRotation = 0;  // 0=0°, 1=90°, 2=180°, 3=270°
};

void settingsLoad(AppSettings &settings);
void settingsSave(const AppSettings &settings);

#endif
