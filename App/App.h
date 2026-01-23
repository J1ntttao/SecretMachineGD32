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

// 待会KEY要用到的神器妙妙工具：
extern EventGroupHandle_t KEY_eventgroup_handle;

#define TOGGLE_COLOR ( 1 << 0 )
#define CHECK_COLOR  ( 1 << 1 )
#define BIT_2 ( 1 << 2 )
#define BIT_3 ( 1 << 3 )
#define BIT_4 ( 1 << 4 )

// 状态机
enum {
    InitState       =   0,
    SwDiffState     =   1,
    SwLevelState    =   2,
    SetConfigState  =   3,
    StartState      =   4,
};
extern uint8_t g_currentState;
    
void vTaskKey();         
void vTaskOLED();        
void vTaskGameProgress(); 
void vTaskWS2812_1();      
void vTaskWS2812_2();

// POS:
extern int8_t g_currentPos;
extern uint8_t g_currentLine;
void Change_Pos(int8_t dir);
void Change_Line();

// COLOR:
extern uint32_t COLORS[7];
extern uint32_t g_currentColor;
void Toggle_Color(int8_t dir);
    
#endif