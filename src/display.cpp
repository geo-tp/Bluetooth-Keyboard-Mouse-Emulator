#include "display.h"

namespace {
constexpr uint32_t kDisplayIdleMs = 45000;
constexpr uint32_t kBatteryUpdateMs = 30000;

uint8_t displayBrightness = 128;
unsigned long displayLastActivityMs = 0;
unsigned long lastBatteryReadMs = 0;
bool displayIsOn = true;
int8_t lastBatteryLevel = -100;

uint16_t batteryColor(int8_t level) {
    if (level < 0) {
        return TFT_DARKGREY;
    }
    if (level <= 15) {
        return TFT_RED;
    }
    if (level <= 30) {
        return TFT_YELLOW;
    }
    return TFT_GREEN;
}

int8_t readBatteryLevel() {
    const int32_t level = M5Cardputer.Power.getBatteryLevel();
    if (level < 0 || level > 100) {
        return -1;
    }
    return static_cast<int8_t>(level);
}
}  // namespace

void displayMarkActivity() {
    displayLastActivityMs = millis();
}

void displayBatteryIndicator() {
    const int8_t level = readBatteryLevel();
    lastBatteryLevel = level;
    lastBatteryReadMs = millis();

    constexpr int x = 200;
    constexpr int y = 11;
    M5Cardputer.Display.fillRect(x, y, 40, 18, TFT_LIGHTGREY);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(batteryColor(level));
    M5Cardputer.Display.setCursor(x + 4, y + 4);
    if (level < 0) {
        M5Cardputer.Display.print("--%");
    } else {
        M5Cardputer.Display.printf("%3d%%", level);
    }
}

void displayUpdateBattery(bool force) {
    if (!displayIsOn) {
        return;
    }

    const uint32_t now = millis();
    const int8_t level = readBatteryLevel();
    if (!force && level == lastBatteryLevel && now - lastBatteryReadMs < kBatteryUpdateMs) {
        return;
    }

    displayBatteryIndicator();
}

void drawDeviceRect(bool mouseMode) {
    if (mouseMode) {
        M5Cardputer.Display.drawRoundRect(10, 70, M5Cardputer.Display.width() / 2 - 15,
                                          M5Cardputer.Display.height() - 80, 3, TFT_WHITE);
        M5Cardputer.Display.drawRoundRect(M5Cardputer.Display.width() / 2 + 5, 70,
                                          M5Cardputer.Display.width() / 2 - 15,
                                          M5Cardputer.Display.height() - 80, 3, TFT_GREEN);
    } else {
        M5Cardputer.Display.drawRoundRect(10, 70, M5Cardputer.Display.width() / 2 - 15,
                                          M5Cardputer.Display.height() - 80, 3, TFT_GREEN);
        M5Cardputer.Display.drawRoundRect(M5Cardputer.Display.width() / 2 + 5, 70,
                                          M5Cardputer.Display.width() / 2 - 15,
                                          M5Cardputer.Display.height() - 80, 3, TFT_WHITE);
    }
}

void drawMouseIcon(uint8_t x, uint8_t y) {
    const uint8_t w = 25;
    const uint8_t h = 35;

    M5Cardputer.Display.fillRoundRect(x, y, w, h, 5, TFT_WHITE);
    M5Cardputer.Display.drawLine(x + w / 2, y, x + w / 2, y + h / 2, TFT_BLACK);
}

void drawKeyboardIcon(uint8_t x, uint8_t y) {
    M5Cardputer.Display.fillRect(x, y, 40, 20, TFT_WHITE);

    for (uint8_t col = 0; col < 5; ++col) {
        M5Cardputer.Display.fillRect(x + 2 + col * 8, y + 2, 6, 6, TFT_BLACK);
        M5Cardputer.Display.fillRect(x + 2 + col * 8, y + 10, 6, 6, TFT_BLACK);
    }

    M5Cardputer.Display.drawRect(x, y, 40, 20, TFT_WHITE);
}

void modeIndicator(bool usbMode, bool bluetoothStatus) {
    M5Cardputer.Display.setTextSize(1);

    if (bluetoothStatus || usbMode) {
        M5Cardputer.Display.fillRoundRect(10, 39, 104, 20, TFT_BLACK);
        M5Cardputer.Display.drawRoundRect(10, 39, 104, 20, 5, TFT_GREEN);
        M5Cardputer.Display.setTextColor(TFT_GREEN);
    } else {
        M5Cardputer.Display.fillRoundRect(10, 39, 104, 20, TFT_BLACK);
        M5Cardputer.Display.drawRoundRect(10, 39, 104, 20, 5, TFT_RED);
        M5Cardputer.Display.setTextColor(TFT_RED);
    }

    if (usbMode) {
        M5Cardputer.Display.setCursor(50, 45);
        M5Cardputer.Display.print("USB");
    } else {
        M5Cardputer.Display.setCursor(23, 45);
        M5Cardputer.Display.print("Bluetooth");
    }
}

void displayMouseStatus(uint8_t mouseSpeed, uint8_t mouseRotation) {
    M5Cardputer.Display.fillRect(150, 98, 88, 20, TFT_BLACK);
    M5Cardputer.Display.setTextColor(TFT_LIGHTGREY);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setCursor(152, 99);
    M5Cardputer.Display.printf("Spd:%u", mouseSpeed);
    M5Cardputer.Display.setCursor(152, 109);
    M5Cardputer.Display.printf("Rot:%u", static_cast<unsigned>(mouseRotation) * 90U);
}

