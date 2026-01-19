#ifndef __BSP_WS2812_H__
#define __BSP_WS2812_H__

#include "gd32f4xx.h"

#ifndef   u8
#define   u8    uint8_t
#endif  
    
#ifndef   u16
#define   u16   uint16_t
#endif  
    
#ifndef   u32
#define   u32   uint32_t
#endif

enum
{   
   OTHERLINE = 1,
    MAINLINE = 2,
};

#define   MAIN_NUM      42
#define   OTHER_NUM     63

#define   SPI_BYTE_LEN  30

//  if(whichline==1){memcpy(other_data[index], tmp, SPI_BYTE_LEN);}
//  else if(whichline==2){memcpy(main_data[index], tmp, SPI_BYTE_LEN);}
//  else return;

void bsp_ws2812_init();

// 8bit(±£¡Ù) + 8bit(RED) + 8bit(Green) + 8bit(Blue)
void bsp_ws2812_set_color(uint8_t whichline, uint16_t index, uint32_t color);

void bsp_ws2812_display_OTHER();
void bsp_ws2812_display_MAIN();

void bsp_ws2812_HSVtoRGB(double h, double s, double v, u8* r, u8* g, u8* b); 
void bsp_ws2812_HSVtoRGB24(double h, double s, double v, u32* rgb);

#endif