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


// 正确答案
uint8_t normal_ans[4 * 5] = { 0,0,0,0,0,
                              3,2,7,1,4,
                              1,6,4,7,5,
                              6,3,2,4,7 }; 

uint8_t hard_ans[4 * 5] = { 0,0,0,0,0,
                            5,2,3,4,1,
                            2,1,7,5,6,
                            1,4,5,7,3 }; 

uint8_t experts_ans[4 * 5] = { 0,0,0,0,0,
                               4,2,6,7,1,
                               1,5,3,6,7,
                               3,2,1,4,6 };  

uint8_t cur_ans[5] = {0};


void Tip_WS2812Refresh(){
    if(g_cur_Diff == Normal){
        
        memcpy(cur_ans, &normal_ans[g_cur_level * 5], 5 * sizeof(uint8_t));    
        
    }else if(g_cur_Diff == Hard){
        
        memcpy(cur_ans, &hard_ans[g_cur_level * 5], 5 * sizeof(uint8_t));
        
    }else{ 
        
        memcpy(cur_ans, &experts_ans[g_cur_level * 5], 5 * sizeof(uint8_t));
        printf("Experts不会有Tips\n"); 
        WS2812_set_color_brightness(1, 35, COLORS[0], 1);
        WS2812_set_color_brightness(1, 36, COLORS[1], 1);
        WS2812_set_color_brightness(1, 37, COLORS[2], 1);
        WS2812_set_color_brightness(1, 38, COLORS[3], 1);
        WS2812_set_color_brightness(1, 39, COLORS[4], 1);
        WS2812_set_color_brightness(1, 40, COLORS[5], 1);
        WS2812_set_color_brightness(1, 41, COLORS[6], 1);
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


void Level_init(){
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
    if(correctCnt == 5){ return 1; }
    return 0;
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
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 4, 0x00FF00, 5);
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 3, 0x00FF00, 5);
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 2, 0x00FF00, 5);
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 1, 0x00FF00, 5);
    }
    
    printf("correntCnt %d\n",(int)correctCnt);
    if(correctCnt == 5){
        return 1;
    }
    return 0;
}


uint8_t DisplayResultSimple(uint8_t pos, uint8_t color, uint8_t line);

// 定义布尔值宏
#define FALSE 0
#define TRUE  1

int8_t Experts_Checked(){
    int8_t toCompareLine = g_currentLine - 1;   // g_currentLine 从1开始
    uint8_t correct_position = 0;               // 颜色和位置都正确的个数
    uint8_t correct_color_wrong_position = 0;   // 颜色正确但位置错误的个数
    uint8_t ans_matched[5] = {FALSE};     // FALSE: 未匹配, TRUE: 已匹配
    uint8_t guess_matched[5] = {FALSE};   // FALSE: 未匹配, TRUE: 已匹配
    
    // 第一步：先找出颜色和位置都正确的
    for(uint8_t i = 0; i < 5; i++){
        if(g_user_guess[(5 * toCompareLine) + i] == cur_ans[i]){
            correct_position++;
            ans_matched[i] = TRUE;
            guess_matched[i] = TRUE;
        }
    }
    
    // 第二步：找出颜色正确但位置错误的
    for(uint8_t i = 0; i < 5; i++){
        if(guess_matched[i]) continue;
        
        for(uint8_t j = 0; j < 5; j++){
            if(ans_matched[j]) continue;
            
            if(g_user_guess[(5 * toCompareLine) + i] == cur_ans[j]){
                correct_color_wrong_position++;
                ans_matched[j] = TRUE;
                break;
            }
        }
    }
    
    // 打印结果
    printf("位置正确：%u，颜色对位置错：%u\n", correct_position, correct_color_wrong_position);
    
    // 调用显示函数
    return DisplayResultSimple(correct_position, correct_color_wrong_position, g_currentLine);
}

uint8_t DisplayResultSimple(uint8_t pos, uint8_t color, uint8_t line){
    if(pos == 5){
        for(uint8_t i = 0; i < 4; i++){
            WS2812_set_color_brightness(2, 35 + (line * 4) - 4 + i, 0x00FF00, 5);
        }
        return 1;
    }
    
    uint8_t red_real = 5 - pos - color;
    uint8_t green_show = (pos > 4) ? 4 : pos;
    uint8_t orange_show = (color > (4 - green_show)) ? (4 - green_show) : color;
    uint8_t red_show = 4 - green_show - orange_show;
    
    uint8_t idx = 0;
    for(uint8_t i = 0; i < green_show; i++, idx++)
        WS2812_set_color_brightness(2, 35 + (line * 4) - 4 + idx, 0x00FF00, 1);
    for(uint8_t i = 0; i < orange_show; i++, idx++)
        WS2812_set_color_brightness(2, 35 + (line * 4) - 4 + idx, 0xFFA500, (orange_show == color) ? 1 : 5);
    for(uint8_t i = 0; i < red_show; i++, idx++)
        WS2812_set_color_brightness(2, 35 + (line * 4) - 4 + idx, 0xFF0000, (red_show == red_real) ? 1 : 5);
    
    return 0;
}