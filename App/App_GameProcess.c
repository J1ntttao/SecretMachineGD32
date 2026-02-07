#include "App.h"

//xTaskCreate( vTaskGameProgress, "vTaskGameProgress",   512, NULL, 3, &xTaskGameProgress_Handle);
uint8_t g_currentState = InitState;

#define WS2812MAIN_RE_W() WS2812_set_color_brightness(1, g_currentPos, g_currentColor, 1)
#define WS2812OTHER_GRN() WS2812_set_color_brightness(2, g_currentPos, 0xFFFFFF, 1)

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
    #if Debug
    print_user_guess();     
    #endif
    WS2812OTHER_GRN();                                             
}            


                                                 
uint8_t g_cur_page = 0;      // 当前页码（0开始）
uint8_t g_total_page = 1;    // 总页数

int8_t Change_Line(){         
    // 没步数了 就结束
    if(g_cur_steps == 1) return -1;    
    if(g_currentLine == 6 && g_cur_page != 0){
        // 灭掉第7行的灯
        WS2812_set_color_brightness(1, 30, 0x000000, 1);
        WS2812_set_color_brightness(1, 31, 0x000000, 1);
        WS2812_set_color_brightness(1, 32, 0x000000, 1);
        WS2812_set_color_brightness(1, 33, 0x000000, 1);
        WS2812_set_color_brightness(1, 34, 0x000000, 1);
        
        WS2812_set_color_brightness(2, 30, 0x000000, 1);
        WS2812_set_color_brightness(2, 31, 0x000000, 1);
        WS2812_set_color_brightness(2, 32, 0x000000, 1);
        WS2812_set_color_brightness(2, 33, 0x000000, 1);
        WS2812_set_color_brightness(2, 34, 0x000000, 1);
        
        WS2812_set_color_brightness(2, 59, 0x000000, 1);
        WS2812_set_color_brightness(2, 60, 0x000000, 1);
        WS2812_set_color_brightness(2, 61, 0x000000, 1);
        WS2812_set_color_brightness(2, 62, 0x000000, 1);
        
        WS2812_display(1);
        WS2812_display(2);        
    } 
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
        
        // 换页这里应该等第二次进来再刷新掉的,因为用户按下后应该停留显示这次的结果
        // ↑ 二编：留下最后一行得了 但是要考虑啥时候刷新掉 -> g_currentLine = 6的时候 用户按下检测按钮 让g_currentLine为7的灯重置
        // 换页操作，清除原先页面
            // 初始化灯 留下最后一行
           
            WS2812_set_color_brightness(1, 0, 0xFF0000, 1);
            for(uint8_t i = 1; i < WS2812_NUM1 - 7 - 5; i++){
                WS2812_set_color_brightness(1, i, 0x000000, 1);
            }
            
            WS2812_set_color_brightness(2, 0, 0xFFFFFF, 1);
            for(uint8_t i = 1; i < 30; i++){
                WS2812_set_color_brightness(2, i, 0x000000, 1);
            }
            
            for(uint8_t i = 35; i < WS2812_NUM2 - 4; i++){
                WS2812_set_color_brightness(2, i, 0x000000, 1);
            }            
            
          WS2812_display(1);
          WS2812_display(2);
        
        memset(g_user_guess, 0, sizeof(g_user_guess));
        memset(g_user_guess, 1, 1);
               
        /*  int8_t*/ g_currentPos = 0; 
        /* uint8_t*/ g_currentLine = 1; 
        /*uint32_t*/ g_currentColor = 0xFF0000;                               
        /*  int8_t*/ currentColorIndex = 1;   
        /*  int8_t*/ g_color_i = 0;
    }
    
    g_cur_steps--;
    // 四灯模式 五灯模式 共享这个页数切换函数：
    if(g_cur_light == 5){
        Change_Pos(0); 
    }else if(g_cur_light == 4){
        old_Change_Pos(0); 
    }
                                                    
    return 0;                                                      
}                                     

                                                              
/********************************************************** 颜色类 *****/   
                                                               
uint32_t COLORS[7] = {0xFF0000,  // 红                         
                      0xFFA500,  // 橙                         
                      0xFFFF00,  // 黄                         
                      0x00FF00,  // 绿                         
                      0x00FFFF,  // 青                         
                      0x0000FF,  // 蓝                         
                      0xFF00FF}; // 紫                         
                                                               
uint32_t g_currentColor = 0xFF0000;                            
  int8_t g_color_i = 0;                                                               
void Toggle_Color(int8_t dir) {                                
                                           
    // 钳位 0123456                                            
    g_color_i = (g_color_i + dir) % 7;                                         
    if(g_color_i < 0) g_color_i += 7;                                          
    currentColorIndex = g_color_i+1;                                
    g_user_guess[g_currentPos] = currentColorIndex;         
    #if Debug
    print_user_guess();     
    #endif                                    
    g_currentColor = COLORS[g_color_i];                             
    WS2812MAIN_RE_W();                                      
}                                                           
                                         

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
    /*uint32_t*/ g_using_time = 0;
    /*  int8_t*/ g_color_i = 0;   
    /*  int8_t*/ g_is_timeout = 0;    
}

void GameTimeout(){ // g_cur_time == 0
    // 为了解决倒计时bug 如果已经步数耗尽结束了就返回
    if(g_currentState != StartState){
        return;
    }
    // 关闭倒计时
    g_cd_enable = pdFALSE;
    g_is_timeout = 1;
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
    // 关闭倒计时
    g_cd_enable = pdFALSE;
    // 切换状态
    g_currentState = SuccessState; 
    
    // 刷新屏幕
    xEventGroupSetBits(OLED_eventgroup_handle, REFRESH_OLED);  
    vTaskDelay(1000);
    g_currentState = KEYInitState;
}


void vTaskGameProgress(){

    while(1){ 
        // printf("vTaskGameProgress\n");
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
