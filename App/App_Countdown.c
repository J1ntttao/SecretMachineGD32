#include "App.h"
/*
     int8_t g_cur_Diff  = Normal;
    uint8_t g_cur_level = 1;
    uint8_t g_cur_config_sw = 1;

    int32_t g_lv_time  = 0;
     int8_t g_lv_steps = 7;
     int8_t g_lv_light = 7;

    int32_t g_cur_time  = 0;
     int8_t g_cur_steps = 0;
     int8_t g_cur_light = 0;
*/

// 定义开关变量
BaseType_t g_cd_enable = pdFALSE;  // 默认关闭

void vTaskTime(void *pvParameters){

    while(1){
        // 等待开关信号
        if(g_cd_enable == pdFALSE){
            vTaskDelay(pdMS_TO_TICKS(5));
            continue;
        }

        // g_cd_enable == pdTRUE
        vTaskDelay(pdMS_TO_TICKS(985));
        xEventGroupSetBits(OLED_eventgroup_handle, REFRESH_OLED);
        // 如果开关开启，则执行倒计时
        if(g_cd_enable && (g_cur_time > -1)){
            g_using_time++;
            if(g_cur_time == 0){
                GameTimeout();
                continue;
            }
            g_cur_time--;
            vTaskDelay(pdMS_TO_TICKS(10));
            xEventGroupSetBits(OLED_eventgroup_handle, REFRESH_OLED);
        }
               
    }
}
