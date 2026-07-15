#include "usbHid.h"

USBHIDMouse mouse;
USBHIDKeyboard keyboard;

static unsigned long lastUsbMouseUpdate = 0;
static unsigned long usbArrowHoldTime = 0;
static uint8_t lastUsbButtons = 0;
static uint8_t lastUsbModifiers = 0;
static uint8_t lastUsbKeys[6] = {0};

extern float baselineAccelX;
extern float baselineAccelY;

void handleUsbMode(bool mouseMode, bool motionMode, bool scrollMode, bool capsLock, bool fnLock) {
    static bool usbInited = false;
    if (!usbInited) {
        mouse.begin();
        keyboard.begin();
        usbInited = true;
    }

    if (mouseMode) {
        usbMouse(motionMode, scrollMode);
    } else {
        usbKeyboard(capsLock, fnLock);
    }
    delay(5);
}

void usbMouse(bool motionMode, bool scrollMode) {
    if (millis() - lastUsbMouseUpdate < 10) return;
    lastUsbMouseUpdate = millis();

    int moveX = 0, moveY = 0, scroll = 0;
    bool isMoving = false;

    if (motionMode && M5.Imu.isEnabled()) {
        auto imuData = M5.Imu.getImuData();
        // Correcting reversed X axis: invert rollDiff
        float rollDiff = imuData.accel.x - baselineAccelX;
        float pitchDiff = imuData.accel.y - baselineAccelY;
        
        if (abs(rollDiff) > 0.1) { moveX = (int)(-rollDiff * 20.0); isMoving = true; }
        if (abs(pitchDiff) > 0.1) { moveY = (int)(pitchDiff * 20.0); isMoving = true; }

        static unsigned long lastImuScrollTick = 0;
        if (millis() - lastImuScrollTick > 50) {
            if (M5Cardputer.Keyboard.isKeyPressed(';')) { scroll = 1; isMoving = true; lastImuScrollTick = millis(); }
            if (M5Cardputer.Keyboard.isKeyPressed('.')) { scroll = -1; isMoving = true; lastImuScrollTick = millis(); }
        }
    } else {
        if (M5Cardputer.Keyboard.isKeyPressed('/')) { moveX = 1; isMoving = true; }
        if (M5Cardputer.Keyboard.isKeyPressed(',')) { moveX = -1; isMoving = true; }
        if (M5Cardputer.Keyboard.isKeyPressed(';')) { moveY = -1; isMoving = true; }
        if (M5Cardputer.Keyboard.isKeyPressed('.')) { moveY = 1; isMoving = true; }
        
        if (isMoving) {
            if (usbArrowHoldTime == 0) usbArrowHoldTime = millis();
            unsigned long held = millis() - usbArrowHoldTime;
            float multiplier = 1.0f + (held / 100.0f);
            if (multiplier > 10.0f) multiplier = 10.0f;
            moveX = (int)(moveX * multiplier);
            moveY = (int)(moveY * multiplier);
        } else {
            usbArrowHoldTime = 0;
        }
    }

    if (scrollMode && !motionMode && M5.Imu.isEnabled()) {
        auto imuData = M5.Imu.getImuData();
        static float scrollAccumulator = 0;
        float gy = imuData.gyro.x;
        if (abs(gy) > 5.0) { // Deadzone to avoid accidental scrolling
            scrollAccumulator += (-gy) * 0.001; // Slower sensitivity
            if (abs(scrollAccumulator) >= 1.0) {
                scroll = (int)scrollAccumulator;
                scrollAccumulator -= scroll;
                isMoving = true;
            }
        } else {
            scrollAccumulator = 0;
        }
    }

    Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
    bool leftClick = status.enter;
    bool rightClick = M5Cardputer.Keyboard.isKeyPressed('\\');
    bool middleClick = M5Cardputer.Keyboard.isKeyPressed(' ');

    uint8_t currentButtons = 0;
    if (leftClick) currentButtons |= MOUSE_BUTTON_LEFT;
    if (rightClick) currentButtons |= MOUSE_BUTTON_RIGHT;
    if (middleClick) currentButtons |= MOUSE_BUTTON_MIDDLE;

    if (isMoving || currentButtons != lastUsbButtons) {
        if (leftClick && !(lastUsbButtons & MOUSE_BUTTON_LEFT)) mouse.press(MOUSE_BUTTON_LEFT);
        if (!leftClick && (lastUsbButtons & MOUSE_BUTTON_LEFT)) mouse.release(MOUSE_BUTTON_LEFT);

        if (rightClick && !(lastUsbButtons & MOUSE_BUTTON_RIGHT)) mouse.press(MOUSE_BUTTON_RIGHT);
        if (!rightClick && (lastUsbButtons & MOUSE_BUTTON_RIGHT)) mouse.release(MOUSE_BUTTON_RIGHT);

        if (middleClick && !(lastUsbButtons & MOUSE_BUTTON_MIDDLE)) mouse.press(MOUSE_BUTTON_MIDDLE);
        if (!middleClick && (lastUsbButtons & MOUSE_BUTTON_MIDDLE)) mouse.release(MOUSE_BUTTON_MIDDLE);

        if (isMoving) {
            mouse.move(moveX, moveY, scroll);
        }

        lastUsbButtons = currentButtons;
    }
}

void usbKeyboard(bool capsLock, bool fnLock) {
    Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

    KeyReport report = {0};
    report.modifiers = status.modifiers;

    if (status.opt) {
        report.modifiers |= 0x08; // Map Opt to Left GUI (Super Key)
    }

    bool fnActive = status.fn ^ fnLock;
    bool hasLetter = false;
    uint8_t idx = 0;

    for (auto k : status.hid_keys) {
        if (idx >= 6) break;
        
        uint8_t mappedKey = k;

        if (!fnActive) {
            if (k == 0x33) mappedKey = 0x52; // ; -> Up Arrow
            else if (k == 0x37) mappedKey = 0x51; // . -> Down Arrow
            else if (k == 0x36) mappedKey = 0x50; // , -> Left Arrow
            else if (k == 0x38) mappedKey = 0x4F; // / -> Right Arrow
            else if (k == 0x35) mappedKey = 0x29; // ` -> Esc
        } else {
            if (k == 0x2D || k == 0x2E) continue; // Skip '-' and '=' (consumed for screen brightness)
            if (k == 0x2A) mappedKey = 0x4C; // Backspace -> Delete
        }

        if (mappedKey >= 0x04 && mappedKey <= 0x1D) {
            hasLetter = true;
        }

        report.keys[idx++] = mappedKey;
    }

    if (capsLock && hasLetter) {
        report.modifiers ^= 0x02; // Shift toggle for Caps Lock
    }

    if (status.space) {
        const uint8_t HID_SPACE = 0x2C;
        bool present = false;
        for (uint8_t i = 0; i < idx; ++i) if (report.keys[i] == HID_SPACE) { present = true; break; }
        if (!present && idx < 6) report.keys[idx++] = HID_SPACE;
    }

    if (report.modifiers != lastUsbModifiers || memcmp(report.keys, lastUsbKeys, 6) != 0) {
        if (idx == 0 && report.modifiers == 0) {
            keyboard.releaseAll();
        } else {
            keyboard.sendReport(&report);
        }
        lastUsbModifiers = report.modifiers;
        memcpy(lastUsbKeys, report.keys, 6);
    }
}