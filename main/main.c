#include <inttypes.h>
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_psram.h"
#include "nvs_flash.h"
#include "prisma_bsp.h"
#include "prisma_audio.h"
#include "prisma_imu.h"
#include "prisma_storage.h"
#include "prisma_ui.h"

static const char *TAG="PRISMA_B4";
static bool init_nvs(void){esp_err_t err=nvs_flash_init();if(err==ESP_ERR_NVS_NO_FREE_PAGES||err==ESP_ERR_NVS_NEW_VERSION_FOUND){if(nvs_flash_erase()!=ESP_OK)return false;err=nvs_flash_init();}return err==ESP_OK;}

void app_main(void)
{
    prisma_b0_status_t s={0};
    ESP_LOGI(TAG,"PRISMA-PI B4 Storage/TF starting");
    ESP_ERROR_CHECK(prisma_bsp_power_hold_init());ESP_ERROR_CHECK(prisma_bsp_buttons_init());
    s.nvs_ok=init_nvs();s.psram_ok=esp_psram_is_initialized();s.psram_mb=(uint32_t)(heap_caps_get_total_size(MALLOC_CAP_SPIRAM)/(1024U*1024U));
    uint32_t flash_bytes=0;if(esp_flash_get_size(NULL,&flash_bytes)==ESP_OK)s.flash_mb=flash_bytes/(1024U*1024U);esp_chip_info_t chip;esp_chip_info(&chip);ESP_LOGI(TAG,"cores=%d revision=%d flash=%" PRIu32 "MB psram=%" PRIu32 "MB",chip.cores,chip.revision,s.flash_mb,s.psram_mb);
    esp_err_t e=prisma_bsp_display_init();s.display_ok=e==ESP_OK;if(e!=ESP_OK){ESP_LOGE(TAG,"display init failed: %s",esp_err_to_name(e));return;}
    e=prisma_bsp_i2c_probe(&s);if(e!=ESP_OK)ESP_LOGW(TAG,"I2C probe: %s",esp_err_to_name(e));
    ESP_LOGI(TAG,"B3 regression display=%s touch=%s psram=%s nvs=%s imu=%s audio-codecs=%s",s.display_ok?"PASS":"FAIL",s.touch_ok?"PASS":"FAIL",s.psram_ok?"PASS":"FAIL",s.nvs_ok?"PASS":"FAIL",s.imu_seen?"PASS":"FAIL",(s.es8311_seen&&s.es7210_seen)?"PASS":"FAIL");
    if(s.es8311_seen&&s.es7210_seen){e=prisma_audio_init();if(e!=ESP_OK)ESP_LOGE(TAG,"audio init failed: %s",esp_err_to_name(e));}else ESP_LOGE(TAG,"B2 disabled: audio codecs not detected");
    if(s.imu_seen){e=prisma_imu_init();if(e==ESP_OK){prisma_imu_state_t imu;if(prisma_imu_read(&imu)==ESP_OK)ESP_LOGI(TAG,"B3 IMU ax=%.3f ay=%.3f az=%.3f gx=%.2f gy=%.2f gz=%.2f temp=%.2fC",imu.ax_g,imu.ay_g,imu.az_g,imu.gx_dps,imu.gy_dps,imu.gz_dps,imu.temperature_c);}else ESP_LOGE(TAG,"IMU init failed: %s",esp_err_to_name(e));}else ESP_LOGE(TAG,"B3 disabled: QMI8658 not detected");

    e=prisma_storage_init();
    if(e==ESP_OK){
        esp_err_t test=prisma_storage_self_test();
        prisma_storage_state_t st;prisma_storage_get_state(&st);
        ESP_LOGI(TAG,"B4 TF mounted=%s self-test=%s capacity=%lluMB free=%lluMB",st.mounted?"YES":"NO",test==ESP_OK?"PASS":"FAIL",(unsigned long long)(st.capacity_bytes/(1024ULL*1024ULL)),(unsigned long long)(st.free_bytes/(1024ULL*1024ULL)));
    }else ESP_LOGW(TAG,"B4 TF unavailable: %s",esp_err_to_name(e));

    ESP_ERROR_CHECK(prisma_ui_run());
}
