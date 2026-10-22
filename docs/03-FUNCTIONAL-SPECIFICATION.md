# 03 — Functional Specification

This is the initial complete functional coverage. Every area must later be implemented as a real behavior, not merely represented by a menu title.

## 1. Boot / Intro

Required sequence:

`Power → minimal initialization → display/backlight → PRISMA glitch intro → hardware/service initialization → main screen`

Requirements:

- recognizable PRISMA glitch identity;
- intro must not unnecessarily block hardware initialization;
- secondary peripheral failure should allow degraded boot when safe;
- failures become diagnostics, not unexplained hangs;
- transition cleanly to operational UI.

## 2. Display / Backlight

- ST7789 240×240 local surface.
- Initial production pixel format expected to be RGB565 unless later measurement justifies otherwise.
- deterministic orientation.
- brightness control.
- backlight on/off independent from whole-device power state.
- images/assets through a normal media path, not ad-hoc driver calls from application code.
- display test patterns and error state.

## 3. Touch / Navigation

Raw state: X, Y, press/release.

Normalized UI events planned:

- tap;
- double tap;
- long press;
- swipe left/right/up/down where reliable.

Include corner/center diagnostic and touch-state visualization. UI must also retain useful physical-button control paths so a touch problem does not make diagnostics unreachable.

## 4. Local UI

### Main screen

Operational information, including microphone level and the most useful node/network/application state. Microphone level is a measurement; microphone gain is a separate control.

### Second screen / hardware quick controls

At minimum:

- microphone ON/OFF;
- microphone gain when supported;
- speaker ON/OFF/mute;
- speaker volume;
- BEEP test;
- NOISE test;
- brightness/control access.

Additional dedicated local pages may cover Network, Sensors, Storage and Diagnostics without turning the 240×240 display into a miniature web page.

## 5. Audio Input

Hardware path: dual microphones → ES7210 → PCM.

Functions:

- microphone service ON/OFF;
- MIC1/MIC2/dual selection if supported by validated codec configuration;
- input gain;
- live level;
- peak;
- PCM capture;
- health/error state;
- later recording and remote stream consumers without redesigning the capture service.

Future-capable but not initial requirements: VAD, wake word, noise reduction, echo cancellation.

## 6. Audio Output

Hardware path: audio service → ES8311 → NS4150B → speaker.

Functions:

- output ON/OFF;
- mute;
- volume;
- PCM playback;
- beep generator;
- tone generator;
- noise generator;
- amplifier/output health.

Initial implementation is a hardware/audio validation engine, not a synthesizer.

## 7. IMU / Sensors

QMI8658 raw sources:

- accelerometer AX/AY/AZ;
- gyroscope GX/GY/GZ.

Derived events planned:

- tilt/orientation;
- rotate;
- shake;
- movement/still;
- impact if reliable thresholds can be established.

Raw and derived state must be usable by Local UI, Web Admin and future PRISMA routing.

## 8. TF / microSD Storage

Functions:

- availability/card state;
- mount/unmount;
- filesystem state;
- total/used/free;
- read/write/delete;
- directories;
- file enumeration;
- filename, extension, size and useful metadata;
- preview for supported image/media types where practical;
- benchmark/health/error reporting.

Initial logical directories:

```text
/audio
/images
/presets
/recordings
/logs
/system
```

Expected uses: audio, images/assets, presets, recordings, logs and future media.

## 9. Flash / PSRAM / NVS

- detect/report flash and PSRAM;
- expose heap/PSRAM health metrics;
- NVS persistent settings;
- Wi-Fi/AP/Admin/configuration persistence;
- avoid duplicated large buffers;
- reserve internal RAM for latency/DMA/critical structures and use PSRAM for appropriate larger buffers/caches.

OTA/A-B layout is not an initial requirement.

## 10. Wi-Fi STA

Functions:

- radio status;
- scan;
- SSID list and RSSI;
- connect with credentials;
- disconnect;
- reconnect;
- forget saved network;
- persistent credentials;
- IP, gateway, DNS, RSSI, hostname/status;
- loss detection and reconnection behavior.

Only 2.4 GHz 802.11 b/g/n is expected from ESP32-S3.

## 11. PRISMA Access Point

- generated PRISMA SSID derived from node identity;
- configurable AP password;
- AP state and connected clients;
- fixed/local administration address as defined during implementation;
- AP+STA where stable;
- fallback AP when normal network access is unavailable, according to final policy;
- access to Web Admin without Internet/router.

