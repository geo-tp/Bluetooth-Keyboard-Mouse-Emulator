#include "usbHid.h"

USBHIDMouse mouse;
USBHIDKeyboard keyboard;

void handleUsbMode(bool mouseMode) {
    if (mouseMode) {
        usbMouse();
    } else  {
        usbKeyboard();
    }
    delay(5);
}
void usbMouse() {
    mouse.begin();
    int moveX = 0;
    int moveY = 0;
    if (M5Cardputer.Keyboard.isPressed()) {
        Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

        if (M5Cardputer.Keyboard.isKeyPressed('/')) {
            // droite
            moveX = 1;
        } 
        
        if (M5Cardputer.Keyboard.isKeyPressed(',')) {
            // gauche
            moveX = -1;
        }  
        
        if (M5Cardputer.Keyboard.isKeyPressed(';')) {
            // haut
            moveY = -1;
        } 
        
        if (M5Cardputer.Keyboard.isKeyPressed('.')) {
            // bas
            moveY = 1;
        }

        // clics souris
        if (status.enter) {
            // gauche
            mouse.press(MOUSE_BUTTON_LEFT);
        } else if (M5Cardputer.Keyboard.isKeyPressed('\\')) {
            // droit
            mouse.press(MOUSE_BUTTON_RIGHT);
        }
        // Send
        mouse.move(moveX, moveY);

    } else {
        mouse.release(MOUSE_BUTTON_LEFT);
        mouse.release(MOUSE_BUTTON_RIGHT);
    }
}

void usbKeyboard() {
    static bool inited = false;
    if (!inited) { keyboard.begin(); inited = true; }

    if (!M5Cardputer.Keyboard.isChange()) return;

    Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

    KeyReport report = {0};
    report.modifiers = status.modifiers;

    uint8_t idx = 0;
    for (auto k : status.hid_keys) {
        if (idx < 6) report.keys[idx++] = k;
        else break;
    }

    if (M5Cardputer.Keyboard.isKeyPressed(' ')) {
        const uint8_t HID_SPACE = 0x2C;
        bool present = false;
        for (uint8_t i = 0; i < idx; ++i) if (report.keys[i] == HID_SPACE) { present = true; break; }
        if (!present && idx < 6) report.keys[idx++] = HID_SPACE;
    }

    if (idx == 0 && report.modifiers == 0) {
        keyboard.releaseAll();
    } else {
        keyboard.sendReport(&report);
    }
}