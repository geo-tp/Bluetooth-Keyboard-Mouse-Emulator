#include "bluetooth.h"

BLEHIDDevice* hid;
BLECharacteristic* mouseInput;
BLECharacteristic* keyboardInput;
bool bluetoothIsConnected = false;
static bool btReady = false;

// Conforming C++ private access trick to bypass NimBLE duplicate UUID check
template<typename Tag, typename Tag::type M>
struct Rob {
    friend typename Tag::type get(Tag) {
        return M;
    }
};

struct BLEService_characteristicMap {
    typedef BLECharacteristicMap BLEService::*type;
    friend type get(BLEService_characteristicMap);
};

template struct Rob<BLEService_characteristicMap, &BLEService::m_characteristicMap>;

static unsigned long lastBtMouseUpdate = 0;
static unsigned long btArrowHoldTime = 0;
static uint8_t lastBtButtons = 0;
static uint8_t lastBtModifiers = 0;
static uint8_t lastBtKeys[6] = {0};

extern float baselineAccelX;
extern float baselineAccelY;

void MyBLEServerCallbacks::onConnect(BLEServer* pServer) {
    bluetoothIsConnected = true;
}

void MyBLEServerCallbacks::onDisconnect(BLEServer* pServer) {
    bluetoothIsConnected = false;
    if (btReady) {
        pServer->startAdvertising(); // restart advertising
    }
}

bool getBluetoothStatus() {
    return bluetoothIsConnected;
}

void bluetoothMouse(bool motionMode, bool scrollMode) {
    if (!bluetoothIsConnected) return;

    if (millis() - lastBtMouseUpdate < 10) return;
    lastBtMouseUpdate = millis();

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
            if (btArrowHoldTime == 0) btArrowHoldTime = millis();
            unsigned long held = millis() - btArrowHoldTime;
            float multiplier = 1.0f + (held / 100.0f);
            if (multiplier > 10.0f) multiplier = 10.0f;
            moveX = (int)(moveX * multiplier);
            moveY = (int)(moveY * multiplier);
        } else {
            btArrowHoldTime = 0;
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
    if (leftClick) currentButtons |= 0x01;
    if (rightClick) currentButtons |= 0x02;
    if (middleClick) currentButtons |= 0x04;

    if (isMoving || currentButtons != lastBtButtons) {
        uint8_t report[4] = {currentButtons, (uint8_t)moveX, (uint8_t)moveY, (uint8_t)scroll};
        mouseInput->setValue(report, sizeof(report));
        if (btReady && bluetoothIsConnected) {
            mouseInput->notify();
        }
        lastBtButtons = currentButtons;
    }
}

void bluetoothKeyboard(bool capsLock, bool fnLock) {
    if (!bluetoothIsConnected) return;
    
    Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

    uint8_t modifiers = status.modifiers;
    if (status.opt) {
        modifiers |= 0x08; // Map Opt to Left GUI (Super Key)
    }
    
    bool fnActive = status.fn ^ fnLock;

    bool hasLetter = false;
    uint8_t keys[6] = {0};
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

        keys[idx++] = mappedKey;
    }

    if (capsLock && hasLetter) {
        modifiers ^= 0x02; // Shift toggle for Caps Lock
    }

    if (status.space) {
        const uint8_t HID_SPACE = 0x2C;
        bool present = false;
        for (uint8_t i = 0; i < idx; ++i) if (keys[i] == HID_SPACE) { present = true; break; }
        if (!present && idx < 6) keys[idx++] = HID_SPACE;
    }

    if (modifiers != lastBtModifiers || memcmp(keys, lastBtKeys, 6) != 0) {
        uint8_t report[8] = {modifiers, 0, keys[0], keys[1], keys[2], keys[3], keys[4], keys[5]};
        keyboardInput->setValue(report, sizeof(report));
        if (btReady && bluetoothIsConnected) {
            keyboardInput->notify();
        }
        lastBtModifiers = modifiers;
        memcpy(lastBtKeys, keys, 6);
    }
}

void deinitBluetooth() {
    BLEDevice::deinit();
    btReady = false;
}

void sendEmptyReports() {
    uint8_t emptyMouseReport[4] = {0, 0, 0, 0};
    mouseInput->setValue(emptyMouseReport, sizeof(emptyMouseReport));
    if (btReady && bluetoothIsConnected) mouseInput->notify();

    uint8_t emptyKeyboardReport[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    keyboardInput->setValue(emptyKeyboardReport, sizeof(emptyKeyboardReport));
    if (btReady && bluetoothIsConnected) keyboardInput->notify();
}

void handleBluetoothMode(bool mouseMode, bool motionMode, bool scrollMode, bool capsLock, bool fnLock) {
    static bool lastMouseMode = true;
    static unsigned long lastSwitchTime = 0;

    if (mouseMode != lastMouseMode) {
        lastMouseMode = mouseMode;
        lastSwitchTime = millis();
        lastBtModifiers = 0;
        memset(lastBtKeys, 0, 6);
        lastBtButtons = 0;
        if (bluetoothIsConnected) {
            sendEmptyReports();
        }
        return; 
    }

    if (millis() - lastSwitchTime < 100) return;

    if (bluetoothIsConnected) {
        if (mouseMode) {
            bluetoothMouse(motionMode, scrollMode);
        } else {
            bluetoothKeyboard(capsLock, fnLock);
        }
    }
}

void initBluetooth() {
    // Preserve original branding name: "M5-Keyboard-Mouse"
    BLEDevice::init("M5-Keyboard-Mouse");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyBLEServerCallbacks());

    hid = new BLEHIDDevice(pServer);
    mouseInput = hid->inputReport(1);    // Mouse Input report
    keyboardInput = hid->inputReport(2);   // Keyboard Input report
 
    // Force register keyboardInput in service map to bypass NimBLE's duplicate UUID check
    BLEService *service = hid->hidService();
    BLECharacteristicMap &map = service->*get(BLEService_characteristicMap());
    map.setByUUID(keyboardInput, keyboardInput->getUUID());

    hid->manufacturer()->setValue("M5Stack");
    hid->pnp(0x02, 0x1234, 0x5678, 0x0100);
    hid->hidInfo(0x00, 0x01);
    hid->reportMap((uint8_t*)HID_REPORT_MAP, sizeof(HID_REPORT_MAP));
    hid->startServices();

    btReady = true;

    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->setAppearance(HID_KEYBOARD);
    pAdvertising->addServiceUUID(hid->hidService()->getUUID());
    pAdvertising->start();
}
