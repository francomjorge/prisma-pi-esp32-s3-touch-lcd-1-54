#pragma once

#include <stdbool.h>
#include "esp_err.h"

typedef struct {
    bool ready;
    float ax_g;
    float ay_g;
    float az_g;
    float gx_dps;
    float gy_dps;
    float gz_dps;
    float temperature_c;
    unsigned long sample_count;
} prisma_imu_state_t;

esp_err_t prisma_imu_init(void);
esp_err_t prisma_imu_read(prisma_imu_state_t *state);
void prisma_imu_get_state(prisma_imu_state_t *state);