void displayRebootMessage() {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    M5Cardputer.Display.setTextColor(TFT_GREEN);
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setCursor(24, 50);
    M5Cardputer.Display.print("Rebooting...");
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(TFT_LIGHTGREY);
    M5Cardputer.Display.setCursor(10, 90);
    M5Cardputer.Display.print("Press Enter in Launcher");
}

void setupDisplay() {
    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    M5Cardputer.Display.setTextColor(TFT_BLACK);

    displayBrightness = M5Cardputer.Display.getBrightness();
    if (displayBrightness == 0) {
        displayBrightness = 128;
    }
    M5Cardputer.Display.setBrightness(displayBrightness);
    displayLastActivityMs = millis();
    displayIsOn = true;
}

void displayWelcomeScreen() {
    M5Cardputer.Display.drawRect(9, 47, 220, 40, TFT_LIGHTGRAY);
    M5Cardputer.Display.setTextColor(TFT_LIGHTGRAY);
    M5Cardputer.Display.setCursor(18, 58);
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.print("M5-Keyboard-Mouse");

    M5Cardputer.Display.setCursor(52, 120);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.print("Version 1.2 - Alexey");

    delay(1500);
}

void displayMainScreen(bool usbMode, bool mouseMode, bool bluetoothStatus, uint8_t mouseSpeed,
                       uint8_t mouseRotation) {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    M5Cardputer.Display.fillRoundRect(10, 10, M5Cardputer.Display.width() - 20, 20, 5, TFT_LIGHTGREY);
    M5Cardputer.Display.setCursor(19, 13);
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setTextColor(TFT_BLACK);
    M5Cardputer.Display.print("M5-Keyboard-Mouse");

    M5Cardputer.Display.setTextColor(TFT_LIGHTGREY);
    M5Cardputer.Display.drawRoundRect(123, 39, 106, 20, 5, TFT_LIGHTGREY);
    M5Cardputer.Display.setCursor(136, 45);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.print("G0 switch");

    drawDeviceRect(mouseMode);
    drawMouseIcon(165, 80);
    drawKeyboardIcon(42, 87);
    modeIndicator(usbMode, bluetoothStatus);
    displayMouseStatus(mouseSpeed, mouseRotation);

    M5Cardputer.Display.setTextColor(TFT_DARKGREY);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setCursor(6, 106);
    M5Cardputer.Display.print(",.;/ or Fn move");
    M5Cardputer.Display.setCursor(6, 116);
    M5Cardputer.Display.print("Fn+-/= 9/0 spd");

    displayBatteryIndicator();
}

void displaySelectionScreen(bool usbMode) {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(TFT_LIGHTGRAY);
    M5Cardputer.Display.setCursor(78, 4);
    M5Cardputer.Display.print("Select mode");
    M5Cardputer.Display.setTextSize(2);

    if (usbMode) {
        M5Cardputer.Display.fillRect(20, 18, 200, 32, TFT_LIGHTGRAY);
        M5Cardputer.Display.drawRect(20, 18, 200, 32, TFT_BLACK);
        M5Cardputer.Display.setTextColor(TFT_BLACK);
    } else {
        M5Cardputer.Display.fillRect(20, 18, 200, 32, TFT_BLACK);
        M5Cardputer.Display.drawRect(20, 18, 200, 32, TFT_LIGHTGRAY);
        M5Cardputer.Display.setTextColor(TFT_LIGHTGRAY);
    }
    M5Cardputer.Display.setCursor(95, 26);
    M5Cardputer.Display.print("USB");

    if (!usbMode) {
        M5Cardputer.Display.fillRect(20, 56, 200, 32, TFT_LIGHTGRAY);
        M5Cardputer.Display.drawRect(20, 56, 200, 32, TFT_BLACK);
        M5Cardputer.Display.setTextColor(TFT_BLACK);
    } else {
        M5Cardputer.Display.fillRect(20, 56, 200, 32, TFT_BLACK);
        M5Cardputer.Display.drawRect(20, 56, 200, 32, TFT_LIGHTGRAY);
        M5Cardputer.Display.setTextColor(TFT_LIGHTGRAY);
    }
    M5Cardputer.Display.setCursor(24, 64);
    M5Cardputer.Display.print("Bluetooth");

    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(TFT_DARKGREY);
    M5Cardputer.Display.setCursor(52, 96);
    M5Cardputer.Display.print("; . switch");
    M5Cardputer.Display.setCursor(68, 106);
    M5Cardputer.Display.print("Enter save");
}

void displayUpdatePowerSave(bool usbMode, bool mouseMode, bool bluetoothStatus, uint8_t mouseSpeed,
                            uint8_t mouseRotation) {
    const bool userActive = M5Cardputer.Keyboard.isPressed() || M5Cardputer.BtnA.isPressed();

    if (userActive) {
        if (!displayIsOn) {
            M5Cardputer.Display.setBrightness(displayBrightness);
            displayIsOn = true;
            displayMainScreen(usbMode, mouseMode, bluetoothStatus, mouseSpeed, mouseRotation);
        } else {
            displayUpdateBattery(true);
        }
        displayLastActivityMs = millis();
        return;
    }

    if (!displayIsOn) {
        return;
    }

    if (millis() - displayLastActivityMs >= kDisplayIdleMs) {
        M5Cardputer.Display.setBrightness(0);
        displayIsOn = false;
    }
}
