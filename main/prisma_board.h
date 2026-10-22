#pragma once

#include "driver/gpio.h"

/* Waveshare ESP32-S3-Touch-LCD-1.54 / SKU 33869.
 * Keep board-specific signals in this BSP boundary only.
 */

#define PRISMA_LCD_WIDTH          240
#define PRISMA_LCD_HEIGHT         240
#define PRISMA_LCD_SCLK           GPIO_NUM_38
#define PRISMA_LCD_MOSI           GPIO_NUM_39
#define PRISMA_LCD_CS             GPIO_NUM_21
#define PRISMA_LCD_DC             GPIO_NUM_45
#define PRISMA_LCD_RST            GPIO_NUM_40
#define PRISMA_LCD_BL             GPIO_NUM_46

#define PRISMA_I2C_SDA            GPIO_NUM_42
#define PRISMA_I2C_SCL            GPIO_NUM_41
#define PRISMA_TOUCH_ADDR         0x15
#define PRISMA_TOUCH_INT          GPIO_NUM_48
#define PRISMA_TOUCH_RST          GPIO_NUM_47
#define PRISMA_IMU_ADDR           0x6B
#define PRISMA_IMU_INT            GPIO_NUM_5
#define PRISMA_ES8311_ADDR        0x18
#define PRISMA_ES7210_ADDR        0x40

#define PRISMA_I2S_MCLK           GPIO_NUM_8
#define PRISMA_I2S_BCLK           GPIO_NUM_9
#define PRISMA_I2S_LRCK           GPIO_NUM_10
#define PRISMA_I2S_MIC_DIN        GPIO_NUM_11
#define PRISMA_I2S_SPK_DOUT       GPIO_NUM_12
#define PRISMA_PA_CTRL            GPIO_NUM_7

#define PRISMA_SD_CMD             GPIO_NUM_15
#define PRISMA_SD_CLK             GPIO_NUM_16
#define PRISMA_SD_D0              GPIO_NUM_17
#define PRISMA_SD_D1              GPIO_NUM_18
#define PRISMA_SD_D2              GPIO_NUM_13
#define PRISMA_SD_D3              GPIO_NUM_14

#define PRISMA_KEY_BOOT           GPIO_NUM_0
#define PRISMA_KEY_PLUS           GPIO_NUM_4
#define PRISMA_BAT_ADC            GPIO_NUM_1
#define PRISMA_BAT_EN             GPIO_NUM_2
#define PRISMA_CHG_STAT           GPIO_NUM_3

#define PRISMA_USB_D_MINUS        GPIO_NUM_19
#define PRISMA_USB_D_PLUS         GPIO_NUM_20
