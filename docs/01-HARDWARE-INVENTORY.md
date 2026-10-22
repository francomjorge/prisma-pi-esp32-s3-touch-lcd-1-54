# 01 — Hardware Inventory

Target: **Waveshare ESP32-S3-Touch-LCD-1.54, SKU 33869**.

This document separates confirmed board resources from items that must still be validated against the exact schematic/vendor examples/physical board before low-level implementation.

## Confirmed baseline

| Area | Resource | Baseline status |
|---|---|---|
| MCU | ESP32-S3R8, dual-core Xtensa LX7 up to 240 MHz | CONFIRMED |
| Internal memory | 512 KB SRAM, 384 KB ROM | CONFIRMED |
| PSRAM | 8 MB | CONFIRMED |
| Flash | 16 MB external NOR | CONFIRMED |
| Display | 1.54-inch IPS, 240×240 | CONFIRMED |
| LCD controller | ST7789 family, SPI | CONFIRMED |
| Touch | CST816 family capacitive touch, I2C | CONFIRMED |
| IMU | QMI8658 6-axis accelerometer + gyroscope | CONFIRMED |
| Audio input | Dual onboard microphones through ES7210 path | CONFIRMED |
| Audio output | ES8311 codec path | CONFIRMED |
| Amplifier | NS4150B | CONFIRMED |
| Speaker | onboard/external speaker path provided by board | CONFIRMED |
| Storage | onboard TF/microSD slot | CONFIRMED |
| Radio | 2.4 GHz Wi-Fi 802.11 b/g/n | CONFIRMED |
| Bluetooth | BLE 5 capability; no Bluetooth Classic | CONFIRMED |
| ESP-NOW | available through ESP32-S3 Wi-Fi radio; not a separate radio | CONFIRMED capability |
| USB | USB Type-C / ESP32-S3 native USB path | CONFIRMED hardware, product modes TO VALIDATE |
| Power | USB and 3.7 V battery charge/discharge support | CONFIRMED baseline |
| Controls | BOOT, PLUS, PWR physical controls | CONFIRMED |
| Expansion | external I2C/UART/USB pads documented for board | CONFIRMED baseline |

## Reference display/touch mapping

The previous exact-board investigation established the following vendor-reference configuration. It must still be cross-checked before BSP constants are frozen:

- LCD: 240×240, RGB565/16 bpp initial format, ST7789, SPI.
- LCD reference SPI host: SPI3_HOST.
- LCD reference pixel clock: 40 MHz.
- LCD SCLK GPIO38.
- LCD MOSI GPIO39.
- LCD RESET GPIO40.
- LCD DC GPIO45.
- LCD CS GPIO21.
- LCD BACKLIGHT GPIO46.
- Touch: CST816S, I2C.
- Touch reference I2C clock: 400 kHz.
- Touch SCL GPIO41.
- Touch SDA GPIO42.
- Touch INT GPIO48.
- Touch RESET GPIO47.

Status: **reference values; freeze only after exact SKU cross-check and physical smoke test.**

## Audio reference mapping

Working reference from prior documentation research:

- ES7210: microphone ADC/front end.
- ES8311: audio output codec.
- NS4150B: power amplifier.
- Reference I2S signals observed in board-specific material: MCLK GPIO8, BCLK GPIO9, WS GPIO10, input data GPIO11, output data GPIO12.
- Reference amplifier enable: GPIO7.
- Common 7-bit I2C addresses observed: ES8311 `0x18`, ES7210 `0x40`, QMI8658 `0x6B`.

Important: some documentation expresses I2C addresses in shifted 8-bit form. Drivers must use the address convention required by the selected ESP-IDF API; do not mix 7-bit and shifted addresses.

Status: **TO VALIDATE against exact-board schematic/vendor example before implementation.**

## Resources still requiring pin/bus freeze

Before Build 0 implementation, complete exact-board verification for:

1. TF/microSD bus mode and all pins.
2. QMI8658 exact shared bus/pins/address/interrupt routing.
3. ES7210/ES8311 control bus and complete I2S routing.
4. NS4150B enable polarity and behavior.
5. BOOT/PLUS/PWR GPIOs and electrical semantics.
6. Battery/power-hold/charge/telemetry circuitry.
7. Whether battery voltage, charging state or percentage can be read reliably.
8. USB Type-C routing and which native USB modes are practical on this PCB.
9. Expansion pads and genuinely free GPIOs after all onboard resources are accounted for.
10. Shared-bus constraints and DMA/resource conflicts.

## Native capability vs board capability

Do not treat every ESP32-S3 peripheral as an exposed board feature. Documentation must distinguish:

- capability inside the ESP32-S3;
- signal physically routed on SKU 33869;
- signal available to the user through a pad/connector;
- feature physically tested by PRISMA.

This distinction is mandatory before adding a feature to the product specification.
