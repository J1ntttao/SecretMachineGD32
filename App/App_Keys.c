#include "App.h"

//xTaskCreate( vTaskKey,          "vTaskKey",            512, NULL, 5, &xTaskKey_Handle);

void vTaskKey(){
    while(1){
        bsp_keys_scan();
        vTaskDelay(pdMS_TO_TICKS(20));    
    }
}

static void sToggle_Color(int8_t dir){
    // 向dir依次切换7种颜色
    Toggle_Color(dir); 
    // 显示颜色
    xEventGroupSetBits(KEY_eventgroup_handle, TOGGLE_COLOR | CHECK_COLOR);    
}

static void sChange_Pos(int8_t dir){
    Clear_NowPos_WS2812(g_currentPos);
    // 往dir走一格
    Change_Pos(dir);
    // 显示颜色
    xEventGroupSetBits(KEY_eventgroup_handle, TOGGLE_COLOR | CHECK_COLOR);
}


/** 按键排布 **********************************************************
 *         	             KEY1                                        *
 *               KEY3	 		 KEY2       KEY5                     *
 *      	             KEY4                                        *
 *********************************************************************/  
static void KeyUpDown_down(int8_t dir){
        if(g_currentState == InitState){
            State_Change(); 
            xEventGroupSetBits(OLED_eventgroup_handle, REFRESH_OLED);
            return;
        }
        if(g_currentState == SwDiffState){
            g_cur_Diff -= dir;
            if(g_cur_Diff < 0) g_cur_Diff = 2;
            if(g_cur_Diff > 2) g_cur_Diff = 0;
            xEventGroupSetBits(OLED_eventgroup_handle, REFRESH_OLED);
            return;   
        }
        if(g_currentState == SwLevelState){
            g_cur_level -= dir;
            if(g_cur_level < 1) g_cur_level = 3;
            if(g_cur_level > 3) g_cur_level = 1;
            xEventGroupSetBits(OLED_eventgroup_handle, REFRESH_OLED);
            return;       
        }
        if(g_currentState == SetConfigState){   // SetConfigState
            g_cur_config_sw -= dir;
            if(g_cur_config_sw < 1) g_cur_config_sw = 3;
            if(g_cur_config_sw > 3) g_cur_config_sw = 1;
            xEventGroupSetBits(OLED_eventgroup_handle, REFRESH_OLED);
            return;       
        }
        if(g_currentState == StartState){
            sToggle_Color(dir);
            return;    
        }
}

static void KeyLeftRight_down(int8_t dir){
        if(g_currentState == InitState){
            State_Change(); 
            xEventGroupSetBits(OLED_eventgroup_handle, REFRESH_OLED);
            return;
        }
        if(g_currentState == SwDiffState){
            // none
            return;   
        }
        if(g_currentState == SwLevelState){
            // none
            return;       
        }
        if(g_currentState == SetConfigState){   // SetConfigState
            if(g_cur_config_sw == 1){
                g_lv_time += (dir*10);
                if(g_lv_time < 0) g_lv_time = 0;
            }
            if(g_cur_config_sw == 2){
                g_lv_steps += dir;
                if(g_lv_steps < 2) g_lv_steps = 1;
            }  
            if(g_cur_config_sw == 3){
                g_lv_light += dir;
            }
            xEventGroupSetBits(OLED_eventgroup_handle, REFRESH_OLED);
            return;           
        }
        if(g_currentState == StartState){
            sChange_Pos(dir);
            return;    
        }
}





void Level_init(){
    if(g_cur_Diff == 0){
        Normal_init();
    }else if(g_cur_Diff == 1){
        Hard_init();
    }else if(g_cur_Diff == 2){
        Experts_init();
    }       
}

int8_t Level_Checked(){
    if(g_cur_Diff == 0){
        return Normal_Checked();
    }else if(g_cur_Diff == 1){
        return Hard_Checked();
    }else if(g_cur_Diff == 2){
        return Experts_Checked();
    }
    return 0;
}

