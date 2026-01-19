#ifndef __SPI0_H__
#define __SPI0_H__

#include "SPI_config.h"

#if USE_SPI_0   

void SPI0_init();

#if SPI0_MOSI_ENABLE
void SPI0_write(uint8_t data);
#endif

#if SPI0_MISO_ENABLE
uint8_t SPI0_read();
#endif

#if SPI0_MOSI_ENABLE && SPI0_MISO_ENABLE
uint8_t SPI0_write_read(uint8_t dat);
#endif


#if SPI0_DMA_TX_ENABLE
void SPI0_dma_tx_start(uint8_t* data, uint32_t len, uint8_t src_increment);
void SPI0_dma_tx_stop();
#if SPI0_DMA_TX_CALLBACK
extern void SPI0_dma_tx_on_finish();
#endif
#endif

#endif
#endif