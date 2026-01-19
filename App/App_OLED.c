#include "App.h"

//xTaskCreate( vTaskOLED,         "vTaskOLED",           512, NULL, 4, &xTaskOLED_Handle);

void vTaskOLED(){
    OLED_DisPlay_On();
    while(1){
        //OLED_Refresh();
        OLED_ShowString(0, 0,"TESTESTESTESTEST",16,1);
        OLED_ShowString(0,16,"TESTESTESTESTEST",16,1);
        OLED_ShowString(0,32,"TESTESTESTESTEST",16,1);
        OLED_ShowString(0,48,"TESTESTESTESTEST",16,1);
        OLED_Refresh();
        vTaskDelay(1000);    
    }
}
