#include "App.h" 
#include "Delay.h"

// xTaskCreate( vTaskWS2812,       "vTaskWS2812",         512, NULL, 2, &xTaskWS2812_Handle);

// MAIN_NUM   28+ 7=35颗灯 SPI1=>PB15
// OTHER_NUM  28+28=56颗灯 SPI0=>PA07

//result = xSemaphoreGive(sema_WS2812Re_handler);

void vTaskWS2812_1(){
    BaseType_t result;
    for(uint8_t i = 0;i<MAIN_NUM;i++){
         bsp_ws2812_set_color(MAINLINE, i, 0xA00AFF);
    }
    while(1){  
        //result = xSemaphoreTake(sema_WS2812Re_handler, portMAX_DELAY);

        bsp_ws2812_display_MAIN();
                
        vTaskDelay(500);
    }
}

void vTaskWS2812_2(){
    BaseType_t result;
    for(uint8_t i = 0;i<OTHER_NUM;i++){
         bsp_ws2812_set_color(OTHERLINE, i, 0xFF00AA);
    }
    while(1){
        //result = xSemaphoreTake(sema_WS2812Re_handler, portMAX_DELAY);

        bsp_ws2812_display_OTHER();
        vTaskDelay(1000);
    }
}
#if 0

// 红橙黄绿青蓝紫
// 0 1 2 3 4 5 6 

/*
        Lv1: TIP显示本关出现的颜色	 
        POS会显示情况指示灯（位置相关）
        ACK不显示
*/
const u8 Lv1Code[4][4] = {2,5,3,1,     // 答案
                          3,1,4,2, 
                          5,0,1,3,
                          0,3,2,4};

void Lv1LEDinit(u8 Lv){
    u8 i,j;
    u8 init_values[28] = {1,6,3,2,  /* 0  1  2  3  系统给出的例子 [只用改这里] */
                          0,7,7,7,  /* 4  5  6  7 */       
                          7,7,7,7,  /* 8  9 10 11 */
                          7,7,7,7,  /* 12 13 14 15 */
                          7,7,7,7,  /* 16 17 18 19 */
                          7,7,7,7,  /* 20 21 22 23 */
                          7,7,7,7}; /* 24 25 26 27 */
    
    /* 使用memcpy复制数据 */
    memcpy(g_color_array, init_values, sizeof(init_values));
                          
		g_current_line = 0;		// 判断函数需要用到行数进行判断 先初始化到第0行											
		g_current_color = 0;	// 初始化颜色
		g_current_pos = 4;		// 初始化当前位置									
		
		for(i=0;i<4;i++){	    // 把系统给的例子写入ANS前四个				
            WS2812_WriteColor2Array(ANS, i, init_values[i]);		
        }
        
        // 判断本回合出现的颜色 TIP初始化
        for(i=0;i<4;i++){
            for(j=0;j<7;j++){
                if(Lv1Code[Lv - 1][i]==j){
                    WS2812_WriteColor2Array(TIP, j, j);
                }
            }
        }
		
        // 系统给的颜色 的 位置判断
        GuessColorLv1(Lv - 1);
        g_current_line = 1;		// 初始化行数\round
	
		
		WS2812_WriteColor2Array(ANS, 4, 0); // 玩家初始位置初始化
        WS2812_WriteColor2Array(POS, 4, 0);

}

/*
        Lv2: TIP显示本关出现的颜色
        POS不显示
        ACK会显示情况指示灯（位置无关，仅表示个数）
*/
const u8 Lv2Code[4][4] = {6,0,2,1,     // 答案
                          4,3,2,5, 
                          2,4,5,0,
                          1,6,3,2};

void Lv2LEDinit(u8 Lv){//Lv 1234
    u8 i,j;
    u8 init_values[28] = {5,1,4,6,  /* 0  1  2  3  系统给出的例子 [只用改这里] */
                          0,7,7,7,  /* 4  5  6  7 */       
                          7,7,7,7,  /* 8  9 10 11 */
                          7,7,7,7,  /* 12 13 14 15 */
                          7,7,7,7,  /* 16 17 18 19 */
                          7,7,7,7,  /* 20 21 22 23 */
                          7,7,7,7}; /* 24 25 26 27 */
    
    /* 使用memcpy复制数据 */
    memcpy(g_color_array, init_values, sizeof(init_values));
                          
		g_current_line = 0;		// 判断函数需要用到行数进行判断 先初始化到第0行											
		g_current_color = 0;	// 初始化颜色
		g_current_pos = 4;		// 初始化当前位置									
		
		for(i=0;i<4;i++){	    // 把系统给的例子写入ANS前四个				
            WS2812_WriteColor2Array(ANS, i, init_values[i]);		
        }
        
        // 判断本回合出现的颜色 TIP初始化
        for(i=0;i<4;i++){
            for(j=0;j<7;j++){
                if(Lv2Code[Lv - 1][i]==j){
                    WS2812_WriteColor2Array(TIP, j, j);
                }
            }
        }
		
        // 系统给的颜色 的 个数判断
        // Lv2不会显示POS
        GuessColorLv2(Lv - 1);
        g_current_line = 1;		// 初始化行数\round
	
		
		WS2812_WriteColor2Array(ANS, 4, 0); // 玩家初始位置初始化
        WS2812_WriteColor2Array(POS, 4, 0);
        
}

/*
        Lv3: TIP不显示
        POS不显示
        ACK会显示情况指示灯（位置无关，仅表示个数）
*/
const u8 Lv3Code[4][4] = {6,2,1,4,     // 答案
                          5,4,0,1, 
                          2,3,5,6, 
                          3,6,2,0};

void Lv3LEDinit(u8 Lv){
    u8 i;
    u8 init_values[28] = {1,5,3,2,  /* 0  1  2  3  系统给出的例子 [只用改这里] */
                          0,7,7,7,  /* 4  5  6  7 */       
                          7,7,7,7,  /* 8  9 10 11 */
                          7,7,7,7,  /* 12 13 14 15 */
                          7,7,7,7,  /* 16 17 18 19 */
                          7,7,7,7,  /* 20 21 22 23 */
                          7,7,7,7}; /* 24 25 26 27 */
    
    /* 使用memcpy复制数据 */
    memcpy(g_color_array, init_values, sizeof(init_values));
                          
		g_current_line = 0;		// 判断函数需要用到行数进行判断 先初始化到第0行											
		g_current_color = 0;	// 初始化颜色
		g_current_pos = 4;		// 初始化当前位置									
		
		for(i=0;i<4;i++){	    // 把系统给的例子写入ANS前四个				
            WS2812_WriteColor2Array(ANS, i, init_values[i]);		
        }
        
        // Lv3不显示 TIP
		
        // 系统给的颜色 的 个数判断
        GuessColorLv3(Lv - 1);
        g_current_line = 1;		// 初始化行数\round
		
		WS2812_WriteColor2Array(ANS, 4, 0); // 玩家初始位置初始化
        WS2812_WriteColor2Array(POS, 4, 0);
        
}
#endif
