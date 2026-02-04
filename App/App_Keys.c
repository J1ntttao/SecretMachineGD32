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
                if(g_lv_light == 5){ g_lv_light = 4;}
                else if(g_lv_light == 4){ g_lv_light = 5;}
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
        g_cur_light = g_lv_light;
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
        // 这次检查没答对就换行呗
        if(g_isSuccess == 0){
            // 改变行
            g_isSuccess = Change_Line();   
        }

        vTaskDelay(11); // 个人感觉这里延时一点能让g_isSuccess能够及时检查到
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

void Keys_on_keydown(uint8_t key){ //printf("KEY%d \n",(int)key);
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
