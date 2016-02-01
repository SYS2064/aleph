#include <blackfin.h>
#include <cdefBF533.h>  
#include "ccblkfn.h"

#include "audio.h"
#include "dma.h"
#include "gpio.h"
#include "isr.h"

// TEST: flip leds
/* long long unsigned int ledCounterRx = 0; */
/* long long unsigned int ledCounterTx = 0; */

#if DMA_DEINTERLEAVE_PINGPONG
// flags for pingpong processing
static volatile u8 inBufFlag = 0;
static volatile u8 outBufFlag = 0;
#endif

__attribute((interrupt_handler)) 
void sport0_rx_isr(void) {

  READY_LO;
#if DMA_DEINTERLEAVE_PINGPONG
  // FIXME: need to convert from 24b to 32b
  if(inBufFlag) {
    audioIn = &inputChannels1;
    inBufFlag = 0;
  } else {
    audioIn = &inputChannels0;
    inBufFlag = 1;
  }
  
#else
  u16 i;
  // copy input for new block
  for(i=0; i<AUDIO_FRAMES; i++) {  
    audioIn[0][i] = audioRxBuf[i * AUDIO_SAMPLESIZE] << 8;
    audioIn[1][i] = audioRxBuf[i * AUDIO_SAMPLESIZE + 1] << 8;
    audioIn[2][i] = audioRxBuf[i * AUDIO_SAMPLESIZE + 2] << 8;
    audioIn[3][i] = audioRxBuf[i * AUDIO_SAMPLESIZE + 3] << 8;
  } 
#endif

  /* if (++ledCounterRx > 4000) { */
  /*   ledCounterRx = 0; */
  /*   LED3_TOGGLE; */
  /* } */

  audioRxDone = 1;

  *pDMA1_IRQ_STATUS = 0x0001;
  ssync();
  READY_HI;
}


__attribute((interrupt_handler)) 
void sport0_tx_isr(void) {
  READY_LO;
#if DMA_DEINTERLEAVE_PINGPONG
  // FIXME: need to convert from 32b to 24b
  if(outBufFlag) {
    audioOut = &outputChannels1;
    outBufFlag = 0;
  } else {
    audioOut = &outputChannels0;
    outBufFlag = 0;
  }
#else
  u16 i;
  // copy output from last block
   for(i=0; i<AUDIO_FRAMES; i++) { 
     audioTxBuf[i * AUDIO_SAMPLESIZE] = audioOut[0][i] >> 8;
     audioTxBuf[i * AUDIO_SAMPLESIZE + 1] = audioOut[1][i] >> 8;
     audioTxBuf[i * AUDIO_SAMPLESIZE + 2] = audioOut[2][i] >> 8;
     audioTxBuf[i * AUDIO_SAMPLESIZE + 3] = audioOut[3][i] >> 8; 
   } 
#endif
   
  /* if (++ledCounterTx > 6000) { */
  /*   ledCounterTx = 0; */
  /*   LED4_TOGGLE; */
  /* } */
  
  audioTxDone = 1;

  // process pending param changes
  control_process();
  
  *pDMA2_IRQ_STATUS = 0x0001;
  ssync();

  READY_HI;
}


__attribute((interrupt_handler)) 
void spi_isr(void) {
  u8 rx, tx;
  READY_LO;

  rx = *pSPI_RDBR;
  // process the lsat byte.
  // if this completes a param change command, it will be queued.
  tx = spi_handle_byte(rx);
  *pSPI_TDBR = tx;
  
  READY_HI; 
}


// assign interrupts
void init_interrupts(void) {
  
#if 0 // test without SPI interrupts...
  /* int i; */
  
  /* // sport0 rx (dma1) -> ID3 = IVG10 */
  /* // sport1 tx (dma2) -> ID2 = IVG9 */
  /* *pSIC_IAR1 = 0x33322231; */

  /* // assign handlers to vectors */
  /* *pEVT10 = sport0_rx_isr; */
  /* *pEVT9 = sport0_tx_isr; */

  /* // unmask DMA1 and DMA2 */
  /* *pSIC_IMASK=0x00000600; */
  
  /* // unmask vectors in the core event processor */
  /* asm volatile ("cli %0; bitset(%0, 9); bitset(%0, 10); sti %0; csync;": "+d"(i)); */

  #else

  int i=0;


  // no interrupts in IAR0
  *pSIC_IAR0 = 0x88888888;
  // sport0 rx (dma1) -> ID3 = IVG10
  // sport0 tx (dma2) -> ID2 = IVG9
  // spi (dma5) -> ID4 = IVG11
  *pSIC_IAR1 = 0x88488238;
  // no interrupts in IAR2
  *pSIC_IAR2 = 0x88888888;

  // assign handlers to vectors
  *pEVT10 = sport0_rx_isr;
  *pEVT9 = sport0_tx_isr;
  *pEVT11 = spi_isr;
  
  // unmask peripherals
  *pSIC_IMASK=0x00002600;
  
  // unmask vectors in the core event processor
    asm volatile ("cli %0; bitset(%0, 9); bitset(%0, 10); bitset(%0, 11); sti %0; csync;": "+d"(i));

  

#endif
  
}
