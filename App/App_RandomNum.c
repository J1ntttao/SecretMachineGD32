#include "App.h"

uint8_t NumArr[5] = {0};  // 存储5个不重复随机数

// 初始化TRNG
void trng_init(void){
    rcu_periph_clock_enable(RCU_TRNG);  // 使能TRNG时钟
    trng_enable();                      // 使能TRNG模块
    delay_1ms(10);                      // 给TRNG一点启动时间
}

// 获取单个随机数（范围1-7）
static uint8_t get_single_random(void){
    // 等待数据就绪
    while(SET != trng_flag_get(TRNG_FLAG_DRDY)){
        // 简单等待，可以添加超时处理
    }
    // 获取32位随机数并映射到1-7
    return (trng_get_true_random_data() % 7) + 1;
}

// 使用洗牌算法
void trng_generate_unique_5_shuffle(uint8_t *arr){
    uint8_t all_numbers[7] = {1, 2, 3, 4, 5, 6, 7};
    uint8_t i, j, temp;
    
    // Fisher-Yates洗牌算法
    for(i = 0; i < 6; i++){  // 只需要洗牌前6次
        // 获取随机位置 i 到 6
        j = i + (get_single_random() % (7 - i)) - 1;
        if(j < 6){  // 确保j在有效范围内
            // 交换
            temp = all_numbers[i];
            all_numbers[i] = all_numbers[j];
            all_numbers[j] = temp;
        }
    }
    // 取前5个数
    for(i = 0; i < 5; i++){
        arr[i] = all_numbers[i];
    }
}

// 打印数组
void print_array(uint8_t *arr, uint8_t size){
    for(uint8_t i = 0; i < size; i++){
        printf(" %d%c", arr[i], (i == size-1) ? '\n' : ',');
    }
}
