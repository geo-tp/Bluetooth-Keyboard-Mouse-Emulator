

#include "bluetooth.h"

BLEHIDDevice* hid;
BLECharacteristic* mouseInput;
BLECharacteristic* keyboardInput;
bool isConnected = false;

void deinitBluetooth() {
    BLEDevice::deinit(); // De-initialize BLE to avoid weird BT connection
    delay(1000);
}

void initBluetooth() {
    // Initialisation bluetooth
    BLEDevice::init("M5-Keyboard-Mouse"); //  nom du periphérique
    BLEServer *pServer = BLEDevice::createServer(); // BT server
    pServer->setCallbacks(new MyBLEServerCallbacks()); // callback (co/deco)
    hid = new BLEHIDDevice(pServer); // Peripherique HID BLE
    mouseInput = hid->inputReport(1); // Mouse report
    keyboardInput = hid->inputReport(2); // Keyboard report
    hid->manufacturer()->setValue("M5Stack"); // Fabricant
    hid->pnp(0x02, 0x1234, 0x5678, 0x0100); // Configurer avec ID par défaut
    hid->hidInfo(0x00, 0x01); // Infos HID Country/Flags
    hid->reportMap((uint8_t*)HID_REPORT_MAP, sizeof(HID_REPORT_MAP)); // Carte des rapports
    hid->startServices(); // start servides HID

    BLEAdvertising *pAdvertising = pServer->getAdvertising(); // Pub BLE
    pAdvertising->setAppearance(HID_MOUSE); // Définir le périphérique comme une souris
    pAdvertising->addServiceUUID(hid->hidService()->getUUID()); // Ajout UUID
    pAdvertising->start(); // Start Pub

    // Configuration de la sécurité BLE
    BLESecurity *pSecurity = new BLESecurity();
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);
    pSecurity->setCapability(ESP_IO_CAP_NONE); // Capacité I/O
    pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK); // Clé init
}
