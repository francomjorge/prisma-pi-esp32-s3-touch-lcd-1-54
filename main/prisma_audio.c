#include "prisma_audio.h"
#include "prisma_bsp.h"
#include "prisma_board.h"

#include <math.h>
#include <string.h>
#include "driver/i2s_std.h"
#include "driver/i2s_tdm.h"
#include "esp_codec_dev.h"
#include "esp_codec_dev_defaults.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SAMPLE_RATE 16000
#define FRAME_SAMPLES 320
static const char *TAG="PRISMA_AUDIO";
static i2s_chan_handle_t s_tx;
static i2s_chan_handle_t s_rx;
static esp_codec_dev_handle_t s_out;
static esp_codec_dev_handle_t s_in;
static prisma_audio_state_t s_state={.input_gain_db=30,.output_volume=55,.mic_enabled=true,.speaker_enabled=true};

static void capture_task(void *arg)
{
    (void)arg;
    int16_t pcm[FRAME_SAMPLES*4];
    while(1){
        if(!s_state.ready||!s_state.mic_enabled){vTaskDelay(pdMS_TO_TICKS(50));continue;}
        if(esp_codec_dev_read(s_in,pcm,sizeof(pcm))!=ESP_CODEC_DEV_OK){vTaskDelay(pdMS_TO_TICKS(20));continue;}
        uint32_t peak=0;double sum=0.0;size_t n=sizeof(pcm)/sizeof(pcm[0]);
        for(size_t i=0;i<n;i+=4){int32_t v=pcm[i];uint32_t a=(uint32_t)(v<0?-v:v);if(a>peak)peak=a;sum+=(double)v*(double)v;}
        double rms=sqrt(sum/(double)(n/4));
        s_state.level=(float)(rms/32768.0);if(s_state.level>1.0f)s_state.level=1.0f;
        float p=(float)peak/32768.0f;if(p>s_state.peak)s_state.peak=p;else s_state.peak*=0.94f;
    }
}

esp_err_t prisma_audio_init(void)
{
    i2c_master_bus_handle_t bus=prisma_bsp_i2c_bus();if(!bus)return ESP_ERR_INVALID_STATE;
    i2s_chan_config_t chan={.id=I2S_NUM_0,.role=I2S_ROLE_MASTER,.dma_desc_num=6,.dma_frame_num=240,.auto_clear_after_cb=true,.auto_clear_before_cb=false,.intr_priority=0};
    ESP_ERROR_CHECK(i2s_new_channel(&chan,&s_tx,&s_rx));
    i2s_std_config_t out_cfg={.clk_cfg={.sample_rate_hz=SAMPLE_RATE,.clk_src=I2S_CLK_SRC_DEFAULT,.ext_clk_freq_hz=0,.mclk_multiple=I2S_MCLK_MULTIPLE_256},.slot_cfg={.data_bit_width=I2S_DATA_BIT_WIDTH_16BIT,.slot_bit_width=I2S_SLOT_BIT_WIDTH_AUTO,.slot_mode=I2S_SLOT_MODE_STEREO,.slot_mask=I2S_STD_SLOT_BOTH,.ws_width=I2S_DATA_BIT_WIDTH_16BIT,.ws_pol=false,.bit_shift=true,.left_align=true,.big_endian=false,.bit_order_lsb=false},.gpio_cfg={.mclk=PRISMA_I2S_MCLK,.bclk=PRISMA_I2S_BCLK,.ws=PRISMA_I2S_LRCK,.dout=PRISMA_I2S_SPK_DOUT,.din=I2S_GPIO_UNUSED,.invert_flags={0}}};
    i2s_tdm_config_t in_cfg={.clk_cfg={.sample_rate_hz=SAMPLE_RATE,.clk_src=I2S_CLK_SRC_DEFAULT,.ext_clk_freq_hz=0,.mclk_multiple=I2S_MCLK_MULTIPLE_256,.bclk_div=8},.slot_cfg={.data_bit_width=I2S_DATA_BIT_WIDTH_16BIT,.slot_bit_width=I2S_SLOT_BIT_WIDTH_AUTO,.slot_mode=I2S_SLOT_MODE_STEREO,.slot_mask=I2S_TDM_SLOT0|I2S_TDM_SLOT1|I2S_TDM_SLOT2|I2S_TDM_SLOT3,.ws_width=I2S_TDM_AUTO_WS_WIDTH,.ws_pol=false,.bit_shift=true,.left_align=false,.big_endian=false,.bit_order_lsb=false,.skip_mask=false,.total_slot=I2S_TDM_AUTO_SLOT_NUM},.gpio_cfg={.mclk=PRISMA_I2S_MCLK,.bclk=PRISMA_I2S_BCLK,.ws=PRISMA_I2S_LRCK,.dout=I2S_GPIO_UNUSED,.din=PRISMA_I2S_MIC_DIN,.invert_flags={0}}};
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(s_tx,&out_cfg));ESP_ERROR_CHECK(i2s_channel_init_tdm_mode(s_rx,&in_cfg));
    audio_codec_i2s_cfg_t data_cfg={.port=I2S_NUM_0,.rx_handle=s_rx,.tx_handle=s_tx};const audio_codec_data_if_t *data_if=audio_codec_new_i2s_data(&data_cfg);if(!data_if)return ESP_FAIL;
    audio_codec_i2c_cfg_t ctrl_cfg={.addr=ES8311_CODEC_DEFAULT_ADDR,.bus_handle=bus};const audio_codec_ctrl_if_t *out_ctrl=audio_codec_new_i2c_ctrl(&ctrl_cfg);const audio_codec_gpio_if_t *gpio_if=audio_codec_new_gpio();if(!out_ctrl||!gpio_if)return ESP_FAIL;
    es8311_codec_cfg_t out_codec_cfg={.codec_mode=ESP_CODEC_DEV_WORK_MODE_DAC,.ctrl_if=out_ctrl,.gpio_if=gpio_if,.pa_pin=PRISMA_PA_CTRL,.use_mclk=true,.hw_gain={.pa_voltage=5.0,.codec_dac_voltage=3.3}};const audio_codec_if_t *out_codec=es8311_codec_new(&out_codec_cfg);if(!out_codec)return ESP_FAIL;
    esp_codec_dev_cfg_t dev_cfg={.dev_type=ESP_CODEC_DEV_TYPE_OUT,.codec_if=out_codec,.data_if=data_if};s_out=esp_codec_dev_new(&dev_cfg);if(!s_out)return ESP_FAIL;
    ctrl_cfg.addr=ES7210_CODEC_DEFAULT_ADDR;const audio_codec_ctrl_if_t *in_ctrl=audio_codec_new_i2c_ctrl(&ctrl_cfg);if(!in_ctrl)return ESP_FAIL;
    es7210_codec_cfg_t in_codec_cfg={.ctrl_if=in_ctrl,.mic_selected=ES7210_SEL_MIC1|ES7210_SEL_MIC2|ES7210_SEL_MIC3|ES7210_SEL_MIC4};const audio_codec_if_t *in_codec=es7210_codec_new(&in_codec_cfg);if(!in_codec)return ESP_FAIL;
    dev_cfg.dev_type=ESP_CODEC_DEV_TYPE_IN;dev_cfg.codec_if=in_codec;s_in=esp_codec_dev_new(&dev_cfg);if(!s_in)return ESP_FAIL;
    esp_codec_dev_sample_info_t fs={.bits_per_sample=16,.channel=1,.channel_mask=0,.sample_rate=SAMPLE_RATE,.mclk_multiple=0};if(esp_codec_dev_open(s_out,&fs)!=ESP_CODEC_DEV_OK)return ESP_FAIL;
    fs.channel=4;fs.channel_mask=ESP_CODEC_DEV_MAKE_CHANNEL_MASK(0);if(esp_codec_dev_open(s_in,&fs)!=ESP_CODEC_DEV_OK)return ESP_FAIL;
    esp_codec_dev_set_out_vol(s_out,s_state.output_volume);esp_codec_dev_set_in_channel_gain(s_in,ESP_CODEC_DEV_MAKE_CHANNEL_MASK(0),s_state.input_gain_db);
    s_state.ready=true;xTaskCreate(capture_task,"audio_meter",4096,NULL,5,NULL);ESP_LOGI(TAG,"B2 audio ready: 16kHz ES7210 -> ESP32-S3 -> ES8311/NS4150B");return ESP_OK;
}

