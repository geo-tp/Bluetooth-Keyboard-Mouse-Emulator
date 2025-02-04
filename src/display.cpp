#include "display.h"


void drawDeviceRect(bool reverse) {
    if (reverse) {
        // GREEN WHITE
        M5Cardputer.Display.drawRoundRect(10, 70, M5Cardputer.Display.width() / 2 - 15, M5Cardputer.Display.height() - 80, 3, TFT_WHITE); // Keyboard mode rectangle
        M5Cardputer.Display.drawRoundRect(M5Cardputer.Display.width() / 2 + 5, 70, M5Cardputer.Display.width() / 2 - 15, M5Cardputer.Display.height() - 80,  3, TFT_GREEN);  // Mouse mode rectangle
    } else {
        // WHITE GREEN
        M5Cardputer.Display.drawRoundRect(10, 70, M5Cardputer.Display.width() / 2 - 15, M5Cardputer.Display.height() - 80, 3, TFT_GREEN);
        M5Cardputer.Display.drawRoundRect(M5Cardputer.Display.width() / 2 + 5, 70, M5Cardputer.Display.width() / 2 - 15, M5Cardputer.Display.height() - 80,  3, TFT_WHITE); 
    }
}

void drawMouseIcon(uint8_t x, uint8_t y) {
    uint8_t w = 25;
    uint8_t h = 35;

    // Corps de la souris
    M5Cardputer.Display.fillRoundRect(x, y, 25, 35, 5, TFT_WHITE);

    // Ligne boutons
    M5Cardputer.Display.drawLine(x+w/2, y, x+w/2, y+h/2, TFT_BLACK);
}

void drawKeyboardIcon(uint8_t x, uint8_t y) {
    // Dessiner le contour du clavier
    M5Cardputer.Display.fillRect(x, y, 40, 20, TFT_WHITE);

    // Dessiner les touches du clavier
    M5Cardputer.Display.fillRect(x + 2, y + 2, 6, 6, TFT_BLACK);   // Touche 1
    M5Cardputer.Display.fillRect(x + 10, y + 2, 6, 6, TFT_BLACK);  // Touche 2
    M5Cardputer.Display.fillRect(x + 18, y + 2, 6, 6, TFT_BLACK);  // Touche 3
    M5Cardputer.Display.fillRect(x + 26, y + 2, 6, 6, TFT_BLACK);  // Touche 4
    M5Cardputer.Display.fillRect(x + 34, y + 2, 6, 6, TFT_BLACK);  // Touche 5

    M5Cardputer.Display.fillRect(x + 2, y + 10, 6, 6, TFT_BLACK);  // Touche 6
    M5Cardputer.Display.fillRect(x + 10, y + 10, 6, 6, TFT_BLACK); // Touche 7
    M5Cardputer.Display.fillRect(x + 18, y + 10, 6, 6, TFT_BLACK); // Touche 8
    M5Cardputer.Display.fillRect(x + 26, y + 10, 6, 6, TFT_BLACK); // Touche 9
    M5Cardputer.Display.fillRect(x + 34, y + 10, 6, 6, TFT_BLACK); // Touche 10

    // Dessiner les lignes pour les contours
    M5Cardputer.Display.drawLine(x, y, x, y + 20, TFT_WHITE);         // Ligne gauche
    M5Cardputer.Display.drawLine(x + 40, y, x + 40, y + 20, TFT_WHITE); // Ligne droite
    M5Cardputer.Display.drawLine(x, y, x + 40, y, TFT_WHITE);         // Ligne haut
    M5Cardputer.Display.drawLine(x, y + 20, x + 40, y + 20, TFT_WHITE);  // Ligne bas
}

void modeIndicator(bool usbMode, bool bluetoothStatus) {
    M5Cardputer.Display.setTextSize(1.6);
    
    if (bluetoothStatus || usbMode) {
        // Green
        M5Cardputer.Display.drawRoundRect(10, 39, 104, 20, 5, TFT_GREEN);
        M5Cardputer.Display.setTextColor(TFT_GREEN);

    } else {
        // Red
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
    M5Cardputer.Display.printf("Version 1.1 - Geo"); 

    delay(2000);
}

void displayMainScreen(bool usbMode, bool mouseMode, bool bluetoothStatus) {
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

    drawDeviceRect(true);
    drawMouseIcon(165, 80);
    drawKeyboardIcon(42, 87);
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