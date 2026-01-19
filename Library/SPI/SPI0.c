#include "SPI0.h"

#if USE_SPI_0

#if SPI0_SOFT

//////////////////////////// 软实现 ///////////////////////////////////

#define SCL(bit)		gpio_bit_write(SPI0_SCL_PORT, SPI0_SCL_PIN, bit ? SET : RESET)

#define MOSI(bit)		gpio_bit_write(SPI0_MOSI_PORT, SPI0_MOSI_PIN, bit ? SET : RESET)

#define MISO()       	gpio_input_bit_get(SPI0_MISO_PORT,SPI0_MISO_PIN)//FS0
void SPI0_init(){
	// SCL
	// MOSI
	
	// gpio output -------------------------------------------------
	rcu_periph_clock_enable(SPI0_SCL_RCU);
	gpio_mode_set(SPI0_SCL_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, SPI0_SCL_PIN);
	gpio_output_options_set(SPI0_SCL_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, SPI0_SCL_PIN);
	
	rcu_periph_clock_enable(SPI0_MOSI_RCU);
	gpio_mode_set(SPI0_MOSI_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, SPI0_MOSI_PIN);
	gpio_output_options_set(SPI0_MOSI_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, SPI0_MOSI_PIN);
	
	// gpio input --------------------------------------------------
	rcu_periph_clock_enable(SPI0_MISO_RCU);
	gpio_mode_set(SPI0_MISO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, SPI0_MISO_PIN);
//	gpio_output_options_set(MISO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, MISO_PIN);

	// IO init
	SCL(1);
	MOSI(1);
	
	// CPOL 1
	// CPHA 1
	
}

void SPI0_write(uint8_t dat){
	// MSB大端模式
	// 1101 0011
	for(uint8_t i=0;i<8;i++)
	{			  
		// SCL拉低
		SCL(0);
		
		// 输出
		MOSI(dat&0x80);
		
		// 左移1位
		dat<<=1;   
		
		// SCL拉高
		SCL(1);
	}				 	
}

uint8_t SPI0_read(){
	// 大端模式
	//  0000 0000 <- 1101 0101
	
	uint8_t i,read=0;
	for(i=0;i<8;i++)
	{
		// SCL拉低
		SCL(0);
		
		read <<= 1;
		if(MISO()) read++;
		
		// SCL拉高
		SCL(1);
  }
	return read;
}


uint8_t SPI0_read_write(uint8_t dat){
	uint8_t i,read=0;
	for(i=0;i<8;i++)
	{
		// SCL拉低
		SCL(0);
		
		// -------------------------- 输出MOSI
		MOSI(dat&0x80);
		// 左移1位
		dat<<=1;   
		
		// -------------------------- 输入MISO
		read <<= 1;
		if(MISO()) read++;
		
		// SCL拉高
		SCL(1);
  }
	return read;
}

#else

//////////////////////////// 硬实现 ///////////////////////////////////
#define SPIx_RCU		RCU_SPI0
#define SPIx			SPI0

#define SPI0_CLK_AF		GPIO_AF_5

#if SPI0_MOSI_ENABLE
#define SPI0_MOSI_AF	GPIO_AF_5
#endif

#if SPI0_MISO_ENABLE
#define SPI0_MISO_AF    GPIO_AF_5
#endif


#if SPI0_DMA_TX_ENABLE
#define SPIx_DMAx_TX_RCU       RCU_DMA1                //RCU_DMA1
#define SPIx_DMAx_TX_CHx       DMA1, DMA_CH3               //DMA1, DMA_CH3
#define SPIx_DMAx_TX_SUBPERI   DMA_SUBPERI3                //DMA_SUBPERI3

#if SPI0_DMA_TX_CALLBACK
#define SPIx_DMAx_TX_IRQ       DMA1_Channel3_IRQn
#define SPIx_INT_FUNC          DMA1_Channel3_IRQHandler
#endif

static void dma_tx_init() {
    /***************** DMA m2p *******************/
    // 时钟
    rcu_periph_clock_enable(SPIx_DMAx_TX_RCU);
    // 重置dma
    dma_deinit(SPIx_DMAx_TX_CHx);

    //////// dma 配置
    dma_single_data_parameter_struct dsdps;
    dma_single_data_para_struct_init(&dsdps);
    // 方向
    dsdps.direction = DMA_MEMORY_TO_PERIPH;
    // 内存: src 
    // dsdps.memory0_addr = (uint32_t)src;
    dsdps.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    // 外设: dst
    dsdps.periph_addr = (uint32_t)(&SPI_DATA(SPIx));
    dsdps.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    // 数据长度
    // dsdps.number = ARR_LEN;
    // 数据宽度
    dsdps.periph_memory_width = DMA_MEMORY_WIDTH_8BIT;
    // 传输优先级
    dsdps.priority = DMA_PRIORITY_ULTRA_HIGH;
//    // 循环模式
//    dsdps.circular_mode = DMA_CIRCULAR_MODE_ENABLE;
    dma_single_data_mode_init(SPIx_DMAx_TX_CHx, &dsdps);

    //////// 配置 dma 子连接
    dma_channel_subperipheral_select(SPIx_DMAx_TX_CHx, SPIx_DMAx_TX_SUBPERI);
    
    dma_channel_disable(SPIx_DMAx_TX_CHx);
    dma_flag_clear(SPIx_DMAx_TX_CHx, DMA_FLAG_FTF);
    
#if SPI0_DMA_TX_CALLBACK
    dma_interrupt_enable(SPIx_DMAx_TX_CHx, DMA_INT_FTF);
    nvic_irq_enable(SPIx_DMAx_TX_IRQ, SPI0_DMA_TX_NVIC);
#endif
}

