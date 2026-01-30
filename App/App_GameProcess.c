#include "App.h"

//xTaskCreate( vTaskGameProgress, "vTaskGameProgress",   512, NULL, 3, &xTaskGameProgress_Handle);
uint8_t g_currentState = InitState;

#define WS2812MAIN_RE_W() WS2812_set_color_brightness(1, g_currentPos, g_currentColor, 1)
#define WS2812OTHER_GRN() WS2812_set_color_brightness(2, g_currentPos, 0x00FF00, 1)
void Clear_NowPos_WS2812(int8_t p){
     WS2812_set_color_brightness(2, p, 0x000000, 1);
}


/** 按键排布 * * * * * * * * * * * * * * * * **
  *       	   KEY1                          *                       
  *    KEY3	 		   KEY2       KEY5       *  
  *            KEY4                          *  
  ** * * * * * * * * * * * * * * * * * * * * **/
 int8_t g_user_guess[5 * 8] = {1,0,0,0,0,  // 0
                               0,0,0,0,0,  // 1
                               0,0,0,0,0,  // 2
                               0,0,0,0,0,  // 3
                               0,0,0,0,0,  // 4
                               0,0,0,0,0,  // 5
                               0,0,0,0,0,  // 6
                               0,0,0,0,0}; // 7
// 无0 红1 橙2 黄3 绿4 青5 蓝6 紫7
void print_user_guess(){
    for(int8_t i = 0; i < 8; i++) {
        int8_t row_start = i * 5; 
        printf("[%d] %d,%d,%d,%d,%d\n",
               (int)i,
               (int)g_user_guess[row_start],
               (int)g_user_guess[row_start + 1],
               (int)g_user_guess[row_start + 2],
               (int)g_user_guess[row_start + 3],
               (int)g_user_guess[row_start + 4]);
    }
    printf("==============\n");
}
                               

/********************************************************** 位置相关 ******/ 
 int8_t g_currentPos = 0;                                          
 int8_t currentColorIndex = 1;                                     
uint8_t g_currentLine = 1;                                         
void Change_Pos(int8_t dir) {                                      
  #define _pos g_currentPos                                        
    uint8_t c_l_max = (g_currentLine * 5) - 1; // 1*5 5   01234    
    uint8_t c_l_min = c_l_max - 4;             // 2*5 10  56789    
                                                                                                        
    _pos += dir;                                                   
    if(_pos < c_l_min) _pos = c_l_min;                             
    if(_pos > c_l_max) _pos = c_l_max;                             
    WS2812MAIN_RE_W();                                             
    g_user_guess[g_currentPos] = currentColorIndex;                
    print_user_guess();                                            
    WS2812OTHER_GRN();                                             
}                                                                   
                                                 
uint8_t g_cur_page = 0;      // 当前页码（0开始）
uint8_t g_total_page = 1;    // 总页数
//g_lv_steps = 0;      // 总步骤数
//g_cur_steps = 0;     // 剩余步数        
int8_t Change_Line(){         
    // 没步数了 就结束
    if(g_cur_steps == 1) return -1;    
    
    // 当前行的话， 1234567 8的时候变为1 g_currentLine默认为 1
    g_currentLine = (g_currentLine % 8) + 1;
    
    if(g_currentLine == 8){ // 换页操作
        g_currentLine = 1;
        // 换页
        if(g_cur_page < g_total_page - 1){
            g_cur_page++;
        }else{
            // 已经是最后一页，回到首页
            g_cur_page = 0;
        }
        
        // 换页操作，清除原先页面所有
        // 刷新所有灯
        Level_init();
        memset(g_user_guess, 0, sizeof(g_user_guess));
        memset(g_user_guess, 1, 1);
        
        /*  int8_t*/ g_currentPos = 0; 
        /* uint8_t*/ g_currentLine = 1; 
        /*uint32_t*/ g_currentColor = 0xFF0000;                               
        /*  int8_t*/ currentColorIndex = 1;   

    }
    
    g_cur_steps--;    
    Change_Pos(0);                                                 
    return 0;                                                      
}                                     

                                                              
/********************************************************** 颜色类 *****/   
                                                                   /* */
