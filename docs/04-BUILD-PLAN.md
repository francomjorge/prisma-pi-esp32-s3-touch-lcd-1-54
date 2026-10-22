# 04 — Functional Build Plan

Builds are functional milestones. Do not create a new milestone merely because a driver compiles. Every build must produce an appliance behavior that can be demonstrated and tested.

## B0 — Hardware Foundation

### Appliance
Initialize and validate CPU/platform, flash, PSRAM, NVS, display/backlight, touch, basic buttons/watchdog/identity. Show a simple hardware-test screen.

### Web
None.

### Exit condition
Stable repeated boot, working display/touch, correct memory detection and no unexplained resets.

---

## B1 — Intro + Display + Touch + Local UI

### Appliance
- PRISMA glitch intro.
- clean transition to Main Screen.
- brightness/backlight control.
- touch and normalized navigation events.
- Main Screen and second hardware/control screen.
- basic physical-button navigation/recognition.

### Web
None.

### Exit condition
PRISMA is a navigable standalone appliance, not merely a hardware test.

---

## B2 — Audio

### Appliance
- ES7210 input path.
- MIC1/MIC2/dual as validated.
- microphone ON/OFF.
- gain.
- level/peak visualization.
- PCM capture.
- ES8311 + NS4150B output path.
- speaker ON/OFF/mute.
- volume.
- beep/tone/noise.

Main Screen displays live microphone information; second screen exposes quick audio controls.

### Web
No full Admin yet. Internal service/state contracts must already be suitable for later Web projection.

### Exit condition
Speaking changes level; audio output tests are audible and controllable; sustained capture is stable.

---

## B3 — IMU / Sensors

### Appliance
- QMI8658 accelerometer and gyroscope live data.
- sensor page.
- derived events such as tilt/rotate/shake where reliable.

### Web
Not required yet, but sensor state/events use authoritative data model.

### Exit condition
Physical movement produces coherent raw data/events while UI/audio remain stable.

---

## B4 — Storage / TF

### Appliance
- TF detect/mount/unmount.
- capacity/free space.
- directory/file browser.
- metadata.
- read/write/delete.
- supported preview/playback path where practical.
- initial logical directories.

### Web
No full Admin yet; file/storage services must be reusable later.

### Exit condition
Reliable card insertion/use, file operations and media path tests without corrupting UI/runtime.

---

## B5 — Wi-Fi STA

### Appliance
- scan/list networks.
- connect/disconnect.
- saved credentials.
- reconnect.
- forget.
- RSSI/IP/network status.

### Web
Only minimal/internal networking required; full Admin comes later.

### Exit condition
Connect, reboot/reconnect, network loss/recovery and forget-network tests pass.

---

## B6 — PRISMA Access Point

### Appliance
- generated PRISMA SSID.
- AP ON/OFF/status.
- configurable password.
- AP+STA/fallback policy.
- connected-client information where available.

### Web
Expose the first usable administration entry point through the PRISMA AP, preparing B7.

### Exit condition
A factory-default device can be reached from a phone/computer without an external router/Internet connection.

---

## B7 — Web Admin

### Appliance
Existing local behavior remains authoritative.

### Web
Implement dedicated areas:

Overview, Display, Touch, Audio, Sensors, Storage, Files, Wi-Fi, ESP-NOW placeholder/status, Bluetooth placeholder/status, Power, Buttons, USB, Memory, System, Diagnostics, Security.

Each applicable page follows:

`STATUS → LIVE DATA → CONFIGURATION → TEST → ERRORS`

### Exit condition
Web controls modify the same services used by the appliance; local changes are reflected by Web state; multiple clients do not destabilize firmware.

---

## B8 — Real-Time Data

### Appliance
No loss of local autonomy.

### Web
Real-time telemetry for suitable sources: microphone level/peak, IMU, touch/buttons, system/network state. Select WebSocket/SSE rates from measured performance.

### Exit condition
Live UI is responsive, disconnect/reconnect is safe, and sustained real-time clients do not leak memory/reset the device.

---

## B9 — ESP-NOW + PRISMA Discovery

### Appliance
- ESP-NOW enable/status.
- discovery.
- peers.
- node identity/type/name/version/capabilities.
- pair/unpair as specified.
- bidirectional commands/events and suitable sensor data.
- local ESP-NOW page.

### Web
Complete ESP-NOW/Discovery tab with peers, capabilities, state and tests.

### Exit condition
Two PRISMA-capable ESP devices discover each other, detect disappearance/reappearance and exchange bidirectional data reliably.

Raw continuous PCM over ESP-NOW is not a B9 requirement.

---

## B10 — BLE

### Appliance
- BLE enable/status.
- advertising/discovery.
- PRISMA control/data services.
- connection state/diagnostics.

### Web
Bluetooth page becomes functional.

### Exit condition
Phone/peer detects and connects, data/control path works and coexistence with Wi-Fi is stable.

---

## B11 — Power + System + Final Diagnostics

### Appliance
- validated PWR semantics.
- controlled shutdown if board allows.
- USB/battery/charging information actually available.
- factory reset.
- complete self-test.
- uptime, heap, PSRAM, reset reason, subsystem health/logs.

### Web
Power/System/Diagnostics/Security pages complete and show the same authoritative state.

### Exit condition
Full self-test and system-management paths pass; no unsupported battery metric is fabricated.

---

## B12 — Platform Release

Adds no new hardware feature. It is the integration/stability milestone.

Run display + touch + audio + IMU + TF + Wi-Fi/AP + Web + real-time + ESP-NOW + BLE + power/diagnostics together.

### Release requirements

- 100 cold boots.
- 100 soft reboots.
- 8-hour burn-in.
- 1-hour continuous microphone capture.
- 1-hour continuous IMU acquisition.
- repeated Wi-Fi loss/reconnect.
- repeated TF read/write.
- continuous local navigation.
- sustained Web real-time connection.
- ESP-NOW active.
- BLE active/coexistence test.
- zero crashes and zero watchdog resets.
- record heap/PSRAM minima, I/O/network/audio/storage/sensor errors.

## Versioning principle

Tag stable milestones using an unambiguous convention such as `build-02-audio`. A tag is created only after the milestone acceptance/regression matrix passes.
