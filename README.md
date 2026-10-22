# PRISMA-PI — ESP32-S3 Touch LCD 1.54

Target hardware: **Waveshare ESP32-S3-Touch-LCD-1.54, SKU 33869**.

This repository starts from a documentation-first hardware/platform specification. **No firmware implementation or physical build should begin until the baseline documents are reviewed.**

## Project objective

Build a standalone PRISMA node/appliance that can operate locally and later participate in a larger PRISMA system. Physical inputs are modeled as sources, outputs as sinks, and useful state/events may be exposed locally, through Web Admin, or to other PRISMA nodes.

Primary onboard areas: display/backlight, capacitive touch, dual-microphone audio input, audio output/speaker, QMI8658 IMU, TF/microSD, Wi-Fi, ESP-NOW, BLE, buttons, power/battery, USB and external expansion.

## Documentation status

The current phase is **SPECIFICATION ONLY**:

- Code: 0%
- Firmware builds: 0%
- OTA/rollback/recovery: outside initial scope
- Factory reset: retained as a normal system function

Start here:

1. [`docs/00-PROJECT-SCOPE.md`](docs/00-PROJECT-SCOPE.md)
2. [`docs/01-HARDWARE-INVENTORY.md`](docs/01-HARDWARE-INVENTORY.md)
3. [`docs/02-ARCHITECTURE.md`](docs/02-ARCHITECTURE.md)
4. [`docs/03-FUNCTIONAL-SPECIFICATION.md`](docs/03-FUNCTIONAL-SPECIFICATION.md)
5. [`docs/04-BUILD-PLAN.md`](docs/04-BUILD-PLAN.md)
6. [`docs/05-ACCEPTANCE-TESTS.md`](docs/05-ACCEPTANCE-TESTS.md)
7. [`docs/06-CONTINUATION-HANDOFF.md`](docs/06-CONTINUATION-HANDOFF.md)

## Core rule

Board-specific GPIOs, Waveshare/controller-specific types and low-level drivers belong only to the BSP. Application, UI, web and PRISMA protocol layers must consume hardware-independent services/contracts.

## Initial platform release path

`B0 Hardware → B1 UI → B2 Audio → B3 Sensors → B4 Storage → B5 Wi-Fi → B6 AP → B7 Web Admin → B8 Real-Time Data → B9 ESP-NOW/Discovery → B10 BLE → B11 Power/Diagnostics → B12 Platform Release`

A build is complete only after its acceptance tests **and all applicable regression tests from previous builds** pass.
