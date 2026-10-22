#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"

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

esp_err_t prisma_bsp_power_hold_init(void);
esp_err_t prisma_bsp_display_init(void);
esp_err_t prisma_bsp_i2c_probe(prisma_b0_status_t *status);
esp_err_t prisma_bsp_buttons_init(void);
void prisma_bsp_draw_b0(const prisma_b0_status_t *status);
