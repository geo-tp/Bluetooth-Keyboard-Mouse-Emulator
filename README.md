# M5 Keyboard and Mouse Emulator

USB/BLE HID keyboard and mouse for **M5 Cardputer** and **Cardputer ADV**.

## Features

- USB or Bluetooth HID mode
- Mouse and keyboard mode (toggle with **G0**)
- Adjustable mouse speed (**Fn + `-`** slower, **Fn + `=`** faster)
- Cursor axis rotation: 0° / 90° / 180° / 270° (**Fn + `9`** / **Fn + `0`**)
- Settings saved in flash (USB/BT, mode, speed, rotation)
- Full keyboard mapping including Fn layer (F-keys, arrows, special keys)
- Cardputer ADV: arrow keys on keycaps (`;` `,` `.` `/`) or via Fn layer
- Display sleep after 45 s idle; battery level in header
- Reboot: **Ctrl + Fn + Esc**

## Controls

| Action | Input |
|--------|-------|
| USB / Bluetooth at boot | `;` or `.` to switch, **Enter** to save |
| Mouse / Keyboard | **G0** (side button) |
| Mouse move | `,` `.` `;` `/` or **Fn +** same keys (arrows) |
| Mouse click | **Enter** left, `\` right |
| Mouse speed | **Fn + `-`** / **Fn + `=`** |
| Cursor rotation | **Fn + `9`** / **Fn + `0`** (shown as `Rot:0` … `Rot:270`) |
| Reboot | **Ctrl + Fn + Esc** |

### Fn layer (no physical F-keys on Cardputer)

There are no separate **F1–F12** keys. They are on the Fn layer:

| Fn + key | Result |
|----------|--------|
| `1` … `0` | F1 … F10 |
| `-` | F11 (speed down in mouse mode) |
| `=` | F12 (speed up in mouse mode) |
| `9` | F9 (rotation −90° in mouse mode) |
| `0` | F10 (rotation +90° in mouse mode) |
| `;` `,` `.` `/` | Arrow keys (on classic Cardputer; ADV has dedicated arrows) |

**Fn** is the key left of **Aa** (3rd row, 1st column). On ADV it may be unlabeled.

### Rotation 90°

Useful when holding Cardputer sideways: right → up, left → down, up → left, down → right.

## Build & flash

```bash
pio run -t upload
```

Or flash `MouseKeyboard-v1.2.bin` from [Releases](https://github.com/immensaccessum/Bluetooth-Keyboard-Mouse-Emulator/releases).

## Version

**1.2** — ADV keyboard, NVS settings, mouse speed & rotation, BLE reconnect fix, compact UI for 135px display.
