#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"

typedef struct {
    bool mounted;
    uint64_t capacity_bytes;
    uint64_t free_bytes;
    uint32_t sector_size;
} prisma_storage_state_t;

esp_err_t prisma_storage_init(void);
esp_err_t prisma_storage_self_test(void);
void prisma_storage_get_state(prisma_storage_state_t *state);
const char *prisma_storage_mount_point(void);