## 12. ESP-NOW

Treat as an independent PRISMA transport/service even though it uses the Wi-Fi radio.

Functions:

- enable/disable;
- discovery broadcast;
- peers;
- pair/unpair;
- unicast/broadcast;
- node identity;
- capability exchange;
- commands/events;
- suitable sensor/state transport;
- RSSI/transport diagnostics where available;
- encryption/security policy.

Do not make continuous raw PCM a requirement without throughput/latency/loss testing.

## 13. PRISMA Discovery

Every node should eventually advertise at least:

- node ID;
- node type;
- friendly name;
- platform/firmware version;
- capabilities;
- availability/status relevant to discovery.

Example capabilities: display, touch, mic1/mic2, audio output, generated audio, accelerometer, gyroscope, TF storage, Wi-Fi, ESP-NOW and BLE.

## 14. BLE

Initial purpose:

- advertising/discovery;
- configuration/control;
- data/services useful to PRISMA;
- connection state/RSSI/diagnostics;
- coexistence testing with Wi-Fi.

Bluetooth Classic/A2DP speaker support is explicitly not assumed.

## 15. USB-C / Native USB

Initial guaranteed role: power/programming/debug path according to board support.

Potential future functions to validate before product commitment:

- USB Serial;
- USB MIDI;
- USB HID;
- other USB device roles.

Do not claim USB host/device modes until exact PCB routing and selected ESP-IDF mode are validated.

## 16. Buttons

Physical controls: BOOT, PLUS, PWR.

Normalize when electrically/reliably possible:

- press/release;
- click;
- double click;
- long press.

PWR receives dedicated system semantics. Final actions are assigned after exact electrical behavior is verified.

## 17. Power / Battery

- USB-powered operation;
- battery-powered operation where fitted;
- continuous operation by default;
- no automatic deep/light sleep unless later explicitly specified;
- backlight policy independent from system power;
- controlled shutdown behavior from PWR long press if hardware supports it;
- charging/battery telemetry only to the extent the exact board exposes reliable data.

Do not invent battery percentage if only voltage or charge-state information is available.

## 18. Expansion

Document and preserve validated external I2C/UART/USB pads and truly free GPIOs. Future sensors should integrate through normal services/data-source contracts rather than bypassing architecture.

## 19. Data Sources / Events

Unified conceptual sources:

- microphone;
- IMU;
- touch;
- buttons;
- generated audio;
- system;
- network/storage state.

A source may have local consumers and remote consumers. One authoritative state/event path should feed UI, Web and PRISMA transports.

## 20. API / Real-Time Data

Once networking/web exists, provide appropriate state/control APIs and real-time channels. Candidate live data:

- microphone level/peak;
- IMU;
- touch events;
- button events;
- system metrics;
- network state.

Protocol and sample rates must be chosen from measurements. WebSocket/SSE/REST are implementation choices, not hardware requirements.

Audio PCM streaming is a distinct requirement from telemetry and must be evaluated for throughput, buffering, latency and loss before choosing transport.

## 21. Web Admin

Dedicated sections, not a single mixed page:

- Overview
- Display
- Touch
- Audio
- Sensors
- Storage
- Files
- Wi-Fi
- ESP-NOW
- Bluetooth
- Power
- Buttons
- USB
- Memory
- System
- Diagnostics
- Security

Standard page pattern where applicable:

`STATUS → LIVE DATA → CONFIGURATION → TEST → ERRORS`

Web controls must operate the same authoritative services/state as local controls.

## 22. Security

Initial specification covers:

- Admin credentials; initial no-password policy may be retained only if deliberately confirmed;
- ability to set/change Admin password;
- Wi-Fi credential persistence;
- configurable PRISMA AP password;
- ESP-NOW pairing/encryption policy;
- BLE access/pairing policy where required;
- factory reset of configuration.

Security behavior must not be hidden inside UI code.

## 23. Diagnostics / Self-Test

At minimum report/test:

- display/backlight;
- touch;
- audio input;
- audio output;
- IMU;
- TF;
- Wi-Fi;
- ESP-NOW;
- BLE;
- PSRAM;
- Flash/NVS;
- power information available;
- uptime;
- heap/PSRAM minima;
- reset reason/watchdog errors;
- subsystem error state/logs.

Local diagnostics and Web Admin should project the same results.
