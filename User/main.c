#include "main.h"

TaskHandle_t xStartTask_Handle;
TaskHandle_t xTaskConfirmKey_Handle;
TaskHandle_t xTaskKey_Handle;
TaskHandle_t xTaskOLED_Handle;
TaskHandle_t xTaskGameProgress_Handle;
TaskHandle_t xTaskWS2812_Handle_1;
TaskHandle_t xTaskWS2812_Handle_2;
TaskHandle_t xTaskTime_Handle;

// 按键事件组
EventGroupHandle_t KEY_eventgroup_handle;
// 屏幕显示事件组
EventGroupHandle_t OLED_eventgroup_handle;

void USART0_on_recv(uint8_t* data, uint32_t len){} 

void sys_init(){ // 初始化
    DelayInit();
    USART0_init();
    
    SPI_init();
    I2C_init();
    
    OLED_Init();
    bsp_keys_init();
    WS2812_init();
}

u8g2_t u8g2;
extern uint8_t u8g2_gpio_and_delay_gd32( U8X8_UNUSED u8x8_t *u8x8, 
/* u8g2初始↑ */                         U8X8_UNUSED uint8_t msg, 
                                         U8X8_UNUSED uint8_t arg_int, 
                                         U8X8_UNUSED void *arg_ptr    );


void vTask_init(uint8_t *pvParameters){
    // 系统初始化
    sys_init();
    printf("Init Complete!\n");
    
    // 创建按键事件组
    KEY_eventgroup_handle = xEventGroupCreate();
    // 创建屏幕显示事件组
    OLED_eventgroup_handle = xEventGroupCreate();
    
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_sw_i2c, u8g2_gpio_and_delay_gd32);
    u8g2_InitDisplay(&u8g2);     // send init sequence to the display, display is in sleep mode after this,
    u8g2_SetPowerSave(&u8g2, 0); // wake up display
    
    // 进入临界区, 所有任务会被禁止切换，中断被屏蔽 -----------------------------
    taskENTER_CRITICAL();
    {   // 创建任务
        xTaskCreate( vTaskTime,         "vTaskTime",           512,   NULL,   6,   &xTaskTime_Handle );
        xTaskCreate( vTaskOLED,         "vTaskOLED",           512,   NULL,   5,   &xTaskOLED_Handle);
        xTaskCreate( vTaskKey,          "vTaskKey",            512,   NULL,   4,   &xTaskKey_Handle);
        xTaskCreate( vTaskGameProgress, "vTaskGameProgress",   512,   NULL,   3,   &xTaskGameProgress_Handle);
        xTaskCreate( vTaskWS2812_1,     "vTaskWS2812_1",       512,   NULL,   2,   &xTaskWS2812_Handle_1);
        xTaskCreate( vTaskWS2812_2,     "vTaskWS2812_2",       512,   NULL,   1,   &xTaskWS2812_Handle_2);
    }  
    // 退出临界区, 所有任务允许切换 ---------------------------------------------    
    taskEXIT_CRITICAL();
    // 开始让屏幕刷新一次
    xEventGroupSetBits(OLED_eventgroup_handle, REFRESH_OLED);
    // 3. 删除当前任务 鲁棒性(健壮性)
    vTaskDelete(NULL); // xStartTask_Handle    
}


int main(void) {
    // 全局优先级分配规则：4抢占[0,15], 0响应
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
    
    // 创建第一个任务(初始化外设, 创建其他任务)
    BaseType_t rst = xTaskCreate(
        (TaskFunction_t)             vTask_init,    // 任务函数的指针
        (const char *)              "Task_init",    // 任务名称, 最大长度取决于 configMAX_TASK_NAME_LEN
        (uint16_t)                          128,    // 任务栈大小, 单位: Word字(32位 4字节)  128 x 4字节, 任务越复杂所需空间越大
        (uint8_t *)                        NULL,    // 任务函数参数, 通常使用NULL
        (UBaseType_t)                         1,    // 任务的优先级, 数值越大, 优先级越高, 最大值为configMAX_PRIORITIES-1
        (TaskHandle_t * )    &xStartTask_Handle     // 任务句柄, 用于在需要时操作该任务
    ); 
    // rst: pdPASS创建成功，pdFAIL失败 (通常栈空间不足时,才会失败)

    // 开启任务调度
    vTaskStartScheduler();
    while(1);  
}