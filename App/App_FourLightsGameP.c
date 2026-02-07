#include "App.h"
/*

    总结一下共有的变量：
    uint8_t g_currentState = InitState; 状态机当前状态
     int8_t g_currentPos = 0;           当前位置信息 初始0                                                        
    uint8_t g_currentLine = 1;          当前行数 换行用 初始1
    uint8_t g_cur_page = 0;             当前所在页 分页用 初始0  
    uint8_t g_total_page = 1;           通过计算玩家输入的步数得的总需页数
    uint32_t COLORS[7] ...              七种颜色数组
    uint32_t g_currentColor = 0xFF0000; 当前格子颜色                           
      int8_t g_color_i = 0;             当前颜色索引
    
    共享的函数：
    // 用于切换位置或者换行时，清除原来的位置指示灯
    void Clear_NowPos_WS2812(int8_t p){
        WS2812_set_color_brightness(2, p, 0x000000, 1);
    }
    
*/

// 4灯模式 正确答案     
uint8_t o_normal_ans[4 * 5] = { 0,0,0,0,0,  // 0 放置
                                7,4,2,1,0,  // 1
                                2,6,3,1,0,  // 2
                                4,2,1,7,0,  // 3
};

uint8_t o_hard_ans[4 * 5] = { 0,0,0,0,0,
                              2,3,7,1,0,
                              2,5,7,3,0,
                              1,4,5,6,0,
}; 

uint8_t o_experts_ans[4 * 5] = { 0,0,0,0,0,
                                 4,5,7,3,0,
                                 3,5,2,6,0,
                                 5,2,3,4,0,
};  

uint8_t o_cur_ans[5] = {0}; // 4灯模式当前关卡答案存储值

void o_Tip_WS2812Refresh(){
    if(g_cur_Diff == Normal){
        memcpy(o_cur_ans, &o_normal_ans[g_cur_level * 5], 5 * sizeof(uint8_t));           
    }else if(g_cur_Diff == Hard){    
        memcpy(o_cur_ans, &o_hard_ans[g_cur_level * 5], 5 * sizeof(uint8_t));
    }else{   
        memcpy(o_cur_ans, &o_experts_ans[g_cur_level * 5], 5 * sizeof(uint8_t));
        #if Debug
        printf("Experts不会有Tips\n"); 
        #endif
        WS2812_set_color_brightness(1, 35, COLORS[0], 1);
        WS2812_set_color_brightness(1, 36, COLORS[1], 1);
        WS2812_set_color_brightness(1, 37, COLORS[2], 1);
        WS2812_set_color_brightness(1, 38, COLORS[3], 1);
        WS2812_set_color_brightness(1, 39, COLORS[4], 1);
        WS2812_set_color_brightness(1, 40, COLORS[5], 1);
        WS2812_set_color_brightness(1, 41, COLORS[6], 1);
        
        if(g_cur_level == 4){
            trng_generate_unique_5_shuffle(o_cur_ans);
        }
        #if Debug
        print_array(o_cur_ans, 5);
        #endif         
        return; 
    }
    
    // 以上本质上是填充当前关卡的答案到cur_ans数组 那么如果你是随机，也就是说关卡值是4
    if(g_cur_level == 4){
        // 就直接获取一个随机值，填入cur_ans数组，不用管哪个关
        trng_generate_unique_5_shuffle(o_cur_ans);
    }
    #if Debug
    print_array(o_cur_ans, 5);
    #endif
    for(uint8_t i = 0;i < 4; i++){     // 把灯提示出来  
        for(uint8_t j = 1;j < 8; j++){ // 1234567 
            if(o_cur_ans[i] == j){
                WS2812_set_color_brightness(1, (34+j), COLORS[j-1], 1);
            }
        }
    }
}


void old_Level_init(){
    // 初始化灯
    {   
        WS2812_set_color_brightness(1, 0, 0xFF0000, 1);
        for(uint8_t i = 1; i < WS2812_NUM1-7; i++){
            WS2812_set_color_brightness(1, i, 0x000000, 1);
        }
        o_Tip_WS2812Refresh();
        WS2812_set_color_brightness(2, 0, 0xFFFFFF, 1);
        for(uint8_t i = 1; i < WS2812_NUM2; i++){
            WS2812_set_color_brightness(2, i, 0x000000, 1);
        }
    }
    WS2812_display(1);
    WS2812_display(2);
}

////////////////////////////////////////////////////////////////////////////////
/*
    0   1   2   3    (4)      3536 3738
    5   6   7   8    (9)      3940 4142
    10  11  12  13  (14)      4344 4546
    15  16  17  18  (19)      4748 4950
    20  21  22  23  (24)      5152 5354
    25  26  27  28  (29)      5556 5758
    30  31  32  33  (34)      5960 6162
    35 36 37 38 39 40 41

*/

//在五灯模式定义的：
// uint32_t g_currentColor = 0xFF0000;                            
//   int8_t g_color_i = 0;

void old_Toggle_Color(int8_t dir){
    // 钳位 0123456                                            
    g_color_i = (g_color_i + dir) % 7;                                         
    if(g_color_i < 0) g_color_i += 7;
    
    currentColorIndex = g_color_i+1;                                
    g_user_guess[g_currentPos] = currentColorIndex;         
    // print_user_guess();     
    
    g_currentColor = COLORS[g_color_i];                             
    WS2812_set_color_brightness(1, g_currentPos, g_currentColor, 1); 
    #if Debug
    print_user_guess();     
    #endif
    xEventGroupSetBits(KEY_eventgroup_handle, TOGGLE_COLOR | CHECK_COLOR);    
}


