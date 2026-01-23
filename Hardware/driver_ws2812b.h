/**
 * @file ws2812b_driver.h
 * @brief WS2812B LEDÇý¶¯
 */

#ifndef __DRIVER_WS2812B_H__
#define __DRIVER_WS2812B_H__

#include "gd32f4xx.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "SPI.h"

// Ô¤¶¨ÒåÑÕÉ«    
#define COLOR_RED        0xFF0000  // ºì
#define COLOR_ORANGE     0xFFA500  // ³È  
#define COLOR_YELLOW     0xFFFF00  // »Æ
#define COLOR_GREEN      0x00FF00  // ÂÌ
#define COLOR_CYAN       0x00FFFF  // Çà
#define COLOR_BLUE       0x0000FF  // À¶
#define COLOR_PURPLE     0xFF00FF  // ×Ï 
    
#define WS2812_NUM1      42
#define WS2812_NUM2      63

void WS2812_init(void);
void WS2812_set_color(uint8_t whichline, uint16_t index, uint32_t color);
void WS2812_set_color_brightness(uint8_t whichline, uint16_t index, uint32_t color, uint8_t value);
void WS2812_display(uint8_t whichline);
void WS2812_set_range_color(uint8_t whichline, uint16_t start, uint16_t end, uint32_t color);
void WS2812_clear(uint8_t whichline);

#endif /* __WS2812B_DRIVER_H */