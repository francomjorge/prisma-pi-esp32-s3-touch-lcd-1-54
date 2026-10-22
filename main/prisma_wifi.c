#include "prisma_wifi.h"

#include <string.h>
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs.h"

#define WIFI_NS "prisma_wifi"
#define WIFI_MAX_RETRY 10

static const char *TAG = "PRISMA_WIFI";
static prisma_wifi_state_t s_state;
static esp_netif_t *s_sta;
static bool s_handlers;

static void save_credentials(const char *ssid, const char *password)
{
    nvs_handle_t nvs;
    if (nvs_open(WIFI_NS, NVS_READWRITE, &nvs) != ESP_OK) return;
    (void)nvs_set_str(nvs, "ssid", ssid);
    (void)nvs_set_str(nvs, "pass", password);
    (void)nvs_commit(nvs);
    nvs_close(nvs);
}

static bool load_credentials(char *ssid, size_t ssid_len, char *password, size_t password_len)
{
    nvs_handle_t nvs;
    if (nvs_open(WIFI_NS, NVS_READONLY, &nvs) != ESP_OK) return false;
    size_t sl = ssid_len, pl = password_len;
    esp_err_t a = nvs_get_str(nvs, "ssid", ssid, &sl);
    esp_err_t b = nvs_get_str(nvs, "pass", password, &pl);
    nvs_close(nvs);
    return a == ESP_OK && b == ESP_OK && ssid[0] != 0;
}

static void event_handler(void *arg, esp_event_base_t base, int32_t id, void *data)
{
    (void)arg;
    if (base == WIFI_EVENT && id == WIFI_EVENT_STA_START) {
        if (s_state.ssid[0]) {
            s_state.status = PRISMA_WIFI_CONNECTING;
            (void)esp_wifi_connect();
        }
    } else if (base == WIFI_EVENT && id == WIFI_EVENT_STA_DISCONNECTED) {
        s_state.ip[0] = 0;
        s_state.rssi = 0;
        if (s_state.ssid[0] && s_state.retry_count < WIFI_MAX_RETRY) {
            s_state.retry_count++;
            s_state.status = PRISMA_WIFI_CONNECTING;
            (void)esp_wifi_connect();
        } else {
            s_state.status = s_state.ssid[0] ? PRISMA_WIFI_FAILED : PRISMA_WIFI_IDLE;
        }
    } else if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *ev = (ip_event_got_ip_t *)data;
        snprintf(s_state.ip, sizeof(s_state.ip), IPSTR, IP2STR(&ev->ip_info.ip));
        s_state.retry_count = 0;
        s_state.status = PRISMA_WIFI_CONNECTED;
        wifi_ap_record_t ap;
        if (esp_wifi_sta_get_ap_info(&ap) == ESP_OK) s_state.rssi = ap.rssi;
        ESP_LOGI(TAG, "connected ssid=%s ip=%s rssi=%d", s_state.ssid, s_state.ip, s_state.rssi);
    }
}

esp_err_t prisma_wifi_init(void)
{
    if (s_state.ready) return ESP_OK;
    esp_err_t err = esp_netif_init();
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) return err;
    err = esp_event_loop_create_default();
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) return err;
    s_sta = esp_netif_create_default_wifi_sta();
    if (!s_sta) return ESP_FAIL;
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    if ((err = esp_wifi_init(&cfg)) != ESP_OK) return err;
    if (!s_handlers) {
        ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, event_handler, NULL));
        ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, event_handler, NULL));
        s_handlers = true;
    }
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    s_state.ready = true;
    s_state.status = PRISMA_WIFI_IDLE;

    char ssid[33] = {0}, password[65] = {0};
    if (load_credentials(ssid, sizeof(ssid), password, sizeof(password))) {
        wifi_config_t wc = {0};
        strlcpy((char *)wc.sta.ssid, ssid, sizeof(wc.sta.ssid));
        strlcpy((char *)wc.sta.password, password, sizeof(wc.sta.password));
        wc.sta.threshold.authmode = WIFI_AUTH_OPEN;
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wc));
        strlcpy(s_state.ssid, ssid, sizeof(s_state.ssid));
    }
    return esp_wifi_start();
}

esp_err_t prisma_wifi_connect(const char *ssid, const char *password)
{
    if (!s_state.ready || !ssid || !ssid[0] || !password) return ESP_ERR_INVALID_ARG;
    wifi_config_t wc = {0};
    strlcpy((char *)wc.sta.ssid, ssid, sizeof(wc.sta.ssid));
    strlcpy((char *)wc.sta.password, password, sizeof(wc.sta.password));
    wc.sta.threshold.authmode = WIFI_AUTH_OPEN;
    esp_err_t err = esp_wifi_set_config(WIFI_IF_STA, &wc);
    if (err != ESP_OK) return err;
    strlcpy(s_state.ssid, ssid, sizeof(s_state.ssid));
    s_state.retry_count = 0;
    s_state.status = PRISMA_WIFI_CONNECTING;
    save_credentials(ssid, password);
    (void)esp_wifi_disconnect();
    return esp_wifi_connect();
}

esp_err_t prisma_wifi_disconnect(void)
{
    if (!s_state.ready) return ESP_ERR_INVALID_STATE;
    s_state.status = PRISMA_WIFI_IDLE;
    s_state.retry_count = WIFI_MAX_RETRY;
    return esp_wifi_disconnect();
}

esp_err_t prisma_wifi_forget(void)
{
    if (!s_state.ready) return ESP_ERR_INVALID_STATE;
    (void)esp_wifi_disconnect();
    nvs_handle_t nvs;
    if (nvs_open(WIFI_NS, NVS_READWRITE, &nvs) == ESP_OK) {
        (void)nvs_erase_all(nvs);
        (void)nvs_commit(nvs);
        nvs_close(nvs);
    }
    memset(s_state.ssid, 0, sizeof(s_state.ssid));
    memset(s_state.ip, 0, sizeof(s_state.ip));
    s_state.rssi = 0;
    s_state.retry_count = 0;
    s_state.status = PRISMA_WIFI_IDLE;
    return ESP_OK;
}

void prisma_wifi_get_state(prisma_wifi_state_t *state)
{
    if (state) *state = s_state;
}
