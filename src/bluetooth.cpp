

#include "bluetooth.h"

BLEHIDDevice* hid;
BLECharacteristic* mouseInput;
BLECharacteristic* keyboardInput;
bool bluetoothIsConnected = false;

void MyBLEServerCallbacks::onConnect(BLEServer* pServer) {
    bluetoothIsConnected = true;
}

void MyBLEServerCallbacks::onDisconnect(BLEServer* pServer, esp_ble_gatts_cb_param_t *param) {
    bluetoothIsConnected = false;
    
    pServer->disconnect(param->disconnect.conn_id); // Properly disconnect the client
    pServer->startAdvertising();  // Start advertising making device discoverable
}

bool getBluetoothStatus() {
    return  bluetoothIsConnected;
}

void bluetoothMouse() {
    int16_t x = 0;         // Déplacement en X
    int16_t y = 0;         // Déplacement en Y
    uint8_t buttons = 0;   // Boutons de la souris

    Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

    // Bouton gauche
    if (status.enter) {
        buttons |= 0x01;
    }
    // Bouton droit
    if (M5Cardputer.Keyboard.isKeyPressed('\\')) {
        buttons |= 0x02;
    }

    // Vertical
    if (M5Cardputer.Keyboard.isKeyPressed(';')) {
        y -= 1;
    }
    else if (M5Cardputer.Keyboard.isKeyPressed('.')) {
        y += 1;
    }

    // Horizontal
    if (M5Cardputer.Keyboard.isKeyPressed('/')) {
        x += 1;
    }
    else if (M5Cardputer.Keyboard.isKeyPressed(',')) {
        x -= 1;
    }

    // Send
    uint8_t report[4] = {buttons, (uint8_t)x, (uint8_t)y, 0};
    mouseInput->setValue(report, sizeof(report));
    mouseInput->notify();
}

void bluetoothKeyboard() {
    uint8_t modifier = 0;
    uint8_t keycode[6] = {0};

    Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

    // Keys
    int count = 0;
    for (auto key : status.hid_keys) {
        if (count < 6) {
            keycode[count] = key;
            count++;
        }
    }

    if (M5Cardputer.Keyboard.isKeyPressed(' ') && count < 6) {
        keycode[count++] = 0x2C;  // HID SPACE
    }

    // Modifiers
    if (status.ctrl)  modifier |= 0x01;
    if (status.shift) modifier |= 0x02;
    if (status.alt)   modifier |= 0x04;

    // Send
    uint8_t report[8] = {
        modifier, 0,
        keycode[0], keycode[1], keycode[2],
        keycode[3], keycode[4], keycode[5]
    };
    keyboardInput->setValue(report, sizeof(report));
    keyboardInput->notify();

    delay(50);
}

void sendEmptyReports() {
    uint8_t emptyMouseReport[4] = {0, 0, 0, 0};
    mouseInput->setValue(emptyMouseReport, sizeof(emptyMouseReport));
    mouseInput->notify();

    uint8_t emptyKeyboardReport[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    keyboardInput->setValue(emptyKeyboardReport, sizeof(emptyKeyboardReport));
    keyboardInput->notify();
}

void handleBluetoothMode(bool mouseMode) {
    if (bluetoothIsConnected) {
        if (M5Cardputer.Keyboard.isPressed()) {
            if (mouseMode) {
                bluetoothMouse();
            } else {
                bluetoothKeyboard();
            }
        } else {
            sendEmptyReports();
        }
    }
    delay(7);
}

void initBluetooth() {
    BLEDevice::init("M5-Keyboard-Mouse");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyBLEServerCallbacks());

    // Création du périphérique HID
    hid = new BLEHIDDevice(pServer);
    mouseInput = hid->inputReport(1);    // Rapport pour la souris
    keyboardInput = hid->inputReport(2);   // Rapport pour le clavier

    // Configuration des informations du fabricant et HID
    hid->manufacturer()->setValue("M5Stack");
    hid->pnp(0x02, 0x1234, 0x5678, 0x0100);
    hid->hidInfo(0x00, 0x01);
    hid->reportMap((uint8_t*)HID_REPORT_MAP, sizeof(HID_REPORT_MAP));
    hid->startServices();

    // Configuration de la publicité BLE
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->setAppearance(HID_MOUSE);  // Apparence de souris
    pAdvertising->addServiceUUID(hid->hidService()->getUUID());
    pAdvertising->start();

    // Configuration de la sécurité BLE
    BLESecurity *pSecurity = new BLESecurity();
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);
    pSecurity->setCapability(ESP_IO_CAP_NONE);
    pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
}

void deinitBluetooth() {
    BLEDevice::deinit(); 
    delay(1000);
}
