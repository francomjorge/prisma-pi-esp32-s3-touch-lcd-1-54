#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"

typedef enum {
    PRISMA_WIFI_OFF = 0,
    PRISMA_WIFI_IDLE,
    PRISMA_WIFI_CONNECTING,
    PRISMA_WIFI_CONNECTED,
    PRISMA_WIFI_FAILED
} prisma_wifi_status_t;

typedef struct {
    bool ready;
    prisma_wifi_status_t status;
    char ssid[33];
    char ip[16];
    int8_t rssi;
    uint32_t retry_count;
    bool ap_enabled;
    char ap_ssid[33];
    char ap_ip[16];
    uint8_t ap_clients;
} prisma_wifi_state_t;

esp_err_t prisma_wifi_init(void);
esp_err_t prisma_wifi_connect(const char *ssid, const char *password);
esp_err_t prisma_wifi_disconnect(void);
esp_err_t prisma_wifi_forget(void);
esp_err_t prisma_wifi_ap_start(const char *ssid, const char *password);
esp_err_t prisma_wifi_ap_stop(void);
void prisma_wifi_get_state(prisma_wifi_state_t *state);
