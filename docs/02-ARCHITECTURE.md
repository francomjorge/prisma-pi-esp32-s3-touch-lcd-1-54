# 02 — Architecture

## Architectural objective

Keep the firmware small enough to reason about while strictly isolating board-specific code.

```text
PRISMA APP
   |
   +-- LOCAL UI
   +-- WEB ADMIN
           |
      PRISMA SERVICES
           |
   +-------+-------+
   |       |       |
HARDWARE NETWORK  DATA
   |       |       |
   +-------+-------+
           |
          BSP
```

## Proposed code structure

```text
main/
  main.c                         composition root only

components/
  prisma_bsp/
    display/
    touch/
    audio/
    imu/
    storage/
    buttons/
    power/
    usb/

  prisma_system/
    boot/
    settings/
    diagnostics/
    identity/
    factory_reset/

  prisma_ui/
    intro/
    screens/
    widgets/
    navigation/

  prisma_audio/
    input/
    output/
    generator/
    levels/

  prisma_network/
    wifi/
    access_point/
    espnow/
    ble/
    discovery/

  prisma_data/
    sources/
    events/
    state/
    streams/

  prisma_web/
    server/
    api/
    realtime/
    admin/

storage/
  web/
```

This is a target organization, not permission to create code during the specification phase.

## BSP rule

Only `prisma_bsp` may contain:

- GPIO numbers;
- Waveshare-specific types;
- ST7789/CST816/QMI8658/ES7210/ES8311/NS4150B low-level details;
- direct ESP-IDF hardware-driver handles where practical.

Application/UI/web/protocol code consumes board-independent services.

## Sources, sinks and state

### Sources

- MIC1/MIC2/dual microphone stream.
- IMU accelerometer.
- IMU gyroscope.
- touch.
- buttons.
- generated audio (beep/tone/noise).
- system/network/storage state.

### Sinks

- display/backlight.
- speaker/audio output.
- storage.
- network/PRISMA transports when applicable.

### Capability descriptor

Each subsystem should eventually expose a consistent conceptual descriptor:

```text
CAPABILITY
  identity
  available
  enabled
  state
  metrics
  configuration
  commands
  events
  errors
```

This allows Local UI, Web Admin and future PRISMA CORE integration to observe the same authoritative state rather than maintaining independent copies.

## Audio architecture

```text
MIC1 --+
       +--> ES7210 --> PCM --> AUDIO INPUT SERVICE
MIC2 --+                    +--> level/peak
                            +--> local consumer
                            +--> recording
                            +--> future network stream

BEEP/TONE/NOISE/PCM --> AUDIO OUTPUT SERVICE --> ES8311 --> NS4150B --> SPEAKER
```

Initial audio scope:

- MIC on/off;
- MIC1/MIC2/dual selection where hardware path permits;
- input gain where usefully supported;
- live level and peak;
- PCM acquisition;
- speaker on/off/mute;
- speaker volume;
- PCM output;
- beep, tone and noise generation.

No initial synthesizer or complex DSP.

## Transport architecture

```text
                    PRISMA DATA
                         |
             +-----------+-----------+
             |           |           |
           Wi-Fi       ESP-NOW      BLE
```

Wi-Fi, ESP-NOW and BLE are not three independent radios. Wi-Fi/ESP-NOW use the 2.4 GHz Wi-Fi radio and BLE shares RF/coexistence resources. Performance tests must therefore cover simultaneous use.

ESP-NOW initial purpose: discovery, capabilities, peer management, commands, events and suitable sensor data. Do not assume continuous raw PCM is appropriate.

## Local-first rule

Hardware should remain useful without a router, browser or PRISMA CORE. Local appliance behavior is implemented/tested first; Web Admin and remote transports project the same state later.

## Factory reset

Factory reset is a normal system command, not a recovery subsystem. Initial intent:

- clear saved Wi-Fi credentials;
- restore AP configuration defaults;
- clear/change Admin credentials back to default state;
- restore configurable settings;
- reboot.

TF user content must not be erased by factory reset unless a separate explicit format-storage action is later specified.
