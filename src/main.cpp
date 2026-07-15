#include "bluetooth.h"
#include <M5Cardputer.h>
#include "display.h"
#include "usbHid.h"
#include <USB.h>

bool mouseMode = true;
bool usbMode = true;
bool motionMode = false;
bool scrollMode = false;
bool lastBluetoothStatus = false;

bool capsLock = false;
bool fnLock = false;
bool lastShiftState = false;
bool lastOptState = false;

float baselineAccelX = 0;
float baselineAccelY = 0;
uint8_t screenBrightness = 80;

void selectMode() {
    bool lastMode = !usbMode;
    while (true) {
        M5Cardputer.update();

        if (lastMode != usbMode) {
            displaySelectionScreen(usbMode);
            lastMode = usbMode;
        }

        if (M5Cardputer.Keyboard.isChange()) {
            if (M5Cardputer.Keyboard.isPressed()) {
                Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

                if(M5Cardputer.Keyboard.isKeyPressed('.') || M5Cardputer.Keyboard.isKeyPressed(';')) {
                    usbMode = !usbMode;
                }

                if (status.enter) {
                    break;
                }
            }
        }
        delay(10);
    }
}

void setup() {
    // Initialisation du M5Cardputer
    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true);
    
    setupDisplay();
    displayWelcomeScreen();

    selectMode();
    if (usbMode) {
        USB.begin();
    } else {
        initBluetooth();
    }

    displayMainScreen(usbMode, mouseMode, getBluetoothStatus(), capsLock, fnLock, motionMode, scrollMode);
}

void loop() {
    M5Cardputer.update();

    // For BT connection status change
    auto bluetoothStatus = getBluetoothStatus();
    if (lastBluetoothStatus != bluetoothStatus) {
        displayMainScreen(usbMode, mouseMode, bluetoothStatus, capsLock, fnLock, motionMode, scrollMode);
        lastBluetoothStatus = bluetoothStatus;
    }

    Keyboard_Class::KeysState keysState = M5Cardputer.Keyboard.keysState();
    
    // Handle screen brightness adjust combo (Fn + '-' to dim, Fn + '=' to brighten)
    if (keysState.fn && M5Cardputer.Keyboard.isPressed()) {
        static unsigned long lastBrightnessChange = 0;
        if (millis() - lastBrightnessChange > 150) {
            if (M5Cardputer.Keyboard.isKeyPressed('-')) {
                screenBrightness = (screenBrightness > 20) ? (screenBrightness - 20) : 10;
                M5Cardputer.Display.setBrightness(screenBrightness);
                lastBrightnessChange = millis();
            } else if (M5Cardputer.Keyboard.isKeyPressed('=')) {
                screenBrightness = (screenBrightness < 235) ? (screenBrightness + 20) : 255;
                M5Cardputer.Display.setBrightness(screenBrightness);
                lastBrightnessChange = millis();
            }
        }
    }

    // Handle Caps Lock and Fn Lock only in Keyboard Mode
    if (!mouseMode) {
        if (keysState.fn && keysState.shift && !lastShiftState) {
            capsLock = !capsLock;
            displayMainScreen(usbMode, mouseMode, getBluetoothStatus(), capsLock, fnLock, motionMode, scrollMode);
        }
        if (keysState.fn && keysState.opt && !lastOptState) {
            fnLock = !fnLock;
            displayMainScreen(usbMode, mouseMode, getBluetoothStatus(), capsLock, fnLock, motionMode, scrollMode);
        }
    }
    lastShiftState = keysState.shift;
    lastOptState = keysState.opt;

    // Switch between IMU and Arrow Keys for mouse movement
    if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
        if (M5Cardputer.Keyboard.isKeyPressed('m') && mouseMode && M5.Imu.isEnabled()) {
            motionMode = !motionMode;
            if (motionMode) {
                scrollMode = false;
                auto imuData = M5.Imu.getImuData();
                baselineAccelX = imuData.accel.x;
                baselineAccelY = imuData.accel.y;
            }
            displayMainScreen(usbMode, mouseMode, getBluetoothStatus(), capsLock, fnLock, motionMode, scrollMode);
        }
        if (M5Cardputer.Keyboard.isKeyPressed('s') && mouseMode && M5.Imu.isEnabled()) {
            scrollMode = !scrollMode;
            if (scrollMode) motionMode = false;
            displayMainScreen(usbMode, mouseMode, getBluetoothStatus(), capsLock, fnLock, motionMode, scrollMode);
        }
    }

    // Switch between keyboard/mouse
    if (M5Cardputer.BtnA.wasPressed()) {
        mouseMode = !mouseMode;
        // Reset all states on mode switch (locks, movements, colors)
        capsLock = false;
        fnLock = false;
        motionMode = false;
        scrollMode = false;
        displayMainScreen(usbMode, mouseMode, getBluetoothStatus(), capsLock, fnLock, motionMode, scrollMode);
        delay(200);
    }

    if (usbMode) {
        handleUsbMode(mouseMode, motionMode, scrollMode, capsLock, fnLock);
    } else {
        handleBluetoothMode(mouseMode, motionMode, scrollMode, capsLock, fnLock);
    }
}
