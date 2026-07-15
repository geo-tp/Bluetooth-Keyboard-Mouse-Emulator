#ifndef DISPLAY_H
#define DISPLAY_H

#include <M5Cardputer.h>

void setupDisplay();
void displayWelcomeScreen();
void displaySelectionScreen(bool mouseMode);
void displayMainScreen(bool usbMode, bool mouseMode, bool bluetoothStatus, bool capsLock, bool fnLock, bool motionMode, bool scrollMode);
void modeIndicator(bool usbMode, bool bluetoothStatus);

#endif
