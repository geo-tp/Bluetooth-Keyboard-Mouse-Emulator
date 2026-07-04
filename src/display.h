#ifndef DISPLAY_H
#define DISPLAY_H

#include <M5Cardputer.h>

void setupDisplay();
void displayWelcomeScreen();
void displaySelectionScreen(bool usbMode);
void displayMainScreen(bool usbMode, bool mouseMode, bool bluetoothStatus, uint8_t mouseSpeed,
                       uint8_t mouseRotation);
void displayMouseStatus(uint8_t mouseSpeed, uint8_t mouseRotation);
void displayRebootMessage();
void modeIndicator(bool usbMode, bool bluetoothStatus);
void drawDeviceRect(bool mouseMode);
void displayMarkActivity();
void displayBatteryIndicator();
void displayUpdateBattery(bool force = false);
void displayUpdatePowerSave(bool usbMode, bool mouseMode, bool bluetoothStatus, uint8_t mouseSpeed,
                            uint8_t mouseRotation);

#endif
