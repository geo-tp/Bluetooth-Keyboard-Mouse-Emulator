# M5 Keyboard and Mouse Emulator

This project implements a robust USB HID and BLE HID device using the **M5Cardputer**, allowing you to emulate a fully functional keyboard and mouse. The device supports seamless toggling between Keyboard and Mouse modes, custom key mappings, status color notification via screen drawings, screen dimming, and IMU-based motion cursor control/scrolling.

## 🚀 Key Improvements & Stability Upgrades (v1.2)

This version contains major fixes and improvements over the original release:
- **NimBLE Library Stability**: Fixed the fatal `BLECharacteristic::notify` assertion crash caused by NimBLE's UUID lookup limitation. A custom C++ template bypass is implemented to register duplicate report characteristics (like keyboard and mouse HID endpoints under the same service) cleanly.
- **Scroll Wheel Support**: Added native vertical scroll wheel capability over both BLE and USB HID descriptors.
- **Yaw + Roll Gyro Movement**: Panning the cursor horizontally is now much easier because Yaw (wrist twisting) and Roll (wrist tilting) gyroscopic inputs are combined to fight cable tension.
- **Local Key Consuming**: Hotkeys like screen dimming are intercepted locally and never leak characters to the target machine.

---

## 🎨 Screen Color Status Indicators

Feedback for active locks and modes is drawn dynamically on the screen itself by changing the colors of the Keyboard and Mouse panels:

### Keyboard Panel Colors (Left Rect & Icon)
- **Solid Green**: Active Keyboard mode (no locks).
- **Solid Blue**: Caps Lock active (Toggle with `Fn` + `Shift`).
- **Solid Red**: Fn Lock active (Toggle with `Fn` + `Opt`).
- **Solid Purple**: Both Caps Lock and Fn Lock active.
- **Solid White**: Inactive mode (Keyboard is selected off).

### Mouse Panel Colors (Right Rect & Icon)
- **Solid Yellow**: Motion Mode active (Toggle with `m`).
- **Solid Magenta/Pink**: Scroll Mode active (Toggle with `s`).
- **Solid Green**: Active Mouse mode (standard key movement).
- **Solid White**: Inactive mode (Mouse is selected off).

---

## 🕹️ Control Layout & Keybindings

### 1. General Controls
- **Toggle Mode**: Press the physical **Btn GO** (BtnA on the side of the Cardputer) to swap between **Keyboard Mode** and **Mouse Mode**.
- **Adjust Brightness**:
  - Hold `Fn` and press `-` to **dim** the screen backlight (down to 10).
  - Hold `Fn` and press `=` to **brighten** the screen backlight (up to 255).

---

### 2. Keyboard Mode Layout

By default, typing is fully mapped 1-to-1. Special mappings are designed for convenient one-handed use:

| Key Press (Fn Inactive) | Action / Output |
| --- | --- |
| `;` | **Up Arrow** |
| `.` | **Down Arrow** |
| `,` | **Left Arrow** |
| `/` | **Right Arrow** |
| `` ` `` | **Esc** |
| `Opt` | **Left GUI (Super/Win/Cmd)** |
| `Backspace` | Backspace |

#### Fn Key Combos (Hold Fn to access default symbols)

| Key Press (Fn Active) | Action / Output |
| --- | --- |
| `Fn` + `;` | `;` (Semicolon) |
| `Fn` + `.` | `.` (Dot) |
| `Fn` + `,` | `,` (Comma) |
| `Fn` + `/` | `/` (Slash) |
| `Fn` + `` ` `` | `` ` `` (Backtick / Tilde) |
| `Fn` + `Backspace` | **Delete** |
| `Fn` + `Shift` | **Caps Lock Toggle** (Blue LED) |
| `Fn` + `Opt` | **Fn Lock Toggle** (Red LED) |

---

### 3. Mouse Mode Layout

You can control the mouse pointer using either the physical keys or the built-in IMU (Gyroscope).

#### Button Clicks
- `Enter` (OK) -> **Left Click**
- `\` (above Enter) -> **Right Click**
- `Space` -> **Middle Click / Scroll Click**

#### Key-based Movement (Default)
- `;` -> Move Up
- `.` -> Move Down
- `,` -> Move Left
- `/` -> Move Right
*Note: Holding down direction keys triggers a dynamic acceleration multiplier, enabling rapid pointer sweeping.*

#### IMU-based Motion Control
*Note: The following IMU features are dynamically enabled only if an IMU hardware module is detected on your Cardputer's StampS3 (via M5.Imu.isEnabled()). If no IMU is present, these keys/modes are ignored.*
- **Motion Mode** (`m`): Wave the Cardputer to pan the cursor around. Horizontal movement utilizes a combination of roll and yaw sensor fusion.
- **Scroll Mode** (`s`): Tilt the Cardputer forward or backward to trigger smooth vertical page scrolling. A built-in sensor deadzone prevents accidental scrolling from minor tremors.

---

## 🛠️ Build & Installation

### Option 1: PlatformIO (Recommended)
1. Install [PlatformIO](https://platformio.org/) on your computer.
2. Open this folder in VSCode / PlatformIO IDE.
3. Connect your M5Cardputer.
4. Run the upload command:
   ```bash
   pio run -t upload
   ```

### Option 2: Flash Binaries
You can download the precompiled `firmware.bin` from the Releases section and flash it directly to your M5Cardputer using [M5Burner](https://m5stack.com/pages/download) or `esptool.py`.

---

## 📄 License & Attribution
Original project created by [Geo (geo-tp)](https://github.com/geo-tp).
Stability improvements, NimBLE fixes, screen dimming, and motion upgrades by [Sinan (sinansevgi)](https://github.com/sinansevgi).
Licensed under the MIT License.