void old_Change_Pos(int8_t dir){
    // 进行当前行的钳位 g_currentLine 初始 1
    uint8_t c_l_max = (g_currentLine * 5) - 2;      //  0  1  2  3
    uint8_t c_l_min = c_l_max - 3;                  //  5  6  7  8   
                                                    // 10 11 12 13                                                            
    g_currentPos += dir;                                                   
    if(g_currentPos < c_l_min) g_currentPos = c_l_min;                             
    if(g_currentPos > c_l_max) g_currentPos = c_l_max;
    // ↑进行位置的更改后
    // 更改相应灯的颜色位置：
    WS2812_set_color_brightness(1, g_currentPos, g_currentColor, 1);      
 
    g_user_guess[g_currentPos] = currentColorIndex;                
    #if Debug
    print_user_guess();     
    #endif                                    
    // 设置移动后位置的灯的颜色为白色
    WS2812_set_color_brightness(2, g_currentPos, 0xFFFFFF, 1);                                            
  
}
                           


////////////////////////////////////////////////////////////////////////////////

int8_t o_Normal_Checked(){
    // 确认按下后，先判断是否答对
    uint8_t correctCnt = 0;
    int8_t toCompareLine = g_currentLine - 1;
    // g_cur_level 1 2 3
    // 循环判断每一个位置是不是等于
    for(uint8_t i = 0; i < 4; i++){
        if(g_user_guess[(5 * toCompareLine) + i] == o_cur_ans[i]){
            WS2812_set_color_brightness(2, (5 * toCompareLine) + i, 0x00FF00, 1);
            correctCnt++;           
        }
    }
    #if Debug
    printf("correntCnt %d\n",(int)correctCnt);
    #endif
    if(correctCnt == 4){ return 1; }    
    return 0;
}

int8_t o_Hard_Checked(){
    uint8_t correctCnt = 0;
    int8_t toCompareLine = g_currentLine - 1;   // g_currentLine 从1开始
    // g_cur_level 1 2 3
    // 35 36 37 38
    // 39 40 41 42
    // ...
    // 59 60 61 62
    // 循环判断每一个位置是不是等于
    for(uint8_t i = 0; i < 4; i++){
        if(g_user_guess[(5 * toCompareLine) + i] == o_cur_ans[i]){
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
    }
    #if Debug
    printf("correntCnt %d\n",(int)correctCnt);
    #endif
    if(correctCnt == 4){
        return 1;
    }
    return 0;
}

// 定义布尔值宏
#define FALSE 0
#define TRUE  1

uint8_t DisplayGuessResult_4Lights(uint8_t pos, uint8_t color, uint8_t line);

int8_t o_Experts_Checked(){
    int8_t toCompareLine = g_currentLine - 1;   // g_currentLine 从1开始
    uint8_t correct_position = 0;               // 对：颜色和位置都正确
    uint8_t correct_color_wrong_position = 0;   // 有：颜色正确但位置错误
    
    // 记录哪些位置已经被匹配过
    uint8_t ans_matched[4] = {FALSE};    // 答案有4个位置
    uint8_t guess_matched[4] = {FALSE};  // 猜测有4个位置
    
    // 第一步：先找出对（颜色和位置都正确）
    for(uint8_t i = 0; i < 4; i++){  // 只检查4个位置
        if(g_user_guess[(5 * toCompareLine) + i] == o_cur_ans[i]){ 
            correct_position++;
            ans_matched[i] = TRUE;
            guess_matched[i] = TRUE;
        }
    }
    
    // 第二步：找出有（颜色正确但位置错误）
    for(uint8_t i = 0; i < 4; i++){
        if(guess_matched[i]) continue;
        
        for(uint8_t j = 0; j < 4; j++){
            if(ans_matched[j]) continue;
            
            if(g_user_guess[(5 * toCompareLine) + i] == o_cur_ans[j]){
                correct_color_wrong_position++;
                ans_matched[j] = TRUE;
                break;
            }
        }
    }
    #if Debug
    // 打印结果
    printf("\r\n4灯模式 - 对：%u，有：%u\n", correct_position, correct_color_wrong_position);
    #endif
    // 显示4灯版本的反馈
    return DisplayGuessResult_4Lights(correct_position, correct_color_wrong_position, g_currentLine);
}

uint8_t DisplayGuessResult_4Lights(uint8_t pos, uint8_t color, uint8_t line){
    uint8_t feedback_start = 35 - 4 + (line * 4);
    // 胜利情况
    if(pos == 4){
        for(uint8_t i = 0; i < 4; i++){
            WS2812_set_color_brightness(2, feedback_start + i, 0x00FF00, 1);
        }
        return 1;
    }
    
    // 普通反馈
    for(uint8_t i = 0; i < 4; i++){
        if(i < pos){
            // 绿灯：位置和颜色都对
            WS2812_set_color_brightness(2, feedback_start + i, 0x00FF00, 1);
        }else if(i < pos + color){
            // 橙灯：颜色对但位置错
            WS2812_set_color_brightness(2, feedback_start + i, 0xFFA500, 1);
        }else{
            // 红色：完全不匹配,
            WS2812_set_color_brightness(2, feedback_start + i, 0xFF0000, 1);
        }
    }
    return 0;
}