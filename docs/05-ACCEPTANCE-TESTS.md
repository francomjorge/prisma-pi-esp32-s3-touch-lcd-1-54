# 05 — Acceptance Tests

This file is the initial test contract. Add exact measurements/thresholds as implementation exposes real board behavior.

## Global rules

- A build must pass its own tests plus applicable previous-build regressions.
- No unexplained watchdog reset, panic or spontaneous reboot is acceptable.
- Test failures must produce enough diagnostic information to identify the failing subsystem.
- Web state must not diverge from appliance state once Web Admin exists.

## B0 — Hardware Foundation

- [ ] Cold boot ×10 without intervention.
- [ ] Soft reboot ×20.
- [ ] Correct flash size reported.
- [ ] Correct PSRAM availability/size reported.
- [ ] NVS read/write sanity test.
- [ ] Display solid colors/test pattern.
- [ ] Backlight stable.
- [ ] Touch four corners + center.
- [ ] Basic button electrical state validated.
- [ ] 30-minute runtime without reset.

## B1 — UI

- [ ] Intro repeats ×20 without hang/corruption.
- [ ] Clean intro → main transition.
- [ ] Main/second screen navigation.
- [ ] Tap.
- [ ] Long press.
- [ ] Swipes only accepted if reliable.
- [ ] Brightness/backlight controls.
- [ ] 30-minute navigation stress.
- [ ] No material progressive heap loss.

## B2 — Audio

- [ ] Speech changes live microphone level.
- [ ] Silence lowers level appropriately.
- [ ] MIC OFF stops/disables capture as specified.
- [ ] MIC1/MIC2/dual path verified if exposed.
- [ ] Gain changes acquisition behavior where supported.
- [ ] Peak meter works.
- [ ] Beep audible.
- [ ] Tone audible.
- [ ] Noise audible.
- [ ] Volume changes output.
- [ ] Mute/speaker OFF silences output.
- [ ] 30-minute continuous capture without reset/underrun failure.

## B3 — IMU

- [ ] AX/AY/AZ react coherently to orientation.
- [ ] GX/GY/GZ react coherently to rotation.
- [ ] Tilt event validation.
- [ ] Shake event validation if enabled.
- [ ] Still state is stable enough for UI/use.
- [ ] 30-minute IMU + UI + audio regression.

## B4 — TF / Storage

- [ ] Card detect/availability.
- [ ] Mount/unmount.
- [ ] Total/free capacity plausible.
- [ ] Create file.
- [ ] Write/read/verify content.
- [ ] Delete file.
- [ ] Directory create/list.
- [ ] List 100+ files without instability.
- [ ] Remove/reinsert handling according to specified safe behavior.
- [ ] Supported image from TF displayed.
- [ ] Supported audio/PCM path from TF played when implemented.
- [ ] No filesystem corruption during normal tested workflow.

## B5 — Wi-Fi STA

- [ ] Scan detects expected 2.4 GHz networks.
- [ ] Connect with valid credentials.
- [ ] Invalid credential behavior is clear/nonblocking.
- [ ] IP/RSSI/status displayed.
- [ ] Reboot reconnects saved network.
- [ ] AP/router loss detected.
- [ ] Network return triggers reconnect according to policy.
- [ ] Forget removes saved credentials.

## B6 — PRISMA AP

- [ ] AP visible from phone/computer.
- [ ] Generated SSID matches identity policy.
- [ ] Password configuration works.
- [ ] Administration reachable without Internet/router.
- [ ] AP+STA behavior tested if enabled.
- [ ] Fallback policy tested.
- [ ] Connected-client state plausible.

## B7 — Web Admin

- [ ] Desktop browser functional.
- [ ] Android browser functional.
- [ ] iPhone/iOS browser functional when available for test.
- [ ] AP mode functional.
- [ ] LAN/STA mode functional.
- [ ] Audio controls operate real hardware.
- [ ] Display controls operate real hardware.
- [ ] Sensor/storage/network state matches appliance.
- [ ] Local change becomes visible on Web.
- [ ] Web change becomes visible locally where applicable.
- [ ] Two simultaneous clients do not destabilize device.
- [ ] Browser refresh preserves authoritative device configuration/state.

## B8 — Real-Time

- [ ] Microphone meter responds in browser.
- [ ] IMU responds in browser.
- [ ] Touch/button events appear where specified.
- [ ] System/network metrics update.
- [ ] Client disconnect does not affect appliance.
- [ ] Client reconnect recovers current state.
- [ ] Sustained connection does not cause progressive memory loss.
- [ ] Update rates measured and documented.

## B9 — ESP-NOW / Discovery

- [ ] Two nodes discover each other.
- [ ] Identity/capabilities exchanged correctly.
- [ ] Peer disappearance detected according to timeout policy.
- [ ] Peer reappearance detected.
- [ ] Bidirectional command/event exchange.
- [ ] IMU/suitable sensor event transfer.
- [ ] Local peer page accurate.
- [ ] Web peer page accurate.
- [ ] Wi-Fi coexistence scenario tested.

## B10 — BLE

- [ ] Device advertises.
- [ ] Phone/peer discovers it.
- [ ] Connect/disconnect.
- [ ] PRISMA data/control operation.
- [ ] Reconnection behavior.
- [ ] BLE + Wi-Fi coexistence.
- [ ] BLE + Wi-Fi + ESP-NOW stress scenario documented/tested as feasible.

## B11 — Power/System/Diagnostics

- [ ] PWR electrical/semantic behavior verified.
- [ ] Long-press behavior works as specified.
- [ ] Controlled shutdown if hardware permits it.
- [ ] USB/battery state is truthful.
- [ ] Charging state/voltage only shown if actually available.
- [ ] Factory reset clears intended configuration.
- [ ] Factory reset does not erase TF user data.
- [ ] Uptime/heap/PSRAM/reset reason valid.
- [ ] All supported subsystem self-tests produce PASS/FAIL/error detail.

## B12 — Platform Release

- [ ] 100 cold boots.
- [ ] 100 soft reboots.
- [ ] 8-hour burn-in.
- [ ] 1-hour continuous microphone capture.
- [ ] 1-hour continuous IMU acquisition.
- [ ] repeated Wi-Fi loss/reconnect.
- [ ] repeated TF read/write.
- [ ] continuous local navigation.
- [ ] sustained Web real-time session.
- [ ] ESP-NOW active.
- [ ] BLE active.
- [ ] zero crashes.
- [ ] zero watchdog resets.
- [ ] heap minimum recorded.
- [ ] PSRAM minimum recorded.
- [ ] network/I-O/audio/storage/sensor errors recorded and reviewed.
