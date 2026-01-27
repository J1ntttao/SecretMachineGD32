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
static BaseType_t g_timer_enable = pdFALSE;  // 默认关闭

void vTaskTime(void *pvParameters){
    
    while(1){
        // 等待开关信号
        if(xSemaphoreTake(CountDown_Semaphore, 0) == pdTRUE){
            // 收到信号，切换开关状态
            g_timer_enable = !g_timer_enable;
        }
        
        // 如果开关开启，则执行倒计时
        if(g_timer_enable && (g_cur_time > 0)){
            g_cur_time--;
        }
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


#if 0
// 在其他任务中切换开关
void toggle_timer_switch(void){
    if(xTimerSwitchSem != NULL){
        xSemaphoreGive(xTimerSwitchSem);
    }
}

void vTaskTime(){
    // BaseType_t xReturn = pdFALSE; // pdTRUE
    while(1){
        // 阻塞等待，直到获取到信号（一直等）
        // xReturn = xSemaphoreTake(CountDown_Semaphore, portMAX_DELAY); // portMAX_DELAY, pdMS_TO_TICKS(2000)
        // 这里想要有一个开关，这个开关开启后g_cur_time一直减反复调用且不阻塞，信号量只能执行一次
        
        g_cur_time = g_lv_time;
        if(g_cur_time > 0){
            g_cur_time--;
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
#endif
