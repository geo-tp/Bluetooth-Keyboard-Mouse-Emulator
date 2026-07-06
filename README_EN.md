# M5Stack CardPuter ADV — Bluetooth / USB Keyboard & Gyro Mouse Emulator

## Overview

This project turns the **M5Stack CardPuter ADV (SKU K132-Adv)** into a **USB / Bluetooth dual-mode HID keyboard and mouse emulator**. You can plug it into a computer as a wireless keyboard & mouse, or connect it via Bluetooth to phones, tablets, TVs, and other devices.

The core highlight of this project is **motion-controlled mouse input**: instead of nudging the cursor with arrow keys one step at a time, the device uses its built-in **IMU accelerometer** to sense tilt angle, allowing you to move the cursor naturally and fluidly — like holding an air mouse in your hand.

---

## Supported Device

| Device Model | MCU | Notes |
|-------------|-----|-------|
| **M5Stack CardPuter ADV** | ESP32-S3 | Target device for this project; onboard IMU, keyboard, and display |

---

## Key Features

### 🔄 Dual-Mode Connectivity
- **USB HID Mode**: Plug-and-play via USB-C, no drivers required
- **Bluetooth BLE HID Mode**: Wirelessly connect to phones, tablets, smart TVs, computers, and more

### 🖱️ Motion-Controlled Mouse
- Uses the CardPuter ADV's built-in **IMU accelerometer** for gyro-style motion control
- **Tilt the device to move the cursor** — greater tilt equals faster cursor movement
- Built-in **deadzone filtering**: slight wobbling won't cause cursor drift
- **Low-pass smoothing filter**: cursor movement is fluid and jitter-free
- **Non-linear speed curve**: fine-grained positioning at small tilt angles, rapid movement at large tilt angles

### ⌨️ Keyboard Mode
- Preserves all physical keystrokes from the CardPuter ADV's native keyboard
- Supports modifier keys (Ctrl / Shift / Alt)
- Supports Space and other standard keys

### 🔀 One-Button Toggle
- Press the **GO (BtnA) button** to instantly switch between keyboard mode and mouse mode
- Clear on-screen status indicators

### 🖥️ On-Screen Display
- **Welcome screen**: Shows project name and version
- **Mode selection screen**: Choose USB or Bluetooth mode at startup
- **Main screen**: Real-time display of current mode (keyboard/mouse), connection type (USB/Bluetooth), and Bluetooth connection status

### 🖲️ Mouse Button Mapping
| Physical Key | Mouse Function |
|-------------|----------------|
| Enter / OK | Left click |
| `\` backslash key | Right click |

---

## Usage

1. **Power on** the CardPuter ADV
2. **Select connection mode**:
   - Press `.` or `;` to toggle between USB mode and Bluetooth mode
   - Press **Enter** to confirm your selection
3. **Toggle between keyboard and mouse**: Press the **GO (BtnA) button** to switch modes
4. **Mouse mode**: Tilt the device to move the cursor; press Enter for left click, `\` for right click
5. **Keyboard mode**: Use the physical keyboard normally

---

## Technical Details

### Motion Mouse Algorithm

```
Accelerometer data → Deadzone filter → Tilt-to-velocity vector → Non-linear speed curve → Low-pass filter → HID mouse report
```

- **Deadzone**: Tilts below 0.10g are ignored to prevent accidental triggering from hand shake
- **Full Scale**: 0.75g maps to maximum movement speed; values beyond this are clamped
- **Speed Curve**: Non-linear mapping with exponent 1.7, balancing precise positioning with rapid movement
- **Low-Pass Filter**: EMA filter with α=0.28, smoothing raw accelerometer data
- **Max Step**: Capped at ±28 pixels per report to prevent cursor jumping

### HID Report Protocol

The project implements standard HID mouse reports (4 bytes: buttons + X + Y + wheel) and HID keyboard reports (8 bytes: modifier + reserved + 6 keycodes), compatible with all major operating systems (Windows / macOS / Linux / Android / iOS).

---

## Firmware Flashing

Pre-compiled firmware binary:

```text
cardputer-adv-imu-keyboard-mouse.bin
```

### Flashing Methods

**Method 1: PlatformIO (Recommended)**
```bash
# After cloning the project
pio run --target upload
```

**Method 2: esptool Command Line**
```bash
esptool.py --chip esp32s3 --port /dev/ttyUSB0 write_flash 0x0 cardputer-adv-imu-keyboard-mouse.bin
```

**Method 3: M5Burner or Other GUI Tools**
Flash the `.bin` file to address `0x0`.

---

## Project Structure

```
├── platformio.ini           # PlatformIO project config (ESP32-S3, 8MB Flash)
├── src/
│   ├── main.cpp             # Entry point, mode selection & main loop
│   ├── bluetooth.cpp/.h     # Bluetooth BLE HID implementation (mouse + keyboard reports)
│   ├── usbHid.cpp/.h        # USB HID implementation (mouse + keyboard reports)
│   ├── imuMouse.cpp/.h      # Motion mouse algorithm (IMU reading + filtering + speed mapping)
│   └── display.cpp/.h       # Display UI (mode selection, status display, icon rendering)
└── README.md
```

---

## Build Dependencies

| Library | Description |
|---------|-------------|
| `M5Cardputer` | M5Stack CardPuter official hardware support library |
| `FastLED` | LED indicator control (optional) |
| ESP32 BLE HID libraries | Built into the Arduino ESP32 framework |

Build platform: `espressif32@6.7.0` + Arduino framework, target chip ESP32-S3, Flash 8MB.

---

## FAQ

**Q: Can't connect via Bluetooth?**
A: Make sure the target device supports the BLE HID protocol. If previously paired, remove (forget) the device on the target side and re-pair.

**Q: Mouse cursor drifts?**
A: Place the device flat and still for 1–2 seconds; the IMU will stabilize automatically. If drift persists, increase the `TILT_DEADZONE_G` parameter in `imuMouse.cpp`.

**Q: Mouse movement is too fast / too slow?**
A: Adjust the `TILT_FULL_SCALE_G` (full-scale threshold) or `SPEED_CURVE` (speed curve exponent) parameters in `imuMouse.cpp`.

**Q: Mouse direction is inverted?**
A: Flip the sign of `stickX` / `stickY` in `imuMouse.cpp` to invert the X/Y axis.
