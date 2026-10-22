#include <inttypes.h>
#include <string.h>
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_psram.h"
#include "nvs_flash.h"
#include "prisma_bsp.h"

static const char *TAG="PRISMA_B0";

static bool init_nvs(void)
{
    esp_err_t err=nvs_flash_init();
    if (err==ESP_ERR_NVS_NO_FREE_PAGES || err==ESP_ERR_NVS_NEW_VERSION_FOUND) {
        if (nvs_flash_erase()!=ESP_OK) return false;
        err=nvs_flash_init();
    }
    return err==ESP_OK;
}

void app_main(void)
{
    prisma_b0_status_t s={0};
    ESP_LOGI(TAG,"PRISMA-PI B0 Hardware Foundation starting");

    ESP_ERROR_CHECK(prisma_bsp_power_hold_init());
    ESP_ERROR_CHECK(prisma_bsp_buttons_init());

    s.nvs_ok=init_nvs();
    s.psram_ok=esp_psram_is_initialized();
    s.psram_mb=(uint32_t)(heap_caps_get_total_size(MALLOC_CAP_SPIRAM)/(1024U*1024U));

    uint32_t flash_bytes=0;
    if (esp_flash_get_size(NULL,&flash_bytes)==ESP_OK) s.flash_mb=flash_bytes/(1024U*1024U);

    esp_chip_info_t chip;
    esp_chip_info(&chip);
    ESP_LOGI(TAG,"cores=%d revision=%d flash=%" PRIu32 "MB psram=%" PRIu32 "MB",chip.cores,chip.revision,s.flash_mb,s.psram_mb);

    esp_err_t display_err=prisma_bsp_display_init();
    s.display_ok=(display_err==ESP_OK);
    if (!s.display_ok) ESP_LOGE(TAG,"display init failed: %s",esp_err_to_name(display_err));

    esp_err_t i2c_err=prisma_bsp_i2c_probe(&s);
    if (i2c_err!=ESP_OK) ESP_LOGE(TAG,"I2C probe failed: %s",esp_err_to_name(i2c_err));

    ESP_LOGI(TAG,"RESULT display=%s touch=%s psram=%s nvs=%s imu=%s audio=%s",
        s.display_ok?"PASS":"FAIL",s.touch_ok?"PASS":"FAIL",s.psram_ok?"PASS":"FAIL",s.nvs_ok?"PASS":"FAIL",
        s.imu_seen?"PASS":"FAIL",(s.es8311_seen&&s.es7210_seen)?"PASS":"FAIL");

    prisma_bsp_draw_b0(&s);
    ESP_LOGI(TAG,"B0 self-test screen active; no later-build services are started");
}
