#include "App.h"

/*
     int8_t g_user_guess[5 * 8] = {1,0,0,0,0,  // 0
                                   0,0,0,0,0,  // 1
                                   0,0,0,0,0,  // 2
                                   0,0,0,0,0,  // 3
                                   0,0,0,0,0,  // 4
                                   0,0,0,0,0,  // 5
                                   0,0,0,0,0,  // 6
                                   0,0,0,0,0}; // 7
     无0 红1 橙2 黄3 绿4 青5 蓝6 紫7
      g_currentSteps
   WS2812_set_color_brightness(1, g_currentPos, g_currentColor, 1)
   WS2812_set_color_brightness(2, g_currentPos, 0x00FF00, 1)
*/

uint8_t normal_ans[4 * 5] = {0,0,0,0,0,
                             3,4,7,1,2,
                             1,6,4,7,5,
                             6,3,2,4,7,
                            }; // 正确答案

uint8_t hard_ans[4 * 5] = {0,0,0,0,0,
                           1,2,3,4,5,
                           2,3,4,5,6,
                           3,4,5,6,7,
                          }; 

uint8_t experts_ans[4 * 5] = {0,0,0,0,0,
                              1,2,3,4,5,
                              2,3,4,5,6,
                              3,4,5,6,7,
                             };  

uint8_t cur_ans[5] = {0};

void Tip_WS2812Refresh(){
    if(g_cur_Diff == Normal){
        
        memcpy(cur_ans, &normal_ans[g_cur_level * 5], 5 * sizeof(uint8_t));    
        
    }else if(g_cur_Diff == Hard){
        
        memcpy(cur_ans, &hard_ans[g_cur_level * 5], 5 * sizeof(uint8_t));
        
    }else{ 
        printf("Experts不会有Tips\n"); 
        return; 
    }
    
    for(uint8_t i = 0;i < 5; i++){     // 把灯提示出来  
        for(uint8_t j = 1;j < 8; j++){ // 1234567 
            if(cur_ans[i] == j){
                WS2812_set_color_brightness(1, (34+j), COLORS[j-1], 1);
            }
        }
    }
}

void Normal_init(){
    // 初始化灯
    {   
        WS2812_set_color_brightness(1, 0, 0xFF0000, 1);
        for(uint8_t i = 1; i < WS2812_NUM1-7; i++){
            WS2812_set_color_brightness(1, i, 0x000000, 1);
        }
        Tip_WS2812Refresh();
        WS2812_set_color_brightness(2, 0, 0xFFFFFF, 1);
        for(uint8_t i = 1; i < WS2812_NUM2; i++){
            WS2812_set_color_brightness(2, i, 0x000000, 1);
        }
    }
    WS2812_display(1);
    WS2812_display(2);
}


int8_t Normal_Checked(){
    // 确认按下后，先判断是否答对
    uint8_t correctCnt = 0;
    int8_t toCompareLine = g_currentLine - 1;
    // g_cur_level 1 2 3
    
    // 循环判断每一个位置是不是等于
    for(uint8_t i = 0; i < 5; i++){
        if(g_user_guess[(5 * toCompareLine) + i] == cur_ans[i]){
            WS2812_set_color_brightness(2, (5 * toCompareLine) + i, 0x00FF00, 1);
            correctCnt++;
        }     
    }
    
    printf("correntCnt %d\n",(int)correctCnt);
    if(correctCnt == 5){
        return 1;
    }
    return 0;
    
}

void Hard_init(){
     // 初始化灯
    Normal_init();
//    {   
//        WS2812_set_color_brightness(1, 0, 0xFF0000, 1);
//        for(uint8_t i = 1; i < WS2812_NUM1-7; i++){
//            WS2812_set_color_brightness(1, i, 0x000000, 1);
//        }
//        Tip_WS2812Refresh();
//        WS2812_set_color_brightness(2, 0, 0xFFFFFF, 1);
//        for(uint8_t i = 1; i < WS2812_NUM2; i++){
//            WS2812_set_color_brightness(2, i, 0x000000, 1);
//        }
//    }
//    WS2812_display(1);
//    WS2812_display(2);
}


int8_t Hard_Checked(){    // 确认按下后，先判断是否答对
    uint8_t correctCnt = 0;
    int8_t toCompareLine = g_currentLine - 1;   // g_currentLine 从1开始
    // g_cur_level 1 2 3
    // 35 36 37 38
    // 39 40 41 42
    // ...
    // 59 60 61 62
    // 循环判断每一个位置是不是等于
    for(uint8_t i = 0; i < 5; i++){
        if(g_user_guess[(5 * toCompareLine) + i] == cur_ans[i]){
            correctCnt++;
        }
    }
    
    if(correctCnt == 0){
        // WS2812_set_color_brightness(2, (5 * toCompareLine) + i, 0x00FF00, 1);
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 4, 0xFF0000, 1);
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 3, 0xFF0000, 1);
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 2, 0xFF0000, 1);
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 1, 0xFF0000, 1);
    }else if(correctCnt == 1){
        // WS2812_set_color_brightness(2, (5 * toCompareLine) + i, 0x00FF00, 1);
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 4, 0x00FF00, 1);
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 3, 0xFF0000, 1);
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 2, 0xFF0000, 1);
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 1, 0xFF0000, 1);
    }else if(correctCnt == 2){
        // WS2812_set_color_brightness(2, (5 * toCompareLine) + i, 0x00FF00, 1);
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 4, 0x00FF00, 1);
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 3, 0x00FF00, 1);
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 2, 0xFF0000, 1);
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 1, 0xFF0000, 1);
    }else if(correctCnt == 3){
        // WS2812_set_color_brightness(2, (5 * toCompareLine) + i, 0x00FF00, 1);
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 4, 0x00FF00, 1);
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 3, 0x00FF00, 1);
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 2, 0x00FF00, 1);
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 1, 0xFF0000, 1);
    }else if(correctCnt == 4){
        // WS2812_set_color_brightness(2, (5 * toCompareLine) + i, 0x00FF00, 1);
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 4, 0x00FF00, 1);
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 3, 0x00FF00, 1);
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 2, 0x00FF00, 1);
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 1, 0x00FF00, 1);
    }else if(correctCnt == 5){
        // WS2812_set_color_brightness(2, (5 * toCompareLine) + i, 0x00FF00, 1);
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 4, 0x00FF00, 6);
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 3, 0x00FF00, 6);
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 2, 0x00FF00, 6);
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 1, 0x00FF00, 6);
    }
    
    printf("correntCnt %d\n",(int)correctCnt);
    if(correctCnt == 5){
        return 1;
    }
    return 0;
}

void Experts_init(){}
int8_t Experts_Checked(){return 0;}
