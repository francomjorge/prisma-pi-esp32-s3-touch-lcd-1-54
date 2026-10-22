# 06 — Continuation / Handoff

Use this document to resume the project without reconstructing decisions from chat history.

## Repository and active work

Repository: `francomjorge/prisma-pi-esp32-s3-touch-lcd-1-54`

Active implementation branch: `b0-hardware-foundation`.

The older repository `francomjorge/prisma-pi-lite-waveshare-esp32-s3-touch-lcd-1-54` is reference material for previously working low-level behavior only; do not copy its architecture blindly.

## Current phase

**Implementation has started at B0 — Hardware Foundation.**

B0 currently contains:

- ESP-IDF project skeleton;
- target ESP32-S3, 16 MB flash, octal PSRAM baseline;
- exact board signal constants isolated in `prisma_board.h`;
- battery power-hold initialization;
- ST7789 240×240 bring-up;
- shared I2C bring-up and presence probes for CST816, QMI8658, ES8311 and ES7210;
- BOOT/PLUS/PWR input initialization;
- on-device B0 diagnostic screen;
- serial B0 diagnostics;
- GitHub Actions ESP-IDF 5.5.1 build workflow;
- exact pin/bus map in `docs/07-PIN-BUS-MAP.md`.

B0 is **not stable/released yet**. It must compile successfully in CI and then pass the physical acceptance tests on SKU 33869 before merge/tag.

No B1 or later functionality should be added until B0 is physically accepted.

## Hardware target

Waveshare ESP32-S3-Touch-LCD-1.54, SKU 33869: ESP32-S3R8, 8 MB octal PSRAM, 16 MB flash, ST7789 240×240, CST816 touch, ES7210 dual-mic ADC, ES8311 output codec, NS4150B amplifier, QMI8658 IMU, TF/microSD, Wi-Fi 2.4 GHz, BLE 5, native USB-C, battery charging/power circuit and BOOT/PLUS/PWR controls.

## Frozen board baseline

See `07-PIN-BUS-MAP.md`. Key mappings:

- LCD: SCLK38 MOSI39 CS21 DC45 RST40 BL46;
- shared I2C: SDA42 SCL41;
- touch: 0x15, RST47, INT48;
- QMI8658: 0x6B, INT6;
- ES8311: 0x18;
- ES7210: 0x40;
- I2S: MCLK8 BCLK9 WS10 MIC-DIN11 SPK-DOUT12;
- amplifier: GPIO7;
- TF SDMMC: CMD15 CLK16 D0=17 D1=18 D2=13 D3=14;
- BOOT0, PLUS4, PWR5;
- BAT_ADC1, BAT_EN2, CHG_STAT3;
- native USB: GPIO19/20.

## Product decisions

1. Board-specific details live only in the BSP.
2. Appliance works locally without Web/PRISMA CORE.
3. Local UI, Web Admin and remote transports share authoritative state/services.
4. Main local screen will include microphone level; second screen quick audio/hardware controls.
5. Initial audio: mic on/off, gain where useful, level/peak/PCM; speaker on/off/mute/volume; beep/tone/noise/PCM.
6. No initial synthesizer/complex DSP.
7. IMU is a first-class data source.
8. TF is a real storage/file/media subsystem.
9. Wi-Fi includes scan/connect/reconnect/forget and PRISMA AP.
10. ESP-NOW provides discovery/capabilities/peers/events/data; continuous raw PCM is not assumed to use ESP-NOW.
11. BLE is for BLE-compatible discovery/control/data, not Classic A2DP.
12. Web Admin has a dedicated section per subsystem.
13. OTA/A-B/rollback/recovery are outside initial scope.
14. Factory reset resets configuration and does not erase TF user data by default.
15. A build is complete only after acceptance and regression tests.

## Build sequence

B0 Hardware Foundation → B1 Intro/Display/Touch/UI → B2 Audio → B3 IMU/Sensors → B4 TF/Storage → B5 Wi-Fi STA → B6 PRISMA AP → B7 Web Admin → B8 Real-Time Data → B9 ESP-NOW/Discovery → B10 BLE → B11 Power/System/Diagnostics → B12 integrated platform release.

## Immediate next action

1. Let CI compile branch `b0-hardware-foundation`.
2. Fix compile issues only; do not expand scope.
3. Produce a flashable B0 binary.
4. Flash exact SKU 33869.
5. Run B0 physical acceptance: repeated boot, display orientation/colors, touch presence, PSRAM/flash/NVS, I2C device presence, button electrical behavior and stability.
6. Record results.
7. Only after all mandatory B0 tests pass, merge/tag B0 and start B1.

## Resume order for a future chat

Read `README.md`, docs 00 through 07, inspect the active branch and latest CI status, then continue from the first incomplete B0 acceptance item. Never assume a test passed merely because the firmware compiled.
