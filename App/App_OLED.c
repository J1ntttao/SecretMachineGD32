#include "App.h"

/* xTaskCreate( vTaskOLED,         "vTaskOLED",           512, NULL, 4, &xTaskOLED_Handle);
   状态机
    enum {
        InitState       =   0,
        SwDiffState     =   1,
        SwLevelState    =   2,
        SetConfigState  =   3,
        StartState      =   4,
        SuccessState    =   5,
        DefeatState     =   6
    };
*/

#define u8g2_SCL_RCU     RCU_GPIOB
#define u8g2_SCL_PORT    GPIOB
#define u8g2_SCL_PIN     GPIO_PIN_10

#define u8g2_SDA_RCU     RCU_GPIOB
#define u8g2_SDA_PORT    GPIOB
#define u8g2_SDA_PIN     GPIO_PIN_11

uint8_t u8g2_gpio_and_delay_gd32(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
    switch(msg) {
    //Initialize SPI peripheral
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        /* HAL initialization contains all what we need so we can skip this part. */
        // skip
        break;

    //Function which implements a delay, arg_int contains the amount of ms
    case U8X8_MSG_DELAY_MILLI:
        delay_1ms(arg_int);

        break;
    //Function which delays 10us
    case U8X8_MSG_DELAY_10MICRO:
        delay_1us(10);
        break;
    //Function which delays 100ns
    case U8X8_MSG_DELAY_100NANO:
        __NOP();

        break;
    case U8X8_MSG_GPIO_I2C_CLOCK:

        if (arg_int) gpio_bit_write(u8g2_SCL_PORT, u8g2_SCL_PIN, SET);
        else gpio_bit_write(u8g2_SCL_PORT, u8g2_SCL_PIN, RESET);

        break;

    case U8X8_MSG_GPIO_I2C_DATA:
        if (arg_int) gpio_bit_write(u8g2_SDA_PORT, u8g2_SDA_PIN, SET);
        else gpio_bit_write(u8g2_SDA_PORT, u8g2_SDA_PIN, RESET);
        break;

    default:
        return 0; //A message was received which is not implemented, return 0 to indicate an error
    }

    return 1; // command processed successfully.
}


void State_Change(){
    //printf("State_Change:%d\n",(int)g_currentState);
    switch(g_currentState){
        case InitState:      g_currentState = SwDiffState;      break;
        case SwDiffState:    g_currentState = SwLevelState;     break;
        case SwLevelState:   g_currentState = SetConfigState;   break;
        case SetConfigState: g_currentState = StartState;       break;
        case KEYInitState:   g_currentState = InitState;        break;

        default: break;
    }  
}

static void InitState_Dis(u8g2_t *u8g2){        
    
    u8g2_ClearBuffer(u8g2);  // 清空缓冲区
    u8g2_SetFontMode(u8g2, 1); 
    u8g2_SetFontDirection(u8g2, 0); // 设置方向
    u8g2_SetFont(u8g2, u8g2_font_fub20_tf); // 设置字体
    u8g2_DrawStr(u8g2, 0, 24, "Secret");  // 设置x,y坐标及字体内容

    //修改为更小的字体
    u8g2_SetFont(u8g2, u8g2_font_9x18B_tf); //u8g2_font_8x13B_tf u8g2_font_inb16_mf u8g2_font_fub20_tf

    u8g2_DrawStr(u8g2, 58, 38,"Machine");
    
    u8g2_DrawHLine(u8g2, 2, 35, 47); // 水平线起点，x,y，长度
    u8g2_DrawHLine(u8g2, 2, 36, 47);
    // u8g2_DrawVLine(u8g2, 45, 32, 12);
    // u8g2_DrawVLine(u8g2, 46, 33, 12);

    u8g2_SetFont(u8g2, u8g2_font_6x10_tf);
    u8g2_DrawStr(u8g2, 1,54," v1.0     by J1ntttao");

    u8g2_SendBuffer(u8g2); // 上面步骤画完，一定要记得将数据发送到屏幕
}

/*
    enum {  // 关卡难度
        Normal  = 0,
        Hard    = 1,
        Experts = 2
    };
*/