int8_t g_isSuccess = 0; // -1 失败     0 正常     1 成功
static void Key5_down(){
    if(g_currentState == InitState){
        State_Change(); 
        xEventGroupSetBits(OLED_eventgroup_handle, REFRESH_OLED);
        return;
    }
    if(g_currentState == SwDiffState){
        State_Change(); 
        xEventGroupSetBits(OLED_eventgroup_handle, REFRESH_OLED);
        return;   
    }
    if(g_currentState == SwLevelState){
        State_Change(); 
        xEventGroupSetBits(OLED_eventgroup_handle, REFRESH_OLED);
        return;       
    }
    if(g_currentState == SetConfigState){
        State_Change(); 
        
         // 步数同步
        g_cur_steps = g_lv_steps;       
        
        // 计算总页数
        if(g_lv_steps <= 7){
            g_total_page = 1;
        }else{
            g_total_page = (g_lv_steps + 6) / 7;  // 向上取整
        }
    
        // 开启倒计时
        g_cur_time = g_lv_time;
        if(g_cur_time == 0) g_cur_time = 2147483647;
        g_cd_enable = pdTRUE;
        
        // 倒计时内会1s调用一次 xEventGroupSetBits(OLED_eventgroup_handle, REFRESH_OLED);
        Level_init();
        
        return;           
    }
    if(g_currentState == StartState){
        // 清除当前位置灯
        Clear_NowPos_WS2812(g_currentPos);
        // 检测答对否
        g_isSuccess = Level_Checked();
        
        if(g_isSuccess == 0){
            // 改变行
            g_isSuccess = Change_Line();
        }
        
        vTaskDelay(11);
        // 刷新屏幕与显示颜色
        xEventGroupSetBits(OLED_eventgroup_handle, REFRESH_OLED);
        xEventGroupSetBits(KEY_eventgroup_handle, TOGGLE_COLOR | CHECK_COLOR);
        return;    
    }
    if(g_currentState == KEYInitState){
        // 状态切换
        State_Change(); 
        // 游戏初始化
        GameClear_init();
        
        // 切换屏幕显示
        xEventGroupSetBits(OLED_eventgroup_handle, REFRESH_OLED);
        // 显示颜色
        xEventGroupSetBits(KEY_eventgroup_handle, TOGGLE_COLOR | CHECK_COLOR);
        return;    
    }
}

void Keys_on_keydown(uint8_t key){
    //printf("KEY%d \n",(int)key);
    switch(key){
        case 1:     KeyUpDown_down(1);   break;
        case 2:  KeyLeftRight_down(1);   break;
        case 3: KeyLeftRight_down(-1);   break;
        case 4:    KeyUpDown_down(-1);   break;
        case 5:           Key5_down();   break;        
        default:
            printf("CLEAR KEY \n");
            xEventGroupClearBits(KEY_eventgroup_handle, TOGGLE_COLOR | CHECK_COLOR);
            break;
    }
}
            







#if 0



extern u32 sys_ms;
char g_current_color = 0;	
  u8 g_current_pos	 = 4;	
  u8 g_current_line  = 1;


// 需要限定范围 每一个round只让你在同一行（4个灯范围内移动）
#define current_move_min  (g_current_line * 4)// 4 8 12 16 20 24
#define current_move_max  ((g_current_line * 4) + 3)


