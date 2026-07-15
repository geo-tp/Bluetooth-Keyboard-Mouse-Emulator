#ifndef USBHID_H
#define USBHID_H

#include <USB.h>
#include <USBHIDMouse.h>
#include <USBHIDKeyboard.h>
#include <M5Cardputer.h>

extern USBHIDMouse mouse;
extern USBHIDKeyboard keyboard;

void handleUsbMode(bool mouseMode, bool motionMode, bool scrollMode, bool capsLock, bool fnLock);
void usbMouse(bool motionMode, bool scrollMode);
void usbKeyboard(bool capsLock, bool fnLock);

#endif // USBHID_H