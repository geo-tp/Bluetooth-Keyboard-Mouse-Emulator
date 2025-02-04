
#ifndef DISPLAY_H
#define DSPLAY_H

#include <M5Cardputer.h>

void setupDisplay();
void displayWelcomeScreen();
void displaySelectionScreen(bool mouseMode);
void displayMainScreen(bool usbMode, bool mouseMode, bool bluetoothStatus);
void modeIndicator(bool usbMode, bool bluetoothStatus);
void drawDeviceRect(bool reverse);

#endif
