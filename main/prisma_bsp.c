#include "prisma_bsp.h"
#include "prisma_board.h"

#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/i2c_master.h"
#include "esp_check.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LCD_HOST SPI2_HOST
#define W PRISMA_LCD_WIDTH
#define H PRISMA_LCD_HEIGHT

static const char *TAG = "PRISMA_BSP";
static esp_lcd_panel_handle_t s_panel;
static i2c_master_bus_handle_t s_i2c;
static uint16_t s_frame[W * H];

static inline uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b)
{
    uint16_t c = ((r & 0xF8U) << 8) | ((g & 0xFCU) << 3) | (b >> 3);
    return (uint16_t)((c >> 8) | (c << 8));
}

static void fill(uint16_t c)
{
    for (size_t i = 0; i < W * H; ++i) s_frame[i] = c;
}

static void rect(int x, int y, int w, int h, uint16_t c)
{
    for (int yy = y; yy < y + h && yy < H; ++yy) {
        if (yy < 0) continue;
        for (int xx = x; xx < x + w && xx < W; ++xx) {
            if (xx >= 0) s_frame[yy * W + xx] = c;
        }
    }
}

/* Minimal 5x7 glyph set required by the B0 diagnostic screen. */
static const uint8_t G_SPACE[5]={0,0,0,0,0}, G_DASH[5]={8,8,8,8,8};
static const uint8_t G_0[5]={0x3e,0x51,0x49,0x45,0x3e},G_1[5]={0,0x42,0x7f,0x40,0},G_2[5]={0x42,0x61,0x51,0x49,0x46},G_3[5]={0x21,0x41,0x45,0x4b,0x31},G_4[5]={0x18,0x14,0x12,0x7f,0x10},G_5[5]={0x27,0x45,0x45,0x45,0x39},G_6[5]={0x3c,0x4a,0x49,0x49,0x30},G_7[5]={1,0x71,9,5,3},G_8[5]={0x36,0x49,0x49,0x49,0x36},G_9[5]={6,0x49,0x49,0x29,0x1e};
static const uint8_t G_A[5]={0x7e,0x11,0x11,0x11,0x7e},G_B[5]={0x7f,0x49,0x49,0x49,0x36},G_C[5]={0x3e,0x41,0x41,0x41,0x22},G_D[5]={0x7f,0x41,0x41,0x22,0x1c},G_E[5]={0x7f,0x49,0x49,0x49,0x41},G_F[5]={0x7f,9,9,9,1},G_H[5]={0x7f,8,8,8,0x7f},G_I[5]={0,0x41,0x7f,0x41,0},G_K[5]={0x7f,8,0x14,0x22,0x41},G_L[5]={0x7f,0x40,0x40,0x40,0x40},G_M[5]={0x7f,2,0x0c,2,0x7f},G_N[5]={0x7f,4,8,0x10,0x7f},G_O[5]={0x3e,0x41,0x41,0x41,0x3e},G_P[5]={0x7f,9,9,9,6},G_R[5]={0x7f,9,0x19,0x29,0x46},G_S[5]={0x46,0x49,0x49,0x49,0x31},G_T[5]={1,1,0x7f,1,1},G_U[5]={0x3f,0x40,0x40,0x40,0x3f},G_V[5]={0x1f,0x20,0x40,0x20,0x1f},G_Y[5]={7,8,0x70,8,7};

static const uint8_t *glyph(char c)
{
    static const uint8_t *digits[]={G_0,G_1,G_2,G_3,G_4,G_5,G_6,G_7,G_8,G_9};
    if (c >= '0' && c <= '9') return digits[c-'0'];
    switch(c) {
        case '-':return G_DASH; case 'A':return G_A; case 'B':return G_B; case 'C':return G_C;
        case 'D':return G_D; case 'E':return G_E; case 'F':return G_F; case 'H':return G_H;
        case 'I':return G_I; case 'K':return G_K; case 'L':return G_L; case 'M':return G_M;
        case 'N':return G_N; case 'O':return G_O; case 'P':return G_P; case 'R':return G_R;
        case 'S':return G_S; case 'T':return G_T; case 'U':return G_U; case 'V':return G_V;
        case 'Y':return G_Y; default:return G_SPACE;
    }
}

static void text(int x, int y, const char *s, int scale, uint16_t c)
{
    while (*s) {
        const uint8_t *g = glyph(*s++);
        for (int col=0; col<5; ++col) for (int row=0; row<7; ++row)
            if (g[col] & (1U<<row)) rect(x+col*scale,y+row*scale,scale,scale,c);
        x += 6*scale;
    }
}

esp_err_t prisma_bsp_power_hold_init(void)
{
    gpio_config_t cfg={.pin_bit_mask=1ULL<<PRISMA_BAT_EN,.mode=GPIO_MODE_OUTPUT};
    ESP_RETURN_ON_ERROR(gpio_config(&cfg), TAG, "BAT_EN gpio");
    return gpio_set_level(PRISMA_BAT_EN, 1);
}

