#include "prisma_web.h"
#include "esp_http_server.h"
#include "esp_log.h"

static const char *TAG = "PRISMA_WEB";
static httpd_handle_t s_server;

static esp_err_t root_get(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/plain");
    return httpd_resp_sendstr(req, "PRISMA PI Web Admin B7");
}

esp_err_t prisma_web_start(void)
{
    if (s_server) return ESP_OK;
    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
    esp_err_t err = httpd_start(&s_server, &cfg);
    if (err != ESP_OK) return err;
    httpd_uri_t root = {.uri = "/", .method = HTTP_GET, .handler = root_get};
    err = httpd_register_uri_handler(s_server, &root);
    if (err != ESP_OK) return err;
    ESP_LOGI(TAG, "Web Admin started");
    return ESP_OK;
}

void prisma_web_stop(void)
{
    if (s_server) {
        httpd_stop(s_server);
        s_server = NULL;
    }
}