void SPI0_dma_tx_start(uint8_t* data, uint32_t len, uint8_t src_increment) {
    
     /* 关闭DMA通道 */
    dma_channel_disable(SPIx_DMAx_TX_CHx);
    
    dma_flag_clear(SPIx_DMAx_TX_CHx, DMA_FLAG_FTF);
    
    /* 配置DMA传输的内存地址和传输数据大小 */
    dma_memory_address_config(SPIx_DMAx_TX_CHx, DMA_MEMORY_0, (uint32_t)data);
    
    dma_transfer_number_config(SPIx_DMAx_TX_CHx, len);
    
    if(src_increment) {
        
        dma_memory_address_generation_config(SPIx_DMAx_TX_CHx, DMA_MEMORY_INCREASE_ENABLE);
        
    } else {
        
        dma_memory_address_generation_config(SPIx_DMAx_TX_CHx, DMA_MEMORY_INCREASE_DISABLE);
        
    }
        /* 使能DMA通道 */
    dma_channel_enable(SPIx_DMAx_TX_CHx);
}

void SPI0_dma_tx_stop() {
    
    dma_channel_disable(SPIx_DMAx_TX_CHx);
    
}

#if SPI0_DMA_TX_CALLBACK

void SPIx_INT_FUNC() {
    if(SET == dma_interrupt_flag_get(SPIx_DMAx_TX_CHx, DMA_INT_FLAG_FTF)) {
        dma_interrupt_flag_clear(SPIx_DMAx_TX_CHx, DMA_INT_FLAG_FTF);
        SPI0_dma_tx_on_finish();
    }
}

#endif

#endif

void SPI0_init() {

    /*********** GPIO config ***************/
    // clk
    rcu_periph_clock_enable(SPI0_CLK_PORT_RCU);
    gpio_mode_set(SPI0_CLK_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, SPI0_CLK_PIN);
    gpio_output_options_set(SPI0_CLK_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, SPI0_CLK_PIN);
    gpio_af_set(SPI0_CLK_PORT, SPI0_CLK_AF, SPI0_CLK_PIN);

#if SPI0_MOSI_ENABLE
    // mosi
    rcu_periph_clock_enable(SPI0_MOSI_PORT_RCU);
    gpio_mode_set(SPI0_MOSI_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, SPI0_MOSI_PIN);
    gpio_output_options_set(SPI0_MOSI_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, SPI0_MOSI_PIN);
    gpio_af_set(SPI0_MOSI_PORT, SPI0_MOSI_AF, SPI0_MOSI_PIN);
#endif
#if SPI0_MISO_ENABLE
    // miso
    rcu_periph_clock_enable(SPI0_MISO_PORT_RCU);
    gpio_mode_set(SPI0_MISO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, SPI0_MISO_PIN);
    gpio_output_options_set(SPI0_MISO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, SPI0_MISO_PIN);
    gpio_af_set(SPI0_MISO_PORT, SPI0_MISO_AF, SPI0_MISO_PIN);
#endif
    /*************** spi config **************/
    // 时钟
    rcu_periph_clock_enable(SPIx_RCU);

    spi_parameter_struct spi_init_struct;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    // negative slave select
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.clock_polarity_phase = SPI0_PARAM_CPOL_CPHA;
    spi_init_struct.prescale             = SPI0_PARAM_PSC;
    spi_init_struct.endian               = SPI0_PARAM_ENDIAN;
    spi_init(SPIx, &spi_init_struct);

    spi_init(SPIx, &spi_init_struct);
    
#if SPI0_DMA_TX_ENABLE
    dma_tx_init();
    
    spi_dma_enable(SPIx, SPI_DMA_TRANSMIT);
#endif

    spi_enable(SPIx);
}

#if SPI0_MOSI_ENABLE
void SPI0_write(uint8_t data) {
    while(RESET == spi_i2s_flag_get(SPIx, SPI_FLAG_TBE));
    //硬件电路，发送数据，放到寄存器
    spi_i2s_data_transmit(SPIx, data);

    while(RESET == spi_i2s_flag_get(SPIx, SPI_FLAG_RBNE));
    //硬件电路，发送数据，放到寄存器
    spi_i2s_data_receive(SPIx);
}
#endif
#if SPI0_MISO_ENABLE
uint8_t SPI0_read() {
    while(RESET == spi_i2s_flag_get(SPIx, SPI_FLAG_TBE));
    //硬件电路，发送数据，放到寄存器
    spi_i2s_data_transmit(SPIx, 0x00);

    while(RESET == spi_i2s_flag_get(SPIx, SPI_FLAG_RBNE));
    //硬件电路，发送数据，放到寄存器
    return spi_i2s_data_receive(SPIx);
}
#endif


#if SPI01_MOSI_ENABLE && SPI0_MISO_ENABLE
uint8_t SPI0_write_read(uint8_t dat) {
    while(RESET == spi_i2s_flag_get(SPIx, SPI_FLAG_TBE));
    //硬件电路，发送数据，放到寄存器
    spi_i2s_data_transmit(SPIx, dat);

    while(RESET == spi_i2s_flag_get(SPIx, SPI_FLAG_RBNE));
    //硬件电路，发送数据，放到寄存器
    return spi_i2s_data_receive(SPIx);
}
#endif



#endif
#endif

