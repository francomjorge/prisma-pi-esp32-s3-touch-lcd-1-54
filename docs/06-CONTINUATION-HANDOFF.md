# 06 — Continuation / Handoff

Use this document to resume the project in a future conversation without reconstructing design decisions from memory.

## Repository

`francomjorge/prisma-pi-esp32-s3-touch-lcd-1-54`

This is the new clean repository for the redesign. The older repository `francomjorge/prisma-pi-lite-waveshare-esp32-s3-touch-lcd-1-54` may be inspected as a **reference source for previously working code/drivers/behavior**, but its architecture must not be copied blindly.

## Current phase

**Documentation/specification completed as initial baseline. Firmware implementation has not started.**

At this handoff:

- code intentionally absent;
- no firmware binary/build generated;
- no OTA/recovery work planned initially;
- implementation should start at Build 0 only after final exact-board pin/bus verification.

## Hardware target

Waveshare ESP32-S3-Touch-LCD-1.54, SKU 33869.

Known platform baseline:

- ESP32-S3R8, dual-core LX7 up to 240 MHz;
- 8 MB PSRAM;
- 16 MB flash;
- 240×240 ST7789 display;
- CST816-family capacitive touch;
- dual microphones / ES7210 audio input path;
- ES8311 output codec;
- NS4150B amplifier/speaker path;
- QMI8658 6-axis IMU;
- TF/microSD;
- Wi-Fi 2.4 GHz;
- BLE 5 (not Bluetooth Classic);
- ESP-NOW over Wi-Fi radio;
- USB-C/native S3 USB path;
- battery charge/discharge support;
- BOOT, PLUS and PWR controls;
- expansion pads documented for I2C/UART/USB.

## Decisions already made

1. Work in Spanish for project discussion; repository technical documentation may remain concise English unless deliberately changed.
2. Documentation before code/builds.
3. Board-specific details live only in `prisma_bsp`.
4. Local appliance must work independently of Web/PRISMA CORE.
5. Local UI, Web Admin and remote transports share authoritative service/state rather than duplicate state.
6. Main local screen includes microphone level; second screen includes quick hardware/audio controls.
7. Audio initial scope: microphone on/off, input gain if useful, level/peak, PCM; speaker on/off/mute/volume; beep/tone/noise/PCM output.
8. No initial synthesizer/complex DSP.
9. IMU is a first-class data source, not merely a demo sensor.
10. TF is a real storage/file/media subsystem.
11. Wi-Fi includes scan/connect/reconnect/forget plus PRISMA AP.
12. ESP-NOW is important for future PRISMA CORE/pedals and includes discovery/capabilities/peers/events/data.
13. Do not assume raw continuous PCM should travel over ESP-NOW.
14. BLE is for BLE-compatible discovery/control/data; generic Classic Bluetooth A2DP speakers are not a target.
15. Web Admin uses one dedicated section per subsystem rather than one mixed page.
16. OTA/A-B/rollback/recovery removed from initial scope to simplify the platform.
17. Factory reset remains, but is normal configuration reset and must not erase TF user data by default.
18. A build is done only after acceptance + regression tests, not when compilation succeeds.

## Build sequence

- B0 Hardware Foundation
- B1 Intro + Display + Touch + Local UI
- B2 Audio
- B3 IMU / Sensors
- B4 TF / Storage
- B5 Wi-Fi STA
- B6 PRISMA AP
- B7 Web Admin
- B8 Real-Time Data
- B9 ESP-NOW + PRISMA Discovery
- B10 BLE
- B11 Power + System + Diagnostics
- B12 Platform Release / integration burn-in

See `04-BUILD-PLAN.md` and `05-ACCEPTANCE-TESTS.md` for exact scope.

## Mandatory work before writing B0 drivers

Perform one final exact-board hardware verification using official Waveshare schematic/product/example material and, where required, the physical board. Freeze/document:

- TF/microSD bus and pins;
- QMI8658 bus/address/interrupt routing;
- ES7210/ES8311 I2C + I2S mapping;
- NS4150B enable polarity;
- BOOT/PLUS/PWR GPIO/electrical semantics;
- battery/power-hold/charge/telemetry behavior;
- USB routing/modes practical on this PCB;
- expansion pads and genuinely free GPIOs;
- shared buses/resources and conflicts.

Do not turn a generic ESP32-S3 capability into a board feature without verifying that SKU 33869 routes/supports it.

## Recommended first implementation action

After the verification above, create only the minimal ESP-IDF project skeleton and `prisma_bsp` needed for **B0 Hardware Foundation**. Do not pre-create all later services with placeholder code. Implement incrementally according to the build plan and tag only tested stable milestones.

## Previous working implementation reference

The older repository/history had working or partially working material including LCD/touch, glitch intro, SPIFFS/assets, AP/web server and advanced Wi-Fi work. Recover useful low-level knowledge from it only after comparing it with this new specification and exact-board hardware mapping.

## Resume instruction for a future chat

Read, in order:

1. `README.md`
2. `docs/00-PROJECT-SCOPE.md`
3. `docs/01-HARDWARE-INVENTORY.md`
4. `docs/02-ARCHITECTURE.md`
5. `docs/03-FUNCTIONAL-SPECIFICATION.md`
6. `docs/04-BUILD-PLAN.md`
7. `docs/05-ACCEPTANCE-TESTS.md`
8. this file

Then inspect the current repository state/commits before making changes. The next intended phase is **implementation beginning with B0**, after the remaining hardware-map verification is completed.
