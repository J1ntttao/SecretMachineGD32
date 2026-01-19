#include "bsp_keys.h"
#include <stdio.h>

// PA 1 2 3 4作为按键上拉输入
// 初始化按键
void bsp_keys_init(void){
    // 1. 时钟初始化
    rcu_periph_clock_enable(RCU_GPIOA);
    // 2. 配置GPIO 输入输出模式
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_6 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4);
}

// keydown weak func
__attribute__((weak)) void Keys_on_keydown(uint8_t key){};
// keyup weak func
__attribute__((weak)) void Keys_on_keyup(uint8_t key){};

// 记录上一次状态
FlagStatus last_state1 = SET;
FlagStatus last_state2 = SET;
FlagStatus last_state3 = SET;
FlagStatus last_state4 = SET;
FlagStatus last_state5 = SET;
// 扫描按键
uint8_t bsp_keys_scan(void){

    uint8_t keys_state = 0xFF;

    FlagStatus state1 = gpio_input_bit_get(GPIOA, GPIO_PIN_1);
    if(state1 != last_state1){
        last_state1 = state1;
        if(state1 == RESET){
            Keys_on_keydown(1);
            // 如果按下, 设置为0
            keys_state &= ~(0x01 << 1);
        }else{
            Keys_on_keyup(1);
            //printf("0x%x\n",keys_state);
        }
    }

    FlagStatus state2 = gpio_input_bit_get(GPIOA, GPIO_PIN_2);
    if(state2 != last_state2){
        last_state2 = state2;
        if(state2 == RESET){
            Keys_on_keydown(2);
            // 如果按下, 设置为0
            keys_state &= ~(0x01 << 2);
        }else{
            Keys_on_keyup(2);
            //printf("0x%x\n",keys_state);
        }
    }

    FlagStatus state3 = gpio_input_bit_get(GPIOA, GPIO_PIN_3);
    if(state3 != last_state3){
        last_state3 = state3;
        if(state3 == RESET){
            Keys_on_keydown(3);
            // 如果按下, 设置为0
            keys_state &= ~(0x01 << 3);
        }else{
            Keys_on_keyup(3);
            //printf("0x%x\n",keys_state);
        }
    }

    FlagStatus state4 = gpio_input_bit_get(GPIOA, GPIO_PIN_4);
    if(state4 != last_state4){
        last_state4 = state4;
        if(state4 == RESET){
            Keys_on_keydown(4);
            // 如果按下, 设置为0
            keys_state &= ~(0x01 << 4);
        }else{
            Keys_on_keyup(4);
            //printf("0x%x\n",keys_state);
        }
    }
    
    FlagStatus state5 = gpio_input_bit_get(GPIOA, GPIO_PIN_6);
    if(state5 != last_state5){
        last_state5 = state5;
        if(state5 == RESET){
            Keys_on_keydown(5);
            // 如果按下, 设置为0
            keys_state &= ~(0x01 << 5);
        }else{
            Keys_on_keyup(5);
            //printf("0x%x\n",keys_state);
        }
    }
    // 返回按键状态
    return keys_state;
}