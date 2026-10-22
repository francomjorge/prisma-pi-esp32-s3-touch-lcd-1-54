# 07 — Exact Pin / Bus Map — Waveshare ESP32-S3-Touch-LCD-1.54 SKU 33869

Status: **implementation baseline for B0**. Sources were cross-checked against the Waveshare schematic and previously working code for this exact S3 1.54 platform. Physical acceptance remains mandatory before declaring B0 stable.

## GPIO allocation

| GPIO | Signal | Subsystem | Notes |
|---:|---|---|---|
| 0 | KEY_MINUS / BOOT | button / boot | active low; ESP32 boot strap |
| 1 | BAT_ADC | power | battery-divider ADC node; divider ratio 3:1 |
| 2 | BAT_EN | power | firmware power-hold/latch; assert high at startup |
| 3 | CHG_STAT | power | charger status, active-low behavior to validate physically |
| 4 | KEY_PLUS | button | active low |
| 5 | KEY_PWR | power button | reads low while PWR is pressed; power circuit also has hardware behavior |
| 6 | IMU_INT | QMI8658 | interrupt line |
| 7 | PA_CTRL | NS4150B | amplifier control |
| 8 | I2S_MCLK | audio | ES8311 + ES7210 |
| 9 | I2S_SCLK/BCLK | audio | shared clock |
| 10 | I2S_LRCK/WS | audio | shared frame clock |
| 11 | I2S_ASDOUT | audio input | ES7210 → ESP32 microphone data |
| 12 | I2S_DSDIN | audio output | ESP32 → ES8311 playback data |
| 13 | SD_D2 | TF | SDMMC |
| 14 | SD_D3 | TF | SDMMC |
| 15 | SD_CMD | TF | SDMMC |
| 16 | SD_CLK | TF | SDMMC |
| 17 | SD_D0 | TF | SDMMC |
| 18 | SD_D1 | TF | SDMMC |
| 19 | USB_N | USB | native ESP32-S3 USB D-; do not use as generic GPIO |
| 20 | USB_P | USB | native ESP32-S3 USB D+; do not use as generic GPIO |
| 21 | LCD_CS | display | ST7789 chip select |
| 38 | LCD_CLK | display | SPI clock |
| 39 | LCD_DIN | display | SPI MOSI |
| 40 | LCD_RST | display | reset |
| 41 | SCL | shared I2C | touch + IMU + codecs + expansion |
| 42 | SDA | shared I2C | touch + IMU + codecs + expansion |
| 43 | ESP_TXD | expansion/debug | UART |
| 44 | ESP_RXD | expansion/debug | UART |
| 45 | LCD_DC | display | data/command |
| 46 | LCD_BL | display | backlight control |
| 47 | TP_RST | touch | CST816 reset |
| 48 | TP_INT | touch | CST816 interrupt |

GPIO33–37 belong to the integrated octal PSRAM interface and are not application GPIO.

## I2C bus

Shared bus: `SDA=GPIO42`, `SCL=GPIO41`, target 400 kHz.

| Device | 7-bit address | Role |
|---|---:|---|
| CST816 family | 0x15 | capacitive touch |
| ES8311 | 0x18 | speaker DAC/codec |
| ES7210 | 0x40 | dual-microphone ADC |
| QMI8658 | 0x6B | 6-axis IMU |

Important: some component documentation writes shifted 8-bit I2C values. PRISMA APIs and ESP-IDF use the 7-bit values above.

## Display

ST7789, 240×240, RGB565 baseline, 4-wire write-only SPI. `SCLK=38`, `MOSI=39`, `CS=21`, `DC=45`, `RST=40`, `BL=46`. B0 starts at 40 MHz SPI and validates orientation/color physically.

## Touch

CST816 family, address `0x15`, shared I2C, `RST=47`, `INT=48`. B0 only verifies presence. Coordinate/event/gesture behavior belongs to B1.

## Audio

I2C control uses the shared bus. I2S: `MCLK=8`, `BCLK=9`, `WS=10`, microphone data into ESP32 on `11`, speaker/playback data out on `12`; amplifier control `GPIO7`. B0 probes codec presence only. Full codec initialization is B2.

## TF / microSD

The exact S3 schematic routes the socket as 4-bit SDMMC: `CMD=15`, `CLK=16`, `D0=17`, `D1=18`, `D2=13`, `D3=14`. Storage initialization is intentionally deferred to B4 so B0 cannot accidentally couple storage failures to core bring-up.

## Power

ETA6098 charging path. `BAT_ADC=1`, `BAT_EN=2`, `CHG_STAT=3`, `KEY_PWR=5`. The battery ADC divider is 200K/100K, therefore the battery node is approximately ADC voltage ×3. This board does not contain a true fuel-gauge IC; voltage is not proof that a battery is physically installed when USB is present.

B0 asserts `BAT_EN` at startup. Actual shutdown, battery voltage calibration and charging semantics are B11 acceptance items.

## USB

Native ESP32-S3 USB is routed to Type-C on GPIO19/20 and is the programming/console path. It is reserved from application GPIO use.

## Resource rule

No application/service outside `prisma_bsp` may hard-code these GPIO numbers. Future code consumes semantic BSP interfaces only.
