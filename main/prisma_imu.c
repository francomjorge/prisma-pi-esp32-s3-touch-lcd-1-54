#include "prisma_imu.h"
#include "prisma_bsp.h"
#include "prisma_board.h"

#include <string.h>
#include "driver/i2c_master.h"
#include "esp_log.h"

#define QMI8658_WHO_AM_I       0x00
#define QMI8658_REVISION       0x01
#define QMI8658_CTRL1          0x02
#define QMI8658_CTRL2          0x03
#define QMI8658_CTRL3          0x04
#define QMI8658_CTRL7          0x08
#define QMI8658_TEMP_L         0x33
#define QMI8658_AX_L           0x35

static const char *TAG = "PRISMA_IMU";
static i2c_master_dev_handle_t s_dev;
static prisma_imu_state_t s_state;

static esp_err_t reg_write(uint8_t reg, uint8_t value)
{
    uint8_t b[2] = {reg, value};
    return i2c_master_transmit(s_dev, b, sizeof(b), 50);
}

static esp_err_t reg_read(uint8_t reg, uint8_t *data, size_t len)
{
    return i2c_master_transmit_receive(s_dev, &reg, 1, data, len, 50);
}

static int16_t le16(const uint8_t *p)
{
    return (int16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

esp_err_t prisma_imu_init(void)
{
    i2c_master_bus_handle_t bus = prisma_bsp_i2c_bus();
    if (!bus) return ESP_ERR_INVALID_STATE;

    i2c_device_config_t cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = PRISMA_IMU_ADDR,
        .scl_speed_hz = 400000,
    };
    esp_err_t err = i2c_master_bus_add_device(bus, &cfg, &s_dev);
    if (err != ESP_OK) return err;

    uint8_t who = 0, rev = 0;
    if ((err = reg_read(QMI8658_WHO_AM_I, &who, 1)) != ESP_OK) return err;
    (void)reg_read(QMI8658_REVISION, &rev, 1);
    if (who != 0x05) {
        ESP_LOGE(TAG, "unexpected WHO_AM_I=0x%02X", who);
        return ESP_ERR_NOT_FOUND;
    }

    /* Address auto-increment, accelerometer +-4g/125Hz, gyro +-512dps/125Hz. */
    if ((err = reg_write(QMI8658_CTRL1, 0x40)) != ESP_OK) return err;
    if ((err = reg_write(QMI8658_CTRL2, 0x23)) != ESP_OK) return err;
    if ((err = reg_write(QMI8658_CTRL3, 0x43)) != ESP_OK) return err;
    if ((err = reg_write(QMI8658_CTRL7, 0x03)) != ESP_OK) return err;

    memset(&s_state, 0, sizeof(s_state));
    s_state.ready = true;
    ESP_LOGI(TAG, "QMI8658 ready who=0x%02X rev=0x%02X", who, rev);
    return ESP_OK;
}

esp_err_t prisma_imu_read(prisma_imu_state_t *state)
{
    if (!s_dev || !s_state.ready) return ESP_ERR_INVALID_STATE;

    uint8_t temp[2];
    uint8_t raw[12];
    esp_err_t err = reg_read(QMI8658_TEMP_L, temp, sizeof(temp));
    if (err != ESP_OK) return err;
    err = reg_read(QMI8658_AX_L, raw, sizeof(raw));
    if (err != ESP_OK) return err;

    /* QMI8658 scale factors for the configured +-4g and +-512dps ranges. */
    s_state.temperature_c = (float)le16(temp) / 256.0f;
    s_state.ax_g = (float)le16(&raw[0]) / 8192.0f;
    s_state.ay_g = (float)le16(&raw[2]) / 8192.0f;
    s_state.az_g = (float)le16(&raw[4]) / 8192.0f;
    s_state.gx_dps = (float)le16(&raw[6]) / 64.0f;
    s_state.gy_dps = (float)le16(&raw[8]) / 64.0f;
    s_state.gz_dps = (float)le16(&raw[10]) / 64.0f;
    s_state.sample_count++;
    if (state) *state = s_state;
    return ESP_OK;
}

void prisma_imu_get_state(prisma_imu_state_t *state)
{
    if (state) *state = s_state;
}
