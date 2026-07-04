#ifndef USBHID_H
#define USBHID_H

#include "USBHIDMouse.h"
#include "USBHIDKeyboard.h"
#include <M5Cardputer.h>

void initUsbHid();
void usbMouse(uint8_t mouseSpeed, uint8_t mouseRotation);
void usbKeyboard();
void handleUsbMode(bool mouseMode, uint8_t mouseSpeed, uint8_t mouseRotation);

#endif
