idf_component_register(
    SRCS "main.c" "prisma_bsp.c" "prisma_ui.c" "prisma_audio.c" "prisma_imu.c" "prisma_storage.c" "prisma_wifi.c" "prisma_web.c"
    INCLUDE_DIRS "."
    REQUIRES esp_lcd driver nvs_flash esp_psram spi_flash esp_codec_dev fatfs sdmmc esp_wifi esp_event esp_netif esp_http_server
)