esp_err_t prisma_bsp_buttons_init(void)
{
    gpio_config_t cfg={
        .pin_bit_mask=(1ULL<<PRISMA_KEY_BOOT)|(1ULL<<PRISMA_KEY_PLUS)|(1ULL<<PRISMA_KEY_PWR),
        .mode=GPIO_MODE_INPUT,.pull_up_en=GPIO_PULLUP_ENABLE,.pull_down_en=GPIO_PULLDOWN_DISABLE
    };
    return gpio_config(&cfg);
}

esp_err_t prisma_bsp_display_init(void)
{
    gpio_config_t bl={.pin_bit_mask=1ULL<<PRISMA_LCD_BL,.mode=GPIO_MODE_OUTPUT};
    ESP_RETURN_ON_ERROR(gpio_config(&bl), TAG, "backlight gpio");
    gpio_set_level(PRISMA_LCD_BL, 0);

    spi_bus_config_t bus={.sclk_io_num=PRISMA_LCD_SCLK,.mosi_io_num=PRISMA_LCD_MOSI,.miso_io_num=-1,.quadwp_io_num=-1,.quadhd_io_num=-1,.max_transfer_sz=W*40*2};
    ESP_RETURN_ON_ERROR(spi_bus_initialize(LCD_HOST,&bus,SPI_DMA_CH_AUTO), TAG, "SPI init");

    esp_lcd_panel_io_handle_t io=NULL;
    esp_lcd_panel_io_spi_config_t io_cfg={.dc_gpio_num=PRISMA_LCD_DC,.cs_gpio_num=PRISMA_LCD_CS,.pclk_hz=40*1000*1000,.lcd_cmd_bits=8,.lcd_param_bits=8,.spi_mode=0,.trans_queue_depth=10};
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST,&io_cfg,&io), TAG, "panel io");

    esp_lcd_panel_dev_config_t panel_cfg={.reset_gpio_num=PRISMA_LCD_RST,.rgb_ele_order=LCD_RGB_ELEMENT_ORDER_RGB,.bits_per_pixel=16};
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_st7789(io,&panel_cfg,&s_panel), TAG, "ST7789");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_reset(s_panel), TAG, "panel reset");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_init(s_panel), TAG, "panel init");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_invert_color(s_panel, true), TAG, "panel invert");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_disp_on_off(s_panel, true), TAG, "panel on");
    gpio_set_level(PRISMA_LCD_BL, 1);
    return ESP_OK;
}

static bool probe(uint8_t addr)
{
    return s_i2c && i2c_master_probe(s_i2c, addr, 100) == ESP_OK;
}

esp_err_t prisma_bsp_i2c_probe(prisma_b0_status_t *status)
{
    if (!status) return ESP_ERR_INVALID_ARG;
    gpio_config_t rst={.pin_bit_mask=1ULL<<PRISMA_TOUCH_RST,.mode=GPIO_MODE_OUTPUT};
    ESP_RETURN_ON_ERROR(gpio_config(&rst), TAG, "touch reset gpio");
    gpio_set_level(PRISMA_TOUCH_RST,0); vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(PRISMA_TOUCH_RST,1); vTaskDelay(pdMS_TO_TICKS(80));

    i2c_master_bus_config_t cfg={.i2c_port=I2C_NUM_0,.sda_io_num=PRISMA_I2C_SDA,.scl_io_num=PRISMA_I2C_SCL,.clk_source=I2C_CLK_SRC_DEFAULT,.glitch_ignore_cnt=7,.flags.enable_internal_pullup=true};
    ESP_RETURN_ON_ERROR(i2c_new_master_bus(&cfg,&s_i2c), TAG, "I2C bus");
    status->touch_ok=probe(PRISMA_TOUCH_ADDR);
    status->imu_seen=probe(PRISMA_IMU_ADDR);
    status->es8311_seen=probe(PRISMA_ES8311_ADDR);
    status->es7210_seen=probe(PRISMA_ES7210_ADDR);
    ESP_LOGI(TAG,"I2C touch=%d imu=%d es8311=%d es7210=%d",status->touch_ok,status->imu_seen,status->es8311_seen,status->es7210_seen);
    return ESP_OK;
}

void prisma_bsp_draw_b0(const prisma_b0_status_t *s)
{
    if (!s_panel || !s) return;
    const uint16_t bg=rgb565(4,7,12), white=rgb565(238,244,248), cyan=rgb565(35,220,220), green=rgb565(40,210,120), red=rgb565(235,65,75), dim=rgb565(55,68,82);
    fill(bg);
    text(18,14,"PRISMA",4,white); text(18,48,"B0 HARDWARE",2,cyan);
    struct row {const char *name; bool ok;} rows[]={
        {"DISPLAY",s->display_ok},{"TOUCH",s->touch_ok},{"PSRAM",s->psram_ok},{"NVS",s->nvs_ok},
        {"IMU",s->imu_seen},{"AUDIO",s->es8311_seen && s->es7210_seen}
    };
    for (int i=0;i<6;i++) {int y=82+i*23;text(18,y,rows[i].name,2,white);rect(176,y-3,46,19,rows[i].ok?green:red);text(184,y+1,rows[i].ok?"OK":"FAIL",1,white);}
    rect(18,222,204,2,dim);
    ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(s_panel,0,0,W,H,s_frame));
}
