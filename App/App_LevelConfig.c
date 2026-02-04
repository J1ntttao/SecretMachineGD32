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
    if(correctCnt == 5){ return 1; }
    return 0;
}


void Hard_init(){ // 初始化灯
    Normal_init();
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


void Experts_init(){ // 初始化灯
    Normal_init();
}

// 定义布尔值宏
#define FALSE 0
#define TRUE  1
int8_t Experts_Checked(){
     int8_t toCompareLine = g_currentLine - 1;   // g_currentLine 从1开始
    uint8_t correct_position = 0;               // 颜色和位置都正确的个数
    uint8_t correct_color_wrong_position = 0;   // 颜色正确但位置错误的个数
    uint8_t all_wrong = 0;                      // 颜色和位置都错误的个数
    
    // 记录哪些位置已经被匹配过（避免重复计数）
    uint8_t ans_matched[5] = {FALSE};     // FALSE: 未匹配, TRUE: 已匹配
    uint8_t guess_matched[5] = {FALSE};   // FALSE: 未匹配, TRUE: 已匹配
    
    // 第一步：先找出颜色和位置都正确的
    for(uint8_t i = 0; i < 5; i++){
        if(g_user_guess[(5 * toCompareLine) + i] == cur_ans[i]){
            correct_position++;
            ans_matched[i] = TRUE;    // 标记这个位置已匹配
            guess_matched[i] = TRUE;  // 标记这个位置已匹配
        }
    }
    
    // 第二步：找出颜色正确但位置错误的
    for(uint8_t i = 0; i < 5; i++){
        // 如果这个猜测位置已经匹配过了，跳过
        if(guess_matched[i] == TRUE){
            continue;
        }
        
        uint8_t found_color_match = FALSE;  // 标记是否找到颜色匹配
        
        // 遍历答案数组，寻找颜色相同但位置不同的
        for(uint8_t j = 0; j < 5; j++){
            // 如果答案位置已匹配，跳过
            if(ans_matched[j] == TRUE){
                continue;
            }
            
            // 如果找到相同颜色但位置不同
            if(g_user_guess[(5 * toCompareLine) + i] == cur_ans[j] && i != j){
                correct_color_wrong_position++;
                ans_matched[j] = TRUE;  // 标记这个答案位置已匹配
                found_color_match = TRUE;
                break;  // 找到匹配后跳出内层循环
            }
        }
        
        // 第三步：如果没有找到颜色匹配，就是完全错误
        if(found_color_match == FALSE){
            all_wrong++;
        }
    }
        
// 处理结果 ///////////////////////////////////////////////////////////////////////////////////////////////////    
    
    // 打印结果
    printf("\r\n位置正确：%u，颜色对位置错：%u\n", correct_position, correct_color_wrong_position);

    uint8_t temp = 0;
    uint8_t green_count = correct_position;
    uint8_t orange_count = correct_color_wrong_position;
    uint8_t total_wrong = 5 - green_count - orange_count;  // 全错的数量

    // 情况1: 5个全对（特殊处理）
    if(green_count == 5) {
        for(temp = 0; temp < 4; temp++) {
            WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 4 + temp, 0x00FF00, 5);
        }
        return 1;
    }

    // 步骤1: 计算实际能显示多少个灯
    uint8_t green_shown = green_count;      // 绿灯显示数量
    uint8_t orange_shown = orange_count;    // 橙灯显示数量
    uint8_t red_shown = total_wrong;        // 红灯显示数量

    // 总共只有4个灯位，可能需要截断
    if(green_shown + orange_shown + red_shown > 4) {
        // 需要截断，按优先级：绿色全显示，橙色次之，红色最后
        if(green_shown > 4) {
            green_shown = 4;
            orange_shown = 0;
            red_shown = 0;
        } 
        else if(green_shown + orange_shown > 4) {
            orange_shown = 4 - green_shown;
            red_shown = 0;
        } 
        else {
            red_shown = 4 - green_shown - orange_shown;
        }
    }

    // 步骤2: 计算亮度
    uint8_t green_brightness = 1;  // 绿色永远是亮度1

    // 橙色亮度：如果显示的少于实际的，说明被抢占，亮度5
    uint8_t orange_brightness = (orange_shown == orange_count) ? 1 : 5;

    // 红色亮度：如果显示的少于实际的，说明被抢占，亮度5
    uint8_t red_brightness = (red_shown == total_wrong) ? 1 : 5;

    printf("实际：绿=%u,橙=%u,红=%u | 显示：绿=%u(亮%u),橙=%u(亮%u),红=%u(亮%u)\n",
           green_count, orange_count, total_wrong,
           green_shown, green_brightness, orange_shown, orange_brightness, red_shown, red_brightness);

    // 步骤3: 显示LED
    uint8_t index = 0;

    // 显示绿灯
    for(temp = 0; temp < green_shown; temp++) {
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 4 + index, 0x00FF00, green_brightness);
        index++;
    }

    // 显示橙灯
    for(temp = 0; temp < orange_shown; temp++) {
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 4 + index, 0xFFA500, orange_brightness);
        index++;
    }

    // 显示红灯
    for(temp = 0; temp < red_shown; temp++) {
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 4 + index, 0xFF0000, red_brightness);
        index++;
    }

    // 如果还有空位（理论上不会，但安全处理）
    for(; index < 4; index++) {
        WS2812_set_color_brightness(2, 35 + (g_currentLine * 4) - 4 + index, 0x000000, 0);
    }

    return 0;
}
