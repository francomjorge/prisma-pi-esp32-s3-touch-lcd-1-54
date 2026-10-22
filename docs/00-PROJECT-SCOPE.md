# 00 — Project Scope

## Target

- Product family: PRISMA-PI
- Board: Waveshare ESP32-S3-Touch-LCD-1.54
- SKU: 33869
- SoC baseline: ESP32-S3R8
- Display: 240×240

## Product concept

The device is a **standalone PRISMA node**. It must work locally without PRISMA CORE, while being architecturally prepared to advertise capabilities and exchange useful information with a future PRISMA CORE or other PRISMA nodes.

A physical input is not owned exclusively by the local application. Example: microphone data can feed local processing and, when a later transport specification permits it, be published remotely. The same applies to IMU, touch, buttons and system state.

## Scope included

1. Boot and PRISMA glitch intro.
2. Display/backlight and local 240×240 UI.
3. Capacitive touch and navigation.
4. Audio input: dual microphones, enable/disable, gain where supported, level/peak and PCM acquisition.
5. Audio output: speaker enable/disable, volume/mute, PCM, beep/tone/noise generation.
6. QMI8658 accelerometer/gyroscope and derived motion events.
7. TF/microSD storage and file management.
8. Flash, PSRAM and NVS/persistent settings.
9. Wi-Fi scan/STA/connect/reconnect/forget/status.
10. PRISMA access point and AP+STA/fallback behavior.
11. ESP-NOW discovery, peers, capabilities, commands/events/sensor data as appropriate.
12. BLE for discovery/control/data. Generic Bluetooth Classic/A2DP speaker support is not a target because ESP32-S3 is BLE-only.
13. USB-C/native USB capabilities to be validated before committing product functions.
14. BOOT, PLUS and PWR controls.
15. Power/battery behavior actually supported by this exact board.
16. External I2C/UART/USB expansion pads where physically available.
17. Unified data sources/events/state model.
18. Network API and real-time telemetry/streaming where technically appropriate.
19. Web Admin with one dedicated area per subsystem.
20. Security for admin, AP/Wi-Fi credentials and peer/pairing functions.
21. Diagnostics/self-test/logging.
22. PRISMA node identity/discovery/capability model.
23. Factory reset of configuration.

## Explicitly outside initial scope

- OTA firmware update.
- A/B application slots.
- Firmware rollback.
- Dedicated recovery firmware.
- Complex synthesizer/effects engine.
- Speech recognition/wake-word as an initial feature.
- Complex distributed audio processing.
- Continuous raw PCM over ESP-NOW unless later measurements demonstrate that it is an appropriate transport.

## Documentation confidence labels

Every hardware statement should use one of these states when relevant:

- **CONFIRMED** — supported by exact-board official documentation/schematic or physically verified.
- **TO VALIDATE** — plausible/present but behavior or routing still requires exact-board verification.
- **PLANNED** — PRISMA behavior to implement.
- **FUTURE** — architecture should not block it, but it is not in the initial implementation.

## Definition of done

A feature is not complete because it compiles. It is complete only when:

1. the physical appliance behavior works;
2. its state model is coherent;
3. Web Admin reflects/controls it once Web Admin exists;
4. relevant remote/event behavior works once its transport build exists;
5. build-specific acceptance tests pass;
6. applicable regression tests from all previous builds pass.
