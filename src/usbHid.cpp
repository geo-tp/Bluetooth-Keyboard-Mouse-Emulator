#include "usbHid.h"

#include "hid_keys.h"
#include <USB.h>

USBHIDMouse mouse;
USBHIDKeyboard keyboard;

void initUsbHid() {
    mouse.begin();
    keyboard.begin();
    USB.begin();
}

void handleUsbMode(bool mouseMode, uint8_t mouseSpeed, uint8_t mouseRotation) {
    if (mouseMode) {
        usbMouse(mouseSpeed, mouseRotation);
    } else {
        usbKeyboard();
    }
    delay(5);
}

void usbMouse(uint8_t mouseSpeed, uint8_t mouseRotation) {
    if (!M5Cardputer.Keyboard.isPressed()) {
        mouse.release(MOUSE_BUTTON_LEFT);
        mouse.release(MOUSE_BUTTON_RIGHT);
        return;
    }

    const MouseDelta delta = readMouseInput(mouseSpeed, mouseRotation);

    if (delta.buttons & 0x01) {
        mouse.press(MOUSE_BUTTON_LEFT);
    } else {
        mouse.release(MOUSE_BUTTON_LEFT);
    }

    if (delta.buttons & 0x02) {
        mouse.press(MOUSE_BUTTON_RIGHT);
    } else {
        mouse.release(MOUSE_BUTTON_RIGHT);
    }

    if (delta.x != 0 || delta.y != 0) {
        mouse.move(delta.x, delta.y);
    }
}

void usbKeyboard() {
    if (!M5Cardputer.Keyboard.isChange()) {
        return;
    }

    if (!M5Cardputer.Keyboard.isPressed()) {
        keyboard.releaseAll();
        return;
    }

    Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

    KeyReport report = {0};
    fillKeyboardModifiers(report.modifiers, status);
    fillKeyboardKeycodes(report.keys, status);

    keyboard.sendReport(&report);
    keyboard.releaseAll();
}