uint32_t COLORS[7] = {0xFF0000,  // 红                            /* */
                      0xFFA500,  // 橙                           /* */
                      0xFFFF00,  // 黄                          /* */
                      0x00FF00,  // 绿                          /**/
                      0x00FFFF,  // 青                          /**/
                      0x0000FF,  // 蓝                          /**/
                      0xFF00FF}; // 紫                          /**/
                                                                /**/
uint32_t g_currentColor = 0xFF0000;                             /**/
                                                                /**/
void Toggle_Color(int8_t dir) {                                 /**/
    static int8_t i = 0;                                        /**/
    // 钳位 0123456                                             /**/
    i = (i + dir) % 7;                                          /**/
    if(i < 0) i += 7;                                           /**/
    currentColorIndex = i+1;                                    /**/
    g_user_guess[g_currentPos] = currentColorIndex;             /**/
    print_user_guess();                                         /**/
    g_currentColor = COLORS[i];                                 /**/
    WS2812MAIN_RE_W();                                          /**/
}                                                               /**/
                                                                /**/
/******************************************************************/
// 定义游戏结束变量
static BaseType_t isGameEnd = pdFALSE;  // 默认关闭

void GameClear_init(){
    // 关闭倒计时
    g_cd_enable = pdFALSE;
    // WS2812初始化
    WS2812_init();
    memset(g_user_guess, 0, sizeof(g_user_guess));
    memset(g_user_guess, 1, 1);
    /*  int8_t*/ g_currentPos = 0; 
    /* uint8_t*/ g_currentLine = 1; 
    /*uint32_t*/ g_currentColor = 0xFF0000;                               
    /*  int8_t*/ currentColorIndex = 1;   
    /*  int8_t*/ g_cur_steps = 0;    
    /* uint8_t*/ g_cur_page = 0;      // 当前页码（0开始）
    /* uint8_t*/ g_total_page = 1;    // 总页数
}

void GameTimeout(){ // g_cur_time == 0
    // 关闭倒计时
    g_cd_enable = pdFALSE;
    isGameEnd = pdTRUE; // 游戏结束标记 
}

void GameDefeat(){
    // 清除游戏结束标记
    isGameEnd = pdFALSE;
    // 切换状态
    g_currentState = DefeatState; 
    
    // 刷新屏幕
    xEventGroupSetBits(OLED_eventgroup_handle, REFRESH_OLED);  
    vTaskDelay(1000);
    g_currentState = KEYInitState;
}


void GameSuccess(){
    // 清除游戏结束标记
    isGameEnd = pdFALSE;
    // 切换状态
    g_currentState = SuccessState; 
    
    // 刷新屏幕
    xEventGroupSetBits(OLED_eventgroup_handle, REFRESH_OLED);  
    vTaskDelay(1000);
    g_currentState = KEYInitState;
}


void vTaskGameProgress(){

    while(1){ //printf("vTaskGameProgress\n");
        if(g_isSuccess == 1){
            g_isSuccess = 0;
            GameSuccess();
        }else if(g_isSuccess == -1){
            g_isSuccess = 0;
            GameDefeat();
        }
        
        if(isGameEnd){
            GameDefeat();
        }
        
        vTaskDelay(10);
    }
}

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

