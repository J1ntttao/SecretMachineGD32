#include "main.h"

TaskHandle_t xStartTask_Handle;
TaskHandle_t xTaskConfirmKey_Handle;
TaskHandle_t xTaskKey_Handle;
TaskHandle_t xTaskOLED_Handle;
TaskHandle_t xTaskGameProgress_Handle;
TaskHandle_t xTaskWS2812_Handle_1;
TaskHandle_t xTaskWS2812_Handle_2;

SemaphoreHandle_t 	sema_WS2812Re_handler;


void USART0_on_recv(uint8_t* data, uint32_t len){
} 


void sys_init(){
    DelayInit();
    USART0_init();
    
    SPI_init();
    I2C_init();
    
    OLED_Init();
    bsp_keys_init();
    bsp_ws2812_init();
}

void vTask_init(uint8_t *pvParameters){
    // 初始化外设 
    sys_init();
    printf("Init Complete!\n");
        
    // 进入临界区, 所有任务会被禁止切换，中断被屏蔽 ---------------------------
    taskENTER_CRITICAL();
    {
        // 创建任务      
        xTaskCreate( vTaskOLED,         "vTaskOLED",           512,   NULL,   5,   &xTaskOLED_Handle);
        xTaskCreate( vTaskKey,          "vTaskKey",            512,   NULL,   4,   &xTaskKey_Handle);
        xTaskCreate( vTaskGameProgress, "vTaskGameProgress",   512,   NULL,   3,   &xTaskGameProgress_Handle);
        xTaskCreate( vTaskWS2812_1,     "vTaskWS2812_1",       512,   NULL,   2,   &xTaskWS2812_Handle_1);
        xTaskCreate( vTaskWS2812_2,     "vTaskWS2812_2",       512,   NULL,   1,   &xTaskWS2812_Handle_2);
    }  
    // 退出临界区, 所有任务允许切换 -------------------------------------------    
    taskEXIT_CRITICAL();
    
    // 3. 删除当前任务 鲁棒性(健壮性)
    vTaskDelete(NULL); // xStartTask_Handle    
}

int main(void) {
    // 全局优先级分配规则：4抢占[0,15], 0响应
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
    
    sema_WS2812Re_handler = xSemaphoreCreateBinary();
    
    // 创建第一个任务(初始化外设, 创建其他任务)
    BaseType_t rst = xTaskCreate(
        (TaskFunction_t)             vTask_init,    // 任务函数的指针
        (const char *)              "Task_init",    // 任务名称, 最大长度取决于 configMAX_TASK_NAME_LEN
        (uint16_t)                          128,    // 任务栈大小, 单位: Word字(32位 4字节)  128 x 4字节, 任务越复杂所需空间越大
        (uint8_t *)                        NULL,    // 任务函数参数, 通常使用NULL
        (UBaseType_t)                         1,    // 任务的优先级, 数值越大, 优先级越高, 最大值为configMAX_PRIORITIES-1
        (TaskHandle_t * )    &xStartTask_Handle     // 任务句柄, 用于在需要时操作该任务
    ); 
    //  rst:  pdPASS创建成功，pdFAIL失败 (通常栈空间不足时,才会失败)
    
    // 开启任务调度
    vTaskStartScheduler();
  
    while(1){};  
}




