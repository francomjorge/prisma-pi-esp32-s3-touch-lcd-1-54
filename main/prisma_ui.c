#include "prisma_ui.h"
#include "prisma_bsp.h"
#include "esp_log.h"
#include "esp_random.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdlib.h>

static const char *TAG="PRISMA_UI";

typedef enum { SCREEN_MAIN=0, SCREEN_QUICK=1 } screen_t;

static void present_delay(int ms){ESP_ERROR_CHECK(prisma_bsp_frame_present());vTaskDelay(pdMS_TO_TICKS(ms));}
static void intro(void)
{
    prisma_bsp_frame_clear(0,0,0);present_delay(120);
    prisma_bsp_frame_text(36,96,"PRISMA",4,235,240,245);present_delay(180);
    for(int f=0;f<11;f++){
        prisma_bsp_frame_clear(0,0,0);
        int dx=(int)(esp_random()%11)-5,dy=(int)(esp_random()%7)-3;
        prisma_bsp_frame_text(36+dx,96+dy,"PRISMA",4,235,240,245);
        for(int i=0;i<5;i++){int y=(int)(esp_random()%220);int h=2+(int)(esp_random()%8);int x=(int)(esp_random()%70);int w=80+(int)(esp_random()%150);if(f&1)prisma_bsp_frame_rect(x,y,w,h,20,220,220);else prisma_bsp_frame_rect(x,y,w,h,220,30,100);}
        present_delay(35+(int)(esp_random()%35));
    }
    prisma_bsp_frame_clear(0,0,0);prisma_bsp_frame_text(36,96,"PRISMA",4,235,240,245);present_delay(260);
}

static void chrome(const char *title,int page)
{
    prisma_bsp_frame_clear(4,7,12);
    prisma_bsp_frame_text(12,10,"PRISMA",2,235,242,247);
    prisma_bsp_frame_text(12,34,title,2,40,215,215);
    prisma_bsp_frame_rect(12,58,216,1,45,58,70);
    if(page==0){prisma_bsp_frame_rect(101,226,16,3,40,215,215);prisma_bsp_frame_rect(122,226,8,3,55,65,75);}else{prisma_bsp_frame_rect(105,226,8,3,55,65,75);prisma_bsp_frame_rect(118,226,16,3,40,215,215);}
}
static void main_screen(void)
{
    chrome("NODE STATUS",0);
    prisma_bsp_frame_text(14,74,"LOCAL",2,235,242,247);prisma_bsp_frame_text(154,74,"READY",1,45,220,125);
    prisma_bsp_frame_text(14,104,"MIC",2,235,242,247);prisma_bsp_frame_rect(70,104,145,12,20,28,35);prisma_bsp_frame_rect(72,106,28,8,35,215,215);
    prisma_bsp_frame_text(14,136,"NETWORK",2,235,242,247);prisma_bsp_frame_text(154,140,"B5",1,115,125,135);
    prisma_bsp_frame_text(14,168,"TOUCH",2,235,242,247);prisma_bsp_frame_text(154,172,"ACTIVE",1,45,220,125);
    prisma_bsp_frame_text(14,202,"SWIPE FOR CONTROLS",1,130,145,155);
    ESP_ERROR_CHECK(prisma_bsp_frame_present());
}
static void quick_screen(void)
{
    chrome("QUICK CONTROL",1);
    prisma_bsp_frame_text(14,74,"MIC",2,235,242,247);prisma_bsp_frame_rect(158,70,58,22,40,65,75);prisma_bsp_frame_text(177,77,"B2",1,235,242,247);
    prisma_bsp_frame_text(14,108,"SPEAKER",2,235,242,247);prisma_bsp_frame_rect(158,104,58,22,40,65,75);prisma_bsp_frame_text(177,111,"B2",1,235,242,247);
    prisma_bsp_frame_text(14,142,"BEEP",2,235,242,247);prisma_bsp_frame_text(158,146,"B2",1,115,125,135);
    prisma_bsp_frame_text(14,176,"BRIGHT",2,235,242,247);prisma_bsp_frame_text(158,180,"ON",1,45,220,125);
    prisma_bsp_frame_text(14,208,"SWIPE BACK",1,130,145,155);
    ESP_ERROR_CHECK(prisma_bsp_frame_present());
}
static void draw(screen_t s){if(s==SCREEN_MAIN)main_screen();else quick_screen();}

esp_err_t prisma_ui_run(void)
{
    intro();screen_t screen=SCREEN_MAIN;draw(screen);
    bool touching=false;uint16_t sx=0,sy=0;TickType_t down_at=0;bool plus_old=false,boot_old=false,pwr_old=false;
    while(1){
        prisma_touch_sample_t t={0};bool touch_ok=prisma_bsp_touch_read(&t)==ESP_OK;
        if(touch_ok&&t.pressed&&!touching){touching=true;sx=t.x;sy=t.y;down_at=xTaskGetTickCount();}
        if((!touch_ok||!t.pressed)&&touching){
            touching=false;TickType_t held=xTaskGetTickCount()-down_at;int dx=(int)t.x-(int)sx;int dy=(int)t.y-(int)sy;
            if(abs(dx)>45&&abs(dx)>abs(dy)){screen=screen==SCREEN_MAIN?SCREEN_QUICK:SCREEN_MAIN;draw(screen);ESP_LOGI(TAG,"swipe -> screen %d",screen);}
            else if(held>pdMS_TO_TICKS(650)){ESP_LOGI(TAG,"long press x=%u y=%u",sx,sy);}
            else {ESP_LOGI(TAG,"tap x=%u y=%u",sx,sy);}
        }
        bool plus=prisma_bsp_button_pressed(PRISMA_BUTTON_PLUS),boot=prisma_bsp_button_pressed(PRISMA_BUTTON_BOOT),pwr=prisma_bsp_button_pressed(PRISMA_BUTTON_PWR);
        if(plus&&!plus_old){screen=screen==SCREEN_MAIN?SCREEN_QUICK:SCREEN_MAIN;draw(screen);}
        if(boot&&!boot_old){screen=SCREEN_MAIN;draw(screen);}
        if(pwr&&!pwr_old)ESP_LOGI(TAG,"PWR press observed; shutdown semantics reserved for B11");
        plus_old=plus;boot_old=boot;pwr_old=pwr;vTaskDelay(pdMS_TO_TICKS(20));
    }
    return ESP_OK;
}