int8_t g_cur_Diff = Normal;
static void SwDiffState_Dis(u8g2_t *u8g2){        

    u8g2_ClearBuffer(u8g2);  // 清空缓冲区
    u8g2_SetFontMode(u8g2, 1); 
    u8g2_SetFontDirection(u8g2, 0); // 设置方向
    u8g2_SetFont(u8g2, u8g2_font_9x18B_tf); // 设置字体
    u8g2_DrawStr(u8g2, 5, 12, "DIFFICULTY:");  // 设置x,y坐标及字体内容
    
    u8g2_DrawHLine(u8g2, 7, 16, 90); // 水平线起点，x,y，长度

    u8g2_SetFont(u8g2, u8g2_font_8x13B_tf); //u8g2_font_8x13B_tf u8g2_font_inb16_mf u8g2_font_fub20_tf
    uint8_t whichdiff = 30;
    if(g_cur_Diff == 0) whichdiff = 30;
    if(g_cur_Diff == 1) whichdiff = 45;
    if(g_cur_Diff == 2) whichdiff = 60;
    
    u8g2_DrawStr(u8g2, 10, whichdiff,">");
    
    u8g2_DrawStr(u8g2, 26, 30,"Normal");
    u8g2_DrawStr(u8g2, 26, 45,"Hard");
    u8g2_DrawStr(u8g2, 26, 60,"Experts");    

    u8g2_SendBuffer(u8g2); // 将数据发送到屏幕
}

uint8_t g_cur_level = 1;
static void SwLevelState_Dis(u8g2_t *u8g2){        

    u8g2_ClearBuffer(u8g2);  // 清空缓冲区
    u8g2_SetFontMode(u8g2, 1); 
    u8g2_SetFontDirection(u8g2, 0); // 设置方向
    u8g2_SetFont(u8g2, u8g2_font_9x18B_tf); // 设置字体
    u8g2_DrawStr(u8g2, 5, 12, "SWITCH LEVEL:");  // 设置x,y坐标及字体内容
    
    u8g2_DrawHLine(u8g2, 7, 16, 110); // 水平线起点，x,y，长度

    u8g2_SetFont(u8g2, u8g2_font_8x13B_tf); //u8g2_font_8x13B_tf u8g2_font_inb16_mf u8g2_font_fub20_tf
    uint8_t whichlv = 30;
    if(g_cur_level == 1) whichlv = 30;
    if(g_cur_level == 2) whichlv = 45;
    if(g_cur_level == 3) whichlv = 60;
    
    u8g2_DrawStr(u8g2, 10, whichlv,">");
    
    u8g2_DrawStr(u8g2, 26, 30,"Lv1 ");
    u8g2_DrawStr(u8g2, 26, 45,"Lv2 ");
    u8g2_DrawStr(u8g2, 26, 60,"Lv3 ");    

    u8g2_SendBuffer(u8g2); // 将数据发送到屏幕
}

uint8_t g_cur_config_sw = 1;

int32_t g_lv_time  = 30;
int8_t g_lv_steps = 7;
int8_t g_lv_light = 7;
char buff_time[20];
char buff_steps[20];
char buff_light[20];

static void SetConfigState_Dis(u8g2_t *u8g2){        
    u8g2_ClearBuffer(u8g2);  // 清空缓冲区
    u8g2_SetFontMode(u8g2, 1); 
    u8g2_SetFontDirection(u8g2, 0); // 设置方向
    u8g2_SetFont(u8g2, u8g2_font_9x18B_tf); // 设置字体
    u8g2_DrawStr(u8g2, 5, 12, "LEVEL CONFIG:");  // 设置x,y坐标及字体内容
    
    u8g2_DrawHLine(u8g2, 7, 16, 110); // 水平线起点，x,y，长度

    u8g2_SetFont(u8g2, u8g2_font_8x13B_tf); //u8g2_font_8x13B_tf u8g2_font_inb16_mf u8g2_font_fub20_tf
    uint8_t whichcfg = 30;
    if(g_cur_config_sw == 1) whichcfg = 30;
    if(g_cur_config_sw == 2) whichcfg = 45;
    if(g_cur_config_sw == 3) whichcfg = 60;
    
    u8g2_DrawStr(u8g2, 10, whichcfg,">");
    
    sprintf(buff_time, "Time(s) %d", g_lv_time);
    sprintf(buff_steps,"Steps   %d", g_lv_steps);
    sprintf(buff_light,"Light   %d", g_lv_light);
    u8g2_DrawStr(u8g2, 26, 30, buff_time);
    u8g2_DrawStr(u8g2, 26, 45, buff_steps);
    u8g2_DrawStr(u8g2, 26, 60, buff_light);    

    u8g2_SendBuffer(u8g2); // 将数据发送到屏幕
}



int32_t g_cur_time = 0;
int8_t g_cur_steps = 0;
int8_t g_cur_light = 0;
char gaming_time[20];
char gaming_steps[20];
char gaming_light[20];

