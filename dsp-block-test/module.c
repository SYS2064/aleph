#include "module.h"
#include "osc.h"

void module_init(void) {
  osc_set_phi(0x00962fc9); // ~220 hz or something
  osc_set_amp(0x3fffffff);
}

void module_process_block(void) {
  u16 i;
  fract32* src = audioProcessInBuf;
  fract32* dst = audioProcessOutBuf;
  
  for(i=0; i<BLOCKSIZE; i++) {
    *dst++ = *src++;
    *dst++ = *src++;
    *dst++ = *src++;
    *dst++ = *src++;
  }
  
  osc_process_block(audioProcessOutBuf);
}
