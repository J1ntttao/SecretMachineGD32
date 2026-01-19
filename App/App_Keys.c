#include "App.h"

//xTaskCreate( vTaskKey,          "vTaskKey",            512, NULL, 5, &xTaskKey_Handle);

void vTaskKey(){
    
    while(1){
        //printf("vTaskKey\n");
        bsp_keys_scan();
        vTaskDelay(10);    
    }
}

BaseType_t result;
void Keys_on_keydown(uint8_t key){
    printf("KEY%d \n",(int)key);
    result = xSemaphoreGive(sema_WS2812Re_handler);
}













#if 0

//  0  1  2  3  //系统开始给出的参考
//  4  5  6  7
//  8  9 10 11
// 12 13 14 15
// 16 17 18 19
// 20 21 22 23
// 24 25 26 27

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