void Keys_on_keydown(u8 key){
    u32 cur_ms = 0;
//    DeviceStandBy
    if(DeviceState == DeviceDefault){	// 初始状态       
            Device_Default2LevelSW(key);
            return;
    }	
    
    if(DeviceState == DeviceLevelSW){   // 选择难度状态
            Device_LevelSW2DeviceLvSwitch(key);
            return;
    }		

    if(DeviceState == DeviceLvSwitch){  // 选择关卡状态
            Device_LvSwitch2Start(key);
            return;
    }	
    
    if(DeviceState == DeviceStart){	    // 游玩状态
            if(key == 0){						
                    g_current_pos--;
                    if(g_current_pos < current_move_min)  g_current_pos = current_move_min;
                    if(g_current_pos > current_move_max)  g_current_pos = current_move_max;
                    
                    g_color_array[g_current_pos] = g_current_color;
                    WS2812_WriteColor2Array(ANS, g_current_pos, g_current_color);
                    WS2812_WriteColor2Array(POS, g_current_pos+1, 7);
                    WS2812_WriteColor2Array(POS, g_current_pos, g_current_color);
            }
            if(key == 2){
                    g_current_pos++;
                    if(g_current_pos < current_move_min)  g_current_pos = current_move_min;
                    if(g_current_pos > current_move_max)  g_current_pos = current_move_max;
                
                    g_color_array[g_current_pos] = g_current_color;
                    WS2812_WriteColor2Array(ANS, g_current_pos, g_current_color);
                    WS2812_WriteColor2Array(POS, g_current_pos-1, 7);
                    WS2812_WriteColor2Array(POS, g_current_pos, g_current_color);
            }
            
            if(key == 1){
                    g_current_color++;
                    if(g_current_color<0) g_current_color = 6;
                    if(g_current_color>6) g_current_color = 0;
                
                    g_color_array[g_current_pos] = g_current_color;
                    WS2812_WriteColor2Array(ANS, g_current_pos, g_current_color);
                    WS2812_WriteColor2Array(POS, g_current_pos, g_current_color);
            }
            if(key == 3){
                    g_current_color--;
                    if(g_current_color<0) g_current_color = 6;
                    if(g_current_color>6) g_current_color = 0;
                
                    g_color_array[g_current_pos] = g_current_color;
                    WS2812_WriteColor2Array(ANS, g_current_pos, g_current_color);
                    WS2812_WriteColor2Array(POS, g_current_pos, g_current_color);
            }
            
            if(key == 4){
                    WS2812_WriteColor2Array(POS, g_current_pos, 7); // 因为要换行，所以清除当前POS的位置颜色
                    if(LevelSwitch == 1)	GuessColorLv1(Lv1Lvsw - 1);	    // 判断 关卡1 （这个过程中会给根据ANS给POS数组写颜色（没有发送））
                    else if(LevelSwitch == 2)	GuessColorLv2(Lv2Lvsw - 1);
                    else if(LevelSwitch == 3)	GuessColorLv3(Lv3Lvsw - 1);
                    // 关卡胜利状态
                    if(DeviceState == DeviceVictory){ 
                        OLED_Clear();
                        App_OLED_refresh();
                        
                        DeviceState = DeviceDefault;
                        
                        // 这里先上锁TASK 硬卡死等待一会（2秒左右）
                        Task_Lock();
                        cur_ms = sys_ms;
                        while((sys_ms - cur_ms)<1000);
                        Task_Unlock();
                        
                        return;
                    }		
                    // 关卡失败状态（6次没猜出来）
                    if(g_current_line == 6){
                        //printf("6次已满\n");
                    
                        DeviceState = DeviceDefeat;
                        OLED_Clear();
                        App_OLED_refresh();
                        DeviceState = DeviceDefault;
                    
                        return;	// 超过6次
                    }
                
                    g_current_line++;	// 让游戏的行数+1	下面这行是修改移动临界值
                    if(g_current_pos < current_move_min)  g_current_pos = current_move_min;
                    
                    g_color_array[g_current_pos] = g_current_color;// 把当前颜色写入当前位置 修改移动临界值之后位置切换了，所以要写入一次
                    WS2812_WriteColor2Array(ANS, g_current_pos, g_current_color);// 把当前颜色写入ANS POS数组
                    WS2812_WriteColor2Array(POS, g_current_pos, g_current_color);
                    
            }
            WS2812_RefreshLEDColor(0);
            WS2812_RefreshLEDColor(1);
    }
}

//void Keys_on_keyup(u8 key){
//    printf("KEY[%d] UP\n",(int)key);
//}

#endif