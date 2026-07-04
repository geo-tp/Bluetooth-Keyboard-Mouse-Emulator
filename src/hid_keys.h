#ifndef HID_KEYS_H
#define HID_KEYS_H

#include <M5Cardputer.h>

struct MouseDelta {
    int16_t x = 0;
    int16_t y = 0;
    uint8_t buttons = 0;
};

void fillKeyboardModifiers(uint8_t &modifier, const Keyboard_Class::KeysState &status);
void fillKeyboardKeycodes(uint8_t keycode[6], const Keyboard_Class::KeysState &status);
MouseDelta readMouseInput(uint8_t speed, uint8_t rotation);
bool shouldRebootToLauncher(const Keyboard_Class::KeysState &status);
bool adjustMouseSpeed(uint8_t &speed, const Keyboard_Class::KeysState &status, bool wasF11, bool wasF12);
bool adjustMouseRotation(uint8_t &rotation, const Keyboard_Class::KeysState &status, bool wasF9, bool wasF10);

#endif