char gaming_level[20];
static void StartState_Dis(u8g2_t *u8g2){  
    
    u8g2_ClearBuffer(u8g2);  // 清空缓冲区
    u8g2_SetFontMode(u8g2, 1); 
    u8g2_SetFontDirection(u8g2, 0); // 设置方向
    u8g2_SetFont(u8g2, u8g2_font_9x18B_tf); // 设置字体
    
    if(g_cur_Diff == 0){
        sprintf(gaming_level, "Normal Lv%d", g_cur_level);
    }else if(g_cur_Diff == 1){
        sprintf(gaming_level, "Hard Lv%d", g_cur_level);
    }else if(g_cur_Diff == 2){
        sprintf(gaming_level, "Experts Lv%d", g_cur_level);
    }
    
    u8g2_DrawStr(u8g2, 5, 12, gaming_level);  // 设置x,y坐标及字体内容

    u8g2_DrawHLine(u8g2, 7, 16, 65); // 水平线起点，x,y，长度

    u8g2_SetFont(u8g2, u8g2_font_8x13B_tf); //u8g2_font_8x13B_tf u8g2_font_inb16_mf u8g2_font_fub20_tf
    if(g_lv_time == 0){sprintf(gaming_time, "Time(s) -");}
    else{sprintf(gaming_time, "Time(s) %d",g_cur_time);}
    
    sprintf(gaming_steps,"Steps   %d",g_cur_steps);
    sprintf(gaming_light,"Light   %d",g_cur_light);
    u8g2_DrawStr(u8g2, 12, 30,  gaming_time);
    u8g2_DrawStr(u8g2, 12, 45, gaming_steps);
    u8g2_DrawStr(u8g2, 12, 60, gaming_light);    

    u8g2_SendBuffer(u8g2); // 将数据发送到屏幕
}

int32_t g_using_time = 0;
char gaming_using[30];
static void SuccessState_Dis(u8g2_t *u8g2){        
    
    u8g2_ClearBuffer(u8g2);  // 清空缓冲区
    u8g2_SetFontMode(u8g2, 1); 
    u8g2_SetFontDirection(u8g2, 0); // 设置方向
    
    u8g2_SetFont(u8g2, u8g2_font_fub20_tf); // 设置字体
    u8g2_DrawStr(u8g2,4, 20, "Success!");  // 设置x,y坐标及字体内容

    // u8g2_DrawHLine(u8g2, 7, 16, 65); // 水平线起点，x,y，长度

    u8g2_SetFont(u8g2, u8g2_font_8x13B_tf); //u8g2_font_8x13B_tf u8g2_font_inb16_mf u8g2_font_fub20_tf

    u8g2_DrawStr(u8g2, 8, 38,"Congratulation");
    
    u8g2_SetFont(u8g2, u8g2_font_6x10_tf);
    sprintf(gaming_using, "Using: %ds Steps: %d", g_using_time, g_lv_steps - g_cur_steps + 1);
    u8g2_DrawStr(u8g2, 8, 54, gaming_using);

    u8g2_SendBuffer(u8g2); // 将数据发送到屏幕
}

static void DefeatState_Dis(u8g2_t *u8g2){        
    
    u8g2_ClearBuffer(u8g2);  // 清空缓冲区
    u8g2_SetFontMode(u8g2, 1); 
    u8g2_SetFontDirection(u8g2, 0); // 设置方向
    
    u8g2_SetFont(u8g2, u8g2_font_fub14_tf); // 设置字体
    u8g2_DrawStr(u8g2, 4, 20, "Defeat!");  // 设置x,y坐标及字体内容
    
    //u8g2_DrawHLine(u8g2, 7, 16, 65); // 水平线起点，x,y，长度

    u8g2_SetFont(u8g2, u8g2_font_8x13B_tf); //u8g2_font_8x13B_tf u8g2_font_inb16_mf u8g2_font_fub20_tf

    u8g2_DrawStr(u8g2, 4, 50,"Plz try again...");

    u8g2_SendBuffer(u8g2); // 将数据发送到屏幕
}
    
void vTaskOLED(){
    OLED_DisPlay_On();
    EventBits_t uxBits;
    
    while(1){
        uxBits = xEventGroupWaitBits(
            OLED_eventgroup_handle, // 事件组句柄
            REFRESH_OLED,           // 关心的事件标志，多个用 |,+ 连在一起
            pdTRUE,                 // 退出时是否清理标记(置0), 只会清理关心的标记
            pdTRUE,                 // 所有关心的标志同时为1时，才解除阻塞
            pdMS_TO_TICKS(3000)     // 等待超时时间：3s
        ); 
        
        switch(g_currentState){
            case InitState      :    InitState_Dis(&u8g2);           break;
            case SwDiffState    :    SwDiffState_Dis(&u8g2);         break;
            case SwLevelState   :    SwLevelState_Dis(&u8g2);        break;
            case SetConfigState :    SetConfigState_Dis(&u8g2);      break;
            case StartState     :    StartState_Dis(&u8g2);          break;
            case SuccessState   :    SuccessState_Dis(&u8g2);        break;
            case DefeatState    :    DefeatState_Dis(&u8g2);         break;
            default             :    break;
        }  
      
        vTaskDelay(pdMS_TO_TICKS(100));    
    }
}
