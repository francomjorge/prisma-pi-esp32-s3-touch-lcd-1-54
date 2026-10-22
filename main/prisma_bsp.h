#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "driver/i2c_master.h"

typedef struct {
    bool display_ok;
    bool touch_ok;
    bool imu_seen;
    bool es8311_seen;
    bool es7210_seen;
    bool psram_ok;
    bool nvs_ok;
    uint32_t flash_mb;
    uint32_t psram_mb;
} prisma_b0_status_t;

typedef struct {
    bool pressed;
    uint16_t x;
    uint16_t y;
} prisma_touch_sample_t;

typedef enum {
    PRISMA_BUTTON_BOOT = 0,
    PRISMA_BUTTON_PLUS,
    PRISMA_BUTTON_PWR,
} prisma_button_t;

esp_err_t prisma_bsp_power_hold_init(void);
esp_err_t prisma_bsp_power_off(void);
esp_err_t prisma_bsp_display_init(void);
esp_err_t prisma_bsp_i2c_probe(prisma_b0_status_t *status);
i2c_master_bus_handle_t prisma_bsp_i2c_bus(void);
esp_err_t prisma_bsp_buttons_init(void);
void prisma_bsp_draw_b0(const prisma_b0_status_t *status);

esp_err_t prisma_bsp_touch_read(prisma_touch_sample_t *sample);
bool prisma_bsp_button_pressed(prisma_button_t button);
void prisma_bsp_backlight_set(bool on);
void prisma_bsp_frame_clear(uint8_t r, uint8_t g, uint8_t b);
void prisma_bsp_frame_rect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b);
void prisma_bsp_frame_text(int x, int y, const char *value, int scale, uint8_t r, uint8_t g, uint8_t b);
esp_err_t prisma_bsp_frame_present(void);
