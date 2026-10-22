#include "prisma_web.h"
#include <stdio.h>
#include "esp_http_server.h"
#include "esp_log.h"
#include "prisma_wifi.h"
#include "prisma_storage.h"
#include "prisma_audio.h"
#include "prisma_imu.h"

static const char *TAG = "PRISMA_WEB";
static httpd_handle_t s_server;

static esp_err_t root_get(httpd_req_t *req)
{
    prisma_wifi_state_t w = {0};
    prisma_storage_state_t st = {0};
    prisma_audio_state_t a = {0};
    prisma_imu_state_t imu = {0};
    prisma_wifi_get_state(&w);
    prisma_storage_get_state(&st);
    prisma_audio_get_state(&a);
    prisma_imu_get_state(&imu);

    char page[3072];
    int n = snprintf(page, sizeof(page),
        "<!doctype html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>"
        "<title>PRISMA PI</title><style>body{background:#101317;color:#eef2f5;font-family:Arial;margin:0}"
        "main{max-width:720px;margin:auto;padding:22px}h1{letter-spacing:5px}.card{background:#171c22;border:1px solid #303943;border-radius:12px;padding:16px;margin:12px 0}"
        "b{color:#fff}.v{float:right;color:#9fe3c2}small{color:#8c98a4}</style></head><body><main>"
        "<h1>PRISMA PI</h1><small>B7 CHECKPOINT / WEB ADMIN</small>"
        "<div class='card'><b>NETWORK</b><p>AP<span class='v'>%s</span></p><p>AP IP<span class='v'>%s</span></p><p>Clients<span class='v'>%u</span></p><p>STA<span class='v'>%s</span></p><p>STA IP<span class='v'>%s</span></p><p>RSSI<span class='v'>%d dBm</span></p></div>"
        "<div class='card'><b>STORAGE</b><p>TF<span class='v'>%s</span></p><p>Capacity<span class='v'>%llu MB</span></p><p>Free<span class='v'>%llu MB</span></p></div>"
        "<div class='card'><b>AUDIO</b><p>Codec<span class='v'>%s</span></p><p>Mic<span class='v'>%s</span></p><p>Speaker<span class='v'>%s</span></p><p>Volume<span class='v'>%d</span></p></div>"
        "<div class='card'><b>IMU</b><p>QMI8658<span class='v'>%s</span></p><p>Samples<span class='v'>%lu</span></p></div>"
        "<div class='card'><b>SYSTEM</b><p>Firmware<span class='v'>0.7.0-b7-checkpoint</span></p><p>HTTP<span class='v'>ONLINE</span></p></div>"
        "</main></body></html>",
        w.ap_enabled ? w.ap_ssid : "OFF", w.ap_ip[0] ? w.ap_ip : "-", w.ap_clients,
        w.ssid[0] ? w.ssid : "not connected", w.ip[0] ? w.ip : "-", w.rssi,
        st.mounted ? "MOUNTED" : "UNAVAILABLE",
        (unsigned long long)(st.capacity_bytes/(1024ULL*1024ULL)),
        (unsigned long long)(st.free_bytes/(1024ULL*1024ULL)),
        a.ready ? "READY" : "OFF", a.mic_enabled ? "ON" : "OFF", a.speaker_enabled ? "ON" : "OFF", a.output_volume,
        imu.ready ? "READY" : "OFF", imu.sample_count);
    if (n < 0 || n >= (int)sizeof(page)) return ESP_FAIL;
    httpd_resp_set_type(req, "text/html; charset=utf-8");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    return httpd_resp_send(req, page, n);
}

static esp_err_t health_get(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/plain");
    return httpd_resp_sendstr(req, "PRISMA-B7-OK");
}

esp_err_t prisma_web_start(void)
{
    if (s_server) return ESP_OK;
    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
    cfg.max_uri_handlers = 4;
    esp_err_t err = httpd_start(&s_server, &cfg);
    if (err != ESP_OK) return err;
    httpd_uri_t root = {.uri = "/", .method = HTTP_GET, .handler = root_get};
    httpd_uri_t health = {.uri = "/health", .method = HTTP_GET, .handler = health_get};
    if ((err = httpd_register_uri_handler(s_server, &root)) != ESP_OK) return err;
    if ((err = httpd_register_uri_handler(s_server, &health)) != ESP_OK) return err;
    ESP_LOGI(TAG, "Web Admin started on port %u", cfg.server_port);
    return ESP_OK;
}

void prisma_web_stop(void)
{
    if (s_server) {
        httpd_stop(s_server);
        s_server = NULL;
    }
}