uint8_t cur_ans[5] = {0};
void Tip_WS2812Refresh(){
    memcpy(cur_ans, &normal_ans[g_cur_level * 5], 5 * sizeof(uint8_t));

    for(uint8_t i = 0;i < 5; i++){
        for(uint8_t j = 1;j < 8; j++){// 1234567
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
        WS2812_set_color_brightness(2, 0, 0x00FF00, 1);
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

void Hard_init(){}
int8_t Hard_Checked(){return 0;}

void Experts_init(){}
int8_t Experts_Checked(){return 0;}

#if 0
       

void Device_Default2LevelSW(u8 key){
		if(key == 4){
		//切换至下一个状态
                WS2812_initLED();   // 灭掉所有灯
                WS2812_initLED();   // 有点bug 灭两次    
				DeviceState = DeviceLevelSW;
				OLED_Clear();
				App_OLED_refresh();
		}
}
	

void Device_LevelSW2DeviceLvSwitch(u8 key){
        static char level = 1;
		if(key == 1){
		//BTN KEY1
				level--;
				if(level < 1) level = 3;
				if(level > 3) level = 1;
				LevelSwitch = level;
				App_OLED_refresh();
		}
		if(key == 3){
		//BTN KEY3
				level++;
				if(level < 1) level = 3;
				if(level > 3) level = 1;
				LevelSwitch = level;
				App_OLED_refresh();
		}
		if(key == 4){ // 玩家按下确认按钮 进入二级选择难度关卡
		//BTN KEY4
				DeviceState = DeviceLvSwitch;

//				if(level == 1){	
//						Lv1LEDinit();
//						WS2812_RefreshLEDColor(1);
//						WS2812_RefreshLEDColor(0);
//				}
//				else if(level == 2){
//						Lv2LEDinit(); 
//						WS2812_RefreshLEDColor(1);
//						WS2812_RefreshLEDColor(0);					
//				}
//				else if(level == 3){
//						Lv3LEDinit();
//						WS2812_RefreshLEDColor(1);
//						WS2812_RefreshLEDColor(0);						
//				}
            
                OLED_Clear();
				App_OLED_refresh();
		}
}
// 已选定关卡 进行初始化关卡

void Device_LvSwitch2Start(u8 key){
    if(key == 1){
    //BTN KEY1
        if(LevelSwitch == Level1){
            Lv1Lvsw--;
            if(Lv1Lvsw>4)  Lv1Lvsw = 1;
            if(Lv1Lvsw==0) Lv1Lvsw = 4;
            App_OLED_refresh();
        }
        else if(LevelSwitch == Level2){
            Lv2Lvsw--;
            if(Lv2Lvsw>4)  Lv2Lvsw = 1;
            if(Lv2Lvsw==0) Lv2Lvsw = 4;
            App_OLED_refresh();
        }      
        else if(LevelSwitch == Level3){
            Lv3Lvsw--;
            if(Lv3Lvsw>4)  Lv3Lvsw = 1;
            if(Lv3Lvsw==0) Lv3Lvsw = 4;
            App_OLED_refresh();
        }             
    }
    
    if(key == 3){
    //BTN KEY3
        if(LevelSwitch == Level1){
            Lv1Lvsw++;
            if(Lv1Lvsw>4)  Lv1Lvsw = 1;
            if(Lv1Lvsw==0) Lv1Lvsw = 4;
            App_OLED_refresh();
        }
        else if(LevelSwitch == Level2){
            Lv2Lvsw++;
            if(Lv2Lvsw>4)  Lv2Lvsw = 1;
            if(Lv2Lvsw==0) Lv2Lvsw = 4;
            App_OLED_refresh();
        }      
        else if(LevelSwitch == Level3){
            Lv3Lvsw++;
            if(Lv3Lvsw>4)  Lv3Lvsw = 1;
            if(Lv3Lvsw==0) Lv3Lvsw = 4;
            App_OLED_refresh();
        }             
    }
    
    if(key == 4){ 
    //BTN KEY4
        DeviceState = DeviceStart;
        if(LevelSwitch == Level1){
        // Lv1Lvsw 当前选择的关卡进行初始化
            Lv1LEDinit(Lv1Lvsw);
            WS2812_RefreshLEDColor(1);
            WS2812_RefreshLEDColor(0);
            App_OLED_refresh();            
        }    
        else if(LevelSwitch == Level2){
        // Lv2Lvsw 当前选择的关卡进行初始化
            Lv2LEDinit(Lv2Lvsw);
            WS2812_RefreshLEDColor(1);
            WS2812_RefreshLEDColor(0);
            App_OLED_refresh();            
        }    
        else if(LevelSwitch == Level3){
        // Lv3Lvsw 当前选择的关卡进行初始化
            Lv3LEDinit(Lv3Lvsw);
            WS2812_RefreshLEDColor(1);
            WS2812_RefreshLEDColor(0);
            App_OLED_refresh();            
        }         
    }
}

u8 g_color_array[28] = {7,7,7,7,	//  0  1  2  3
                        0,7,7,7,	//  4  5  6  7		
                        7,7,7,7,    //  8  9 10 11
                        7,7,7,7,    // 12 13 14 15
                        7,7,7,7,    // 16 17 18 19
                        7,7,7,7,    // 20 21 22 23
                        7,7,7,7};   // 24 25 26 27
/* 
	 情况：
										 
			有这个颜色并且位置对了 1 1		判断指示灯 绿色
										 
			有这个颜色但是位置不对 1 0  	橙色
			
			没这个颜色所以不谈位置 0 0		灭
*/

                        
                        
// 红橙黄绿青蓝紫
// 0 1 2 3 4 5 6 
                        
/*
        Lv1: TIP显示本关出现的颜色	 
        POS会显示情况指示灯（位置相关）
        ACK不显示
*/
                        
void GuessColorLv1(u8 Lv){ // 第一关，仅表示POS情况指示灯
    u8 i = 0;		
    u8 current_level_correctCnt = 0;
    for(i=0;i<4;i++){
        if(g_color_array[(g_current_line * 4) + i] == Lv1Code[Lv][i]){
            // 对的话 POS 写上绿色
            WS2812_WriteColor2Array(POS, (g_current_line * 4) + i, 3);
            current_level_correctCnt++;
        }
    }
    if(current_level_correctCnt == 4){
        DeviceState = DeviceVictory;
        WS2812_RefreshLEDColor(0);
        WS2812_RefreshLEDColor(1);
    }
}

/*
        Lv2: TIP显示本关出现的颜色
        POS不显示
        ACK会显示情况指示灯（位置无关，仅表示个数）
*/
void GuessColorLv2(u8 Lv){
    u8 i = 0;
    u8 current_level_correctCnt = 0;
    for(i=0;i<4;i++){
        if(g_color_array[(g_current_line * 4) + i] == Lv2Code[Lv][i]){
            current_level_correctCnt++;
        }
    }
        // 一个不对 
        if(current_level_correctCnt == 0){      
            for(i=0;i<4;i++){
                WS2812_WriteColor2Array(ACK,((g_current_line * 4)+i), 0);
            }
        }
        else if(current_level_correctCnt == 1){
            
            WS2812_WriteColor2Array(ACK,(g_current_line * 4), 3);
            for(i=0;i<3;i++){
                WS2812_WriteColor2Array(ACK,((g_current_line * 4)+1+i), 0);
            }
        }
        else if(current_level_correctCnt == 2){
            for(i=0;i<2;i++){
                WS2812_WriteColor2Array(ACK,((g_current_line * 4)+i), 3);
            }
            for(i=0;i<2;i++){
                WS2812_WriteColor2Array(ACK,((g_current_line * 4)+2+i), 0);
            }
        }
        else if(current_level_correctCnt == 3){
            
            for(i=0;i<3;i++){
                WS2812_WriteColor2Array(ACK,((g_current_line * 4)+i), 3);
            }         
        
            WS2812_WriteColor2Array(ACK,(g_current_line * 4)+3, 0);
        }        
        // 四个都对，结束
        else if(current_level_correctCnt == 4){ 
            
            for(i=0;i<4;i++){
                WS2812_WriteColor2Array(ACK,((g_current_line * 4)+i), 3);
            }
            DeviceState = DeviceVictory;
            WS2812_RefreshLEDColor(0);
            WS2812_RefreshLEDColor(1);
        }
}
    
/*        
        Lv3: TIP不显示
        POS不显示
        ACK会显示情况指示灯（位置无关，仅表示个数）
*/
void GuessColorLv3(u8 Lv) {
    u8 i, j;
    u8 correct_positions = 0;      /* 位置正确的数量 */
    u8 correct_colors = 0;         /* 颜色正确但位置不对的数量 */
    u8 answer_copy[4];             /* 答案副本用于标记 */
    u8 guess_copy[4];              /* 猜测副本用于标记 */
    u8 display_case;
    u8 green_count = 0;
    u8 orange_count = 0;
    u8 color;
    
    const char ACK_display_table[5][5] = {
        { 0,  1,  2,  3,  4},
        {-1,  5,  6,  7,  9},
        {-2, -6,  8,  9,  9},
        {-3, -7,  9,  9,  9},
        {-4,  9,  9,  9,  9}
    };
    
    /* 复制当前猜测和答案到临时数组 */
    for(i = 0; i < 4; i++) {
        guess_copy[i] = g_color_array[(g_current_line * 4) + i];
        answer_copy[i] = Lv3Code[Lv][i];
    }
    
    /* 第一步：检查位置完全正确的 */
    for(i = 0; i < 4; i++) {
        if(guess_copy[i] == answer_copy[i]) {
            correct_positions++;
            guess_copy[i] = 0xFF;     /* 标记为已匹配 */
            answer_copy[i] = 0xFF;    /* 标记为已使用 */
        }
    }
    
    /* 第二步：检查颜色正确但位置不对的 */
    for(i = 0; i < 4; i++) {
        /* 跳过已匹配的位置 */
        if(guess_copy[i] == 0xFF) {
            continue;
        }
        
        /* 在答案中寻找相同颜色 */
        for(j = 0; j < 4; j++) {
            /* 跳过已使用的答案位置 */
            if(answer_copy[j] == 0xFF) {
                continue;
            }
            
            /* 找到颜色相同但位置不同的 */
            if(guess_copy[i] == answer_copy[j]) {
                correct_colors++;
                guess_copy[i] = 0xFF;   /* 标记为已匹配 */
                answer_copy[j] = 0xFF;  /* 标记为已使用 */
                break;                   /* 找到匹配后跳出内层循环 */
            }
        }
    }
    
    /* 特殊处理全对情况 */
    if(correct_positions == 4) {
        for(i = 0; i < 4; i++) {
            WS2812_WriteColor2Array(ACK, (g_current_line * 4) + i, 3);
        }
        DeviceState = DeviceVictory;
        WS2812_RefreshLEDColor(0);
        WS2812_RefreshLEDColor(1);
        return;
    }
    
    /* 计算显示状态 */
    display_case = ACK_display_table[correct_positions][correct_colors];
    
    /* 设置LED颜色 */
    green_count = correct_positions;
    orange_count = correct_colors;
    
    for(i = 0; i < 4; i++) {
        if(i < green_count) {
            color = 3;  /* 绿色 - 位置正确 */
        } else if(i < green_count + orange_count) {
            color = 1;  /* 橙色 - 颜色正确但位置不对 */
        } else {
            color = 0;  /* 红色 - 不正确 */
        }
        WS2812_WriteColor2Array(ACK, (g_current_line * 4) + i, color);
    }
    
    /* 处理特殊情况9（重新初始化LED） */
    if(display_case == 9) {
        WS2812_initLED();
    }
    
    /* 调试输出 */
    #if 0
    printf("Level: %d, Line: %d\n", Lv, g_current_line);
    printf("Correct positions: %d\n", correct_positions);
    printf("Correct colors: %d\n", correct_colors);
    printf("Display case: %d\n", display_case);
    #endif
}
#endif
