#include "App.h"

//xTaskCreate( vTaskGameProgress, "vTaskGameProgress",   512, NULL, 3, &xTaskGameProgress_Handle);

void vTaskGameProgress(){

    while(1){
        //printf("vTaskGameProgress\n");
        vTaskDelay(500);
    }
}

#if 0
        
//按键排布
//   	  KEY1
// KEY0	 		 KEY2       KEY4
//	      KEY3

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