void prisma_audio_get_state(prisma_audio_state_t *state){if(state)*state=s_state;}
esp_err_t prisma_audio_set_mic(bool enabled){s_state.mic_enabled=enabled;if(!s_in)return ESP_ERR_INVALID_STATE;esp_codec_dev_set_in_gain(s_in,enabled?(float)s_state.input_gain_db:0.0f);return ESP_OK;}
esp_err_t prisma_audio_set_speaker(bool enabled){s_state.speaker_enabled=enabled;if(!s_out)return ESP_ERR_INVALID_STATE;esp_codec_dev_set_out_vol(s_out,enabled?(float)s_state.output_volume:0.0f);return ESP_OK;}
esp_err_t prisma_audio_set_input_gain(int gain_db){if(gain_db<0)gain_db=0;if(gain_db>42)gain_db=42;s_state.input_gain_db=gain_db;if(s_in&&s_state.mic_enabled)esp_codec_dev_set_in_gain(s_in,(float)gain_db);return ESP_OK;}
esp_err_t prisma_audio_set_output_volume(int volume){if(volume<0)volume=0;if(volume>100)volume=100;s_state.output_volume=volume;if(s_out&&s_state.speaker_enabled)esp_codec_dev_set_out_vol(s_out,(float)volume);return ESP_OK;}
esp_err_t prisma_audio_beep(uint32_t hz,uint32_t ms){if(!s_out||!s_state.speaker_enabled||hz<20||ms==0)return ESP_ERR_INVALID_STATE;int16_t frame[320];uint32_t total=SAMPLE_RATE*ms/1000;uint32_t phase=0,step=(uint32_t)(((uint64_t)hz<<24)/SAMPLE_RATE);while(total){size_t n=total>320?320:total;for(size_t i=0;i<n;i++){phase+=step;frame[i]=(phase&0x800000)?7000:-7000;}if(esp_codec_dev_write(s_out,frame,n*sizeof(int16_t))!=ESP_CODEC_DEV_OK)return ESP_FAIL;total-=(uint32_t)n;}return ESP_OK;}
