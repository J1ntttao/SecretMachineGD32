#ifndef __APP_H__
#define __APP_H__

#include "gd32f4xx.h"

#include <stdio.h>
#include <string.h>

// bsp
#include "driver_ws2812b.h"
#include "bsp_keys.h"
#include "oled.h"

// FreeRTOS NEEDS:
#include "FreeRTOS.h"
#include "task.h"
#include "list.h"
#include "semphr.h" 
#include "queue.h"
#include "event_groups.h"

// 待会倒计时要用到的神奇妙妙工具：
extern SemaphoreHandle_t  CountDown_Semaphore;
// 待会OLED要用到的神奇妙妙工具：
extern EventGroupHandle_t OLED_eventgroup_handle;
// 待会KEY要用到的神奇妙妙工具：
extern EventGroupHandle_t KEY_eventgroup_handle;

#define TOGGLE_COLOR ( 1 << 0 )
#define CHECK_COLOR  ( 1 << 1 )
#define REFRESH_OLED ( 1 << 2 )
#define BIT_3 ( 1 << 3 )
#define BIT_4 ( 1 << 4 )

// 状态机
enum {
    InitState       =   0,
    SwDiffState     =   1,
    SwLevelState    =   2,
    SetConfigState  =   3,
    StartState      =   4,
    SuccessState    =   5,
    DefeatState     =   6
};

extern uint8_t g_currentState;
    
void vTaskKey();         
void vTaskOLED();        
void vTaskGameProgress(); 
void vTaskWS2812_1();      
void vTaskWS2812_2();
void vTaskTime(void *pvParameters);

// APP_OLED.c
enum {  // 关卡难度
    Normal  = 0,
    Hard    = 1,
    Experts = 2
};
extern int8_t g_cur_Diff;
extern uint8_t g_cur_level;
extern uint8_t g_cur_config_sw;

extern int8_t g_lv_steps;
extern int8_t g_lv_light;
extern int32_t g_lv_time;

extern int32_t g_cur_time;
extern int8_t g_cur_steps;
extern int8_t g_cur_light;

extern void State_Change();

#include "u8g2.h"
extern u8g2_t u8g2;

// POS:
extern int8_t g_currentPos;
extern uint8_t g_currentLine;
extern uint8_t g_currentSteps;
void Change_Pos(int8_t dir);
void Change_Line();

// COLOR:
extern uint32_t COLORS[7];
extern uint32_t g_currentColor;
void Toggle_Color(int8_t dir);

// GAMING:
extern int8_t Normal_Checked();

#endif