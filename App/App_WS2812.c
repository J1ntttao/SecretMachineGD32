#include "App.h" 
#include "Delay.h"

// xTaskCreate( vTaskWS2812,       "vTaskWS2812",         512, NULL, 2, &xTaskWS2812_Handle);

// MAIN_NUM   28+ 7=35颗灯 SPI1=>PB15
// OTHER_NUM  28+28=56颗灯 SPI0=>PA07

//  0     1     2     3     4
//  5     6     7     8     9
//  10    11    12    13    14
//  15    16    17    18    19
//  20    21    22    23    24
//  25    26    27    28    29
//  30    31    32    33    34
//  35  36  37  38  39  40  41

/** 按键排布 **********************************************************
 *         	             KEY1                                        *
 *               KEY3	 		 KEY2       KEY5                     *
 *      	             KEY4                                        *
 *********************************************************************/  
void vTaskWS2812_1(){
    EventBits_t uxBits;
    
    while(1){  
        uxBits = xEventGroupWaitBits(
            KEY_eventgroup_handle,  // 事件组句柄
            TOGGLE_COLOR,           // 关心的事件标志，多个用 |,+ 连在一起
            pdTRUE,                 // 退出时是否清理标记(置0), 只会清理关心的标记
            pdTRUE,                 // 所有关心的标志同时为1时，才解除阻塞
            pdMS_TO_TICKS(2222)     // 等待超时时间：2.22s
        );        
        WS2812_display(1);                
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}


void vTaskWS2812_2(){
    EventBits_t uxBits;     
    
    while(1){
        uxBits = xEventGroupWaitBits(
            KEY_eventgroup_handle,  // 事件组句柄
            CHECK_COLOR,            // 关心的事件标志，多个用 |,+ 连在一起
            pdTRUE,                 // 退出时是否清理标记(置0), 只会清理关心的标记
            pdTRUE,                 // 所有关心的标志同时为1时，才解除阻塞
            pdMS_TO_TICKS(3333)     // 等待超时时间： 3.33s
        );
        WS2812_display(2);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}