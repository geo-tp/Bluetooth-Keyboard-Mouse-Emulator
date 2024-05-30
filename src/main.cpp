#include <M5Cardputer.h>
#include "bluetooth.h"
#include "display.h"

bool mouseMode = true;

void setup() {
    // Initialisation du M5Cardputer
    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true);

    // Initialisation ecran
    setupDisplay();

    // Initialisation du clavier
    M5Cardputer.Keyboard.begin();

    // Initialisation bluetooth 
    BLEDevice::init("M5-Keyboard-Mouse"); //  nom du periphérique
    BLEServer *pServer = BLEDevice::createServer(); // BT server
    pServer->setCallbacks(new MyBLEServerCallbacks()); // callback (co/deco)
    hid = new BLEHIDDevice(pServer); // Peripherique HID BLE
    mouseInput = hid->inputReport(1); // Mouse report
    keyboardInput = hid->inputReport(2); // Keyboard report
    hid->manufacturer()->setValue("Espressif"); // Fabricant
    hid->pnp(0x02, 0x045e, 0x028e, 0x0110); // Infos HID
    hid->hidInfo(0x00, 0x01); // Infos HID
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

void loop() {
    // Update M5cardputer state for keyboard inputs
    M5Cardputer.update();

    // Bluetooth is connected
    if (isConnected) {
        int16_t x = 0; // Mouse X
        int16_t y = 0; // Mouse Y
        uint8_t buttons = 0; // Mouse buttons
        uint8_t modifier = 0; // alt ctrl shift...
        uint8_t keycode[6] = {0}; // for chars

        // Bouton GO
        if (M5Cardputer.BtnA.isPressed()) {
            mouseMode = !mouseMode;  // Toggle mouse mode
            delay(250);  // Debounce delay
            updateDisplay(mouseMode);
        }


        if (M5Cardputer.Keyboard.isPressed()) {
            Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

            if (mouseMode) {

                // ##################### Mode souris #######################

                // Boutons
                if (status.enter) {
                    buttons |= 0x01; // Bouton gauche
                }

                if (M5Cardputer.Keyboard.isKeyPressed('\\')) {
                    buttons |= 0x02; // Bouton droit
                }

                // Position souris
                if (M5Cardputer.Keyboard.isKeyPressed(';')) {
                    y-= 1;
                }
                else if (M5Cardputer.Keyboard.isKeyPressed('.')) {
                    y += 1;
                }
                else if (M5Cardputer.Keyboard.isKeyPressed('/')) {
                    x += 1;
                }
                else if (M5Cardputer.Keyboard.isKeyPressed(',')) {
                    x -= 1;
                }

                // Envoie etat bouton et position
                uint8_t report[4] = {buttons, (uint8_t)x, (uint8_t)y, 0};
                mouseInput->setValue(report, sizeof(report));
                mouseInput->notify();
            } else {

                // ############### Mode clavier ######################

                // Keys
                int count = 0;
                for (auto i : status.hid_keys) {
                    keycode[count] = i;
                    count++;
                }
                
                // Modifiers
                if (status.ctrl) {
                    modifier |= 0x01;
                }

                if (status.shift) {
                    modifier |= 0x02;
                }

                if (status.alt) {
                    modifier |= 0x03;
                }

                // Envoie du modifier et des inputs
                uint8_t report[8] = {modifier, 0, keycode[0], keycode[1], keycode[2], keycode[3], keycode[4], keycode[5]};
                keyboardInput->setValue(report, sizeof(report));
                keyboardInput->notify();
                delay(50); // Modifie la vitesse de répétition des touches claviers
            }
            ;
        } else {
            
            // ######################### Empty #############################

            uint8_t emptyMouseReport[4] = {0, 0, 0, 0};
            mouseInput->setValue(emptyMouseReport, sizeof(emptyMouseReport));
            mouseInput->notify();

            uint8_t emptyKeyboardReport[8] = {0, 0, 0, 0, 0, 0, 0, 0};
            keyboardInput->setValue(emptyKeyboardReport, sizeof(emptyKeyboardReport));
            keyboardInput->notify();
        }
        
    }

    delay(7); // Modifie la vitesse de déplacement souris
}
