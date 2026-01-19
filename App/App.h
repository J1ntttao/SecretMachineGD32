#ifndef __APP_H__
#define __APP_H__

#include "gd32f4xx.h"

#include <stdio.h>
#include <string.h>

// bsp
#include "bsp_ws2812.h"
#include "bsp_keys.h"
#include "oled.h"

// FreeRTOS NEEDS:
#include "FreeRTOS.h"
#include "task.h"
#include "list.h"
#include "semphr.h" // semaphore

// Ç¯Î»ºê
#define CLIP_VALUE(value, min, max) do {\
	if (value < min) value = min;			    \
	else if (value > max) value = max;		\
} while(0)


extern SemaphoreHandle_t sema_WS2812Re_handler;

void vTaskKey();         
void vTaskOLED();        
void vTaskGameProgress(); 
void vTaskWS2812_1();      
void vTaskWS2812_2();

#endif