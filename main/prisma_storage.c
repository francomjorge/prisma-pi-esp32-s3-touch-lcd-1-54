#include "prisma_storage.h"
#include "prisma_board.h"

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "ff.h"

static const char *TAG = "PRISMA_STORAGE";
static const char *MOUNT = "/sdcard";
static sdmmc_card_t *s_card;
static prisma_storage_state_t s_state;

static void refresh_space(void)
{
    FATFS *fs = NULL;
    DWORD free_clusters = 0;
    if (f_getfree("0:", &free_clusters, &fs) == FR_OK && fs != NULL) {
        uint64_t sector_size = s_state.sector_size ? s_state.sector_size : 512U;
        uint64_t cluster_bytes = (uint64_t)fs->csize * sector_size;
        s_state.capacity_bytes = (uint64_t)(fs->n_fatent - 2U) * cluster_bytes;
        s_state.free_bytes = (uint64_t)free_clusters * cluster_bytes;
    }
}

esp_err_t prisma_storage_init(void)
{
    if (s_state.mounted) return ESP_OK;

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_DEFAULT;

    sdmmc_slot_config_t slot = SDMMC_SLOT_CONFIG_DEFAULT();
    slot.width = 4;
    slot.clk = PRISMA_SD_CLK;
    slot.cmd = PRISMA_SD_CMD;
    slot.d0 = PRISMA_SD_D0;
    slot.d1 = PRISMA_SD_D1;
    slot.d2 = PRISMA_SD_D2;
    slot.d3 = PRISMA_SD_D3;
    slot.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    esp_vfs_fat_sdmmc_mount_config_t mount_cfg = {
        .format_if_mount_failed = false,
        .max_files = 8,
        .allocation_unit_size = 16 * 1024,
    };

    esp_err_t err = esp_vfs_fat_sdmmc_mount(MOUNT, &host, &slot, &mount_cfg, &s_card);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "TF mount failed: %s", esp_err_to_name(err));
        return err;
    }

    s_state.mounted = true;
    s_state.sector_size = s_card->csd.sector_size;
    refresh_space();
    ESP_LOGI(TAG, "TF mounted capacity=%lluMB free=%lluMB sector=%lu",
             (unsigned long long)(s_state.capacity_bytes / (1024ULL * 1024ULL)),
             (unsigned long long)(s_state.free_bytes / (1024ULL * 1024ULL)),
             (unsigned long)s_state.sector_size);
    return ESP_OK;
}

esp_err_t prisma_storage_self_test(void)
{
    if (!s_state.mounted) return ESP_ERR_INVALID_STATE;
    const char *path = "/sdcard/.prisma_b4_test";
    const char payload[] = "PRISMA-B4-STORAGE-OK\n";
    char readback[sizeof(payload)] = {0};

    FILE *f = fopen(path, "wb");
    if (!f) return ESP_FAIL;
    size_t n = fwrite(payload, 1, sizeof(payload) - 1, f);
    fclose(f);
    if (n != sizeof(payload) - 1) { remove(path); return ESP_FAIL; }

    f = fopen(path, "rb");
    if (!f) { remove(path); return ESP_FAIL; }
    n = fread(readback, 1, sizeof(payload) - 1, f);
    fclose(f);
    remove(path);
    if (n != sizeof(payload) - 1 || memcmp(readback, payload, sizeof(payload) - 1) != 0) return ESP_FAIL;

    refresh_space();
    ESP_LOGI(TAG, "TF read/write self-test PASS");
    return ESP_OK;
}

void prisma_storage_get_state(prisma_storage_state_t *state)
{
    if (state) *state = s_state;
}

const char *prisma_storage_mount_point(void)
{
    return MOUNT;
}
