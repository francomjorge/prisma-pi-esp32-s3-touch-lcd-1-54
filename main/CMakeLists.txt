idf_component_register(
    SRCS "main.c" "prisma_bsp.c" "prisma_ui.c"
    INCLUDE_DIRS "."
    REQUIRES esp_lcd driver nvs_flash esp_psram spi_flash
)
