#include "driver_ws2812b.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// SPI 10.5MHZ下
// 0和1的10位模式
#define BIT_0 0b1110000000  // 0x380 - 3高7低
#define BIT_1 0b1111111000  // 0x3F8 - 7高3低


// 使用连续内存，避免对齐问题
uint8_t colors1[WS2812_NUM1 * 30]; // Line 1
uint8_t colors2[WS2812_NUM2 * 30]; // Line 2

// 优化：使用查找表提高效率
static uint8_t bit_lut[256][10]; // 256个值的10字节表示
static uint8_t lut_initialized = 0;  // 用uint8_t代替bool


// 初始化查找表
static void init_bit_lut(void) {
    if (lut_initialized) return;
    
    uint16_t value;
    for (value = 0; value < 256; value++) {
        uint8_t b[10] = {0};
        uint16_t patterns[8];
        uint8_t i, j;
        
        // 为每个bit生成对应的10位模式
        for (i = 0; i < 8; i++) {
            patterns[i] = (value & (1 << (7 - i))) ? BIT_1 : BIT_0;
        }
        
        // 将8个10位模式紧密打包到10个字节中
        uint16_t bit_pos = 0;
        for (i = 0; i < 8; i++) {
            // 将当前10位模式放入输出缓冲区
            for (j = 0; j < 10; j++) {
                if (patterns[i] & (1 << (9 - j))) {
                    b[bit_pos / 8] |= (1 << (7 - (bit_pos % 8)));
                }
                bit_pos++;
            }
        }
        
        for (i = 0; i < 10; i++) {
            bit_lut[value][i] = b[i];
        }
    }
    
    lut_initialized = 1;
}


void WS2812_init(void) {
    #if Debug
    printf("WS2812_init\n");
    #endif
    
    // 初始化查找表
    init_bit_lut();
    
    // 清空缓冲区
    memset(colors1, 0, sizeof(colors1));
    memset(colors2, 0, sizeof(colors2));
    
    // 初始化灯
    
    for(uint8_t i = 0; i < WS2812_NUM1; i++){
        WS2812_set_color_brightness(1, i, 0x000000, 1);
    }
    for(uint8_t i = 0; i < WS2812_NUM2; i++){
        WS2812_set_color_brightness(2, i, 0x000000, 1);
    }
    
    WS2812_display(1);
    WS2812_display(2);
}


void WS2812_set_color(uint8_t whichline, uint16_t index, uint32_t color) {
    if (!lut_initialized) {
        init_bit_lut();
    }
    
    if (whichline == 1) {
        if (index >= WS2812_NUM1) return;
    } else if (whichline == 2) {
        if (index >= WS2812_NUM2) return;
    } else {
        return; // 无效的线路
    }
    
    // 提取GRB分量（注意WS2812使用GRB顺序）
    uint8_t g = (color >> 8) & 0xff;
    uint8_t r = (color >> 16) & 0xff;
    uint8_t b = (color >> 0) & 0xff;
    
    uint8_t *buffer = (whichline == 1) ? colors1 : colors2;
    uint16_t offset = index * 30;
    
    // 使用查找表填充
    {
        uint8_t *g_ptr = bit_lut[g];
        uint8_t *r_ptr = bit_lut[r];
        uint8_t *b_ptr = bit_lut[b];
        uint8_t *dst = &buffer[offset];
        
        uint8_t i;
        for (i = 0; i < 10; i++) {
            dst[i] = g_ptr[i];
            dst[i + 10] = r_ptr[i];
            dst[i + 20] = b_ptr[i];
        }
    }
}


void WS2812_set_color_brightness(uint8_t whichline, uint16_t index, uint32_t color, uint8_t value) {
    if (whichline == 1) {
        if (index >= WS2812_NUM1) return;
    } else if (whichline == 2) {
        if (index >= WS2812_NUM2) return;
    } else {
        return; // 无效的线路
    }
    
    if (value > 100) value = 100;
    
    // 提取原始RGB分量
    uint8_t r = (color >> 16) & 0xff;
    uint8_t g = (color >> 8) & 0xff;
    uint8_t b = (color >> 0) & 0xff;
    
    // 亮度调整
    if (value == 0) {
        r = g = b = 0;
    } else if (value < 100) {
        r = (uint16_t)(r) * value / 100;
        g = (uint16_t)(g) * value / 100;
        b = (uint16_t)(b) * value / 100;
    }
    
    // 设置调整后的颜色
    uint32_t adjusted_color = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
    WS2812_set_color(whichline, index, adjusted_color);
}


void WS2812_display(uint8_t whichline) {
    uint32_t data_len;
    
    if (whichline == 1) {
        if (WS2812_NUM1 == 0) return; // 没有LED
        
        data_len = 30 * WS2812_NUM1;
        SPI0_dma_tx_start(colors1, data_len, 1);
    } 
    else if (whichline == 2) {
        if (WS2812_NUM2 == 0) return; // 没有LED
        
        data_len = 30 * WS2812_NUM2;
        SPI1_dma_tx_start(colors2, data_len, 1);
    }
}


void WS2812_set_range_color(uint8_t whichline, uint16_t start, uint16_t end, uint32_t color) {
    uint16_t i;
    for (i = start; i <= end; i++) {
        WS2812_set_color(whichline, i, color);
    }
}


void WS2812_clear(uint8_t whichline) {
    if (whichline == 1) {
        memset(colors1, 0, sizeof(colors1));
    } else if (whichline == 2) {
        memset(colors2, 0, sizeof(colors2));
    }
}