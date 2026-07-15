#include "display.h"

void drawMouseIcon(uint8_t x, uint8_t y, uint16_t color) {
    uint8_t w = 25;
    uint8_t h = 35;
    M5Cardputer.Display.fillRoundRect(x, y, 25, 35, 5, color);
    M5Cardputer.Display.drawLine(x+w/2, y, x+w/2, y+h/2, TFT_BLACK);
}

void drawKeyboardIcon(uint8_t x, uint8_t y, uint16_t color) {
    M5Cardputer.Display.fillRect(x, y, 40, 20, color);
    for (int col = 0; col < 5; col++) {
        M5Cardputer.Display.fillRect(x + 2 + col * 8, y + 2, 6, 6, TFT_BLACK);
        M5Cardputer.Display.fillRect(x + 2 + col * 8, y + 10, 6, 6, TFT_BLACK);
    }
    M5Cardputer.Display.drawLine(x, y, x, y + 20, color);
    M5Cardputer.Display.drawLine(x + 40, y, x + 40, y + 20, color);
    M5Cardputer.Display.drawLine(x, y, x + 40, y, color);
    M5Cardputer.Display.drawLine(x, y + 20, x + 40, y + 20, color);
}

void modeIndicator(bool usbMode, bool bluetoothStatus) {
    M5Cardputer.Display.setTextSize(1.6);
    
    if (bluetoothStatus || usbMode) {
        M5Cardputer.Display.drawRoundRect(10, 39, 104, 20, 5, TFT_GREEN);
        M5Cardputer.Display.setTextColor(TFT_GREEN);
    } else {
        M5Cardputer.Display.drawRoundRect(10, 39, 104, 20, 5, TFT_RED);
        M5Cardputer.Display.setTextColor(TFT_RED);
    }

    if (usbMode) {
        M5Cardputer.Display.setCursor(50, 43);
        M5Cardputer.Display.print("USB");
    } else {
        M5Cardputer.Display.setCursor(23, 43);
        M5Cardputer.Display.print("Bluetooth");
    }
}

void setupDisplay() {
    M5Cardputer.Display.setRotation(1);
    // Dim the screen light to prevent eye strain (default comfortable level: 80 out of 255)
    M5Cardputer.Display.setBrightness(80);
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    M5Cardputer.Display.setTextColor(TFT_BLACK);
}

void displayWelcomeScreen() {
    M5Cardputer.Display.drawRect(9, 47, 220, 40, TFT_LIGHTGRAY);
    M5Cardputer.Display.setTextColor(TFT_LIGHTGRAY);
    M5Cardputer.Display.setCursor(18, 58);
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.printf("M5-Keyboard-Mouse");

    M5Cardputer.Display.setCursor(70, 120);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.printf("Version 1.2 - Geo"); 

    delay(2000);
}

void displayMainScreen(bool usbMode, bool mouseMode, bool bluetoothStatus, bool capsLock, bool fnLock, bool motionMode, bool scrollMode) {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    M5Cardputer.Display.fillRoundRect(10, 10, M5Cardputer.Display.width()-20, 20, 5, TFT_LIGHTGREY); // Around main title
    M5Cardputer.Display.setCursor(19, 13);
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setTextColor(TFT_BLACK);
    M5Cardputer.Display.print("M5-Keyboard-Mouse"); // Main title

    // Go switch frame
    M5Cardputer.Display.setTextColor(TFT_LIGHTGREY);
    M5Cardputer.Display.drawRoundRect(123, 39, 106, 20, 5, TFT_LIGHTGREY);
    M5Cardputer.Display.setCursor(136, 43);
    M5Cardputer.Display.setTextSize(1.6);
    M5Cardputer.Display.print("GO switch");

    // Dynamic colors for screen drawing indicators
    uint16_t kbColor = TFT_WHITE;
    uint16_t mColor = TFT_WHITE;

    if (!mouseMode) {
        if (capsLock && fnLock) kbColor = TFT_PURPLE;
        else if (capsLock) kbColor = TFT_BLUE;
        else if (fnLock) kbColor = TFT_RED;
        else kbColor = TFT_GREEN;
    } else {
        if (motionMode) mColor = TFT_YELLOW;
        else if (scrollMode) mColor = TFT_MAGENTA;
        else mColor = TFT_GREEN;
    }

    uint16_t w = M5Cardputer.Display.width() / 2 - 15;
    uint16_t h = M5Cardputer.Display.height() - 80;

    M5Cardputer.Display.drawRoundRect(10, 70, w, h, 3, kbColor);
    M5Cardputer.Display.drawRoundRect(M5Cardputer.Display.width() / 2 + 5, 70, w, h, 3, mColor);

    drawKeyboardIcon(42, 87, kbColor);
    drawMouseIcon(165, 80, mColor);
    
    modeIndicator(usbMode, bluetoothStatus);
}

void displaySelectionScreen(bool mode) {
    M5Cardputer.Display.clear();
    M5Cardputer.Display.setTextSize(1.5);
    M5Cardputer.Display.setTextColor(TFT_LIGHTGRAY);
    M5Cardputer.Display.setCursor(70, 10);
    M5Cardputer.Display.printf("Select Mode:");
    M5Cardputer.Display.setTextSize(3);

    // USB SELECTED
    if (mode) {
        M5Cardputer.Display.fillRect(20, 30, 200, 40, TFT_LIGHTGRAY);
        M5Cardputer.Display.drawRect(20, 30, 200, 40, TFT_BLACK);
        M5Cardputer.Display.setTextColor(TFT_BLACK);
    } else {
        M5Cardputer.Display.fillRect(20, 30, 200, 40, TFT_BLACK);
        M5Cardputer.Display.drawRect(20, 30, 200, 40, TFT_LIGHTGRAY);
        M5Cardputer.Display.setTextColor(TFT_LIGHTGRAY);
    }
    M5Cardputer.Display.setCursor(95, 40);
    M5Cardputer.Display.printf("USB");

    // BT SELECTED
    if (!mode) {
        M5Cardputer.Display.fillRect(20, 80, 200, 40, TFT_LIGHTGRAY);
        M5Cardputer.Display.drawRect(20, 80, 200, 40, TFT_BLACK);
        M5Cardputer.Display.setTextColor(TFT_BLACK);
    } else {
        M5Cardputer.Display.fillRect(20, 80, 200, 40, TFT_BLACK);
        M5Cardputer.Display.drawRect(20, 80, 200, 40, TFT_LIGHTGRAY);
        M5Cardputer.Display.setTextColor(TFT_LIGHTGRAY);
    }
    M5Cardputer.Display.setCursor(42, 90);
    M5Cardputer.Display.printf("Bluetooth");
}