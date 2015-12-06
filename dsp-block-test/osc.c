#include <fract.h>
#include <fract_typedef.h>

#include "audio.h"
#include "osc.h"

const fract32 sine_table[WAVE_TAB_SIZE] = {
#include "sine_table_inc.c"
};

u32 phase = 0;
u32 phi = 0;
fract32 amp = 0;

void osc_set_phi(u32 _phi) {
  phi = _phi;
}

void osc_set_amp(fract32 _amp) {
  amp = _amp;
}

void osc_process_block(fract32* dst) {
  u16 i;
  fract32 val;

  u32 idxA;
  u32 idxB;
  fract32 mulA;
  fract32 mulB;
  fract32 waveA;
  fract32 waveB;
  
  fract32* p = dst;
  
  for(i=0; i<BLOCKSIZE; i++) {

    // phase is unsigned 32b
    // allow overflow
    phase = phase + phi;
    
    // lookup index
    // shift left for 10-bit index
    idxA = phase >> 22;
    idxB = (idxA + 1) & 1023;

    // use bottom 22 bits for interpolation
    // shift back to [0, 7fffffff]
    mulB = (fract32) ((phase & 0x3fffff) << 9);
    mulA = sub_fr1x32(0x7fffffff, mulB);

    waveA = shr_fr1x32(sine_table[idxA], 1);
    waveB = shr_fr1x32(sine_table[idxB], 1);

    // lookup, scale, and attenuate
    val = mult_fr1x32x32(
    		      amp,
    		      add_fr1x32(
    				 mult_fr1x32x32(waveA, mulA),
    				 mult_fr1x32x32(waveB, mulB)
    				 )
    		      );

    // mix to output buffer (all channels)
    *p = add_fr1x32(*p, val);
    p++;
    *p = add_fr1x32(*p, val);
    p++;
    *p = add_fr1x32(*p, val);
    p++;
    *p = add_fr1x32(*p, val);
    p++;
  }
}


