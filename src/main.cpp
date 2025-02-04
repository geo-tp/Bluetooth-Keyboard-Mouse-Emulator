#include <M5Cardputer.h>
#include "bluetooth.h"
#include "display.h"
#include "usbHid.h"
#include <USB.h>

bool mouseMode = true;
bool usbMode = true;
bool lastBluetoothStatus = false;

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

    displayMainScreen(usbMode, mouseMode, getBluetoothStatus());
}

void loop() {
    M5Cardputer.update();

    // For BT connection status change
    auto bluetoothStatus = getBluetoothStatus();
    if (lastBluetoothStatus != bluetoothStatus) {
        modeIndicator(usbMode, bluetoothStatus);
        lastBluetoothStatus = bluetoothStatus;
    }

    // Switch between keyboard/mouse
    if (M5Cardputer.BtnA.isPressed()) {
        mouseMode = !mouseMode;
        drawDeviceRect(mouseMode);
        delay(200);
    }

    if (usbMode) {
        handleUsbMode(mouseMode);
    } else {
        handleBluetoothMode(mouseMode);
    }

}
