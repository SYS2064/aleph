#include "types.h"

#if ARCH_BFIN
#include "fix.h"
#endif


#define TWOPI 6
#define PI 3
typedef struct {
  fract32 lastIn;
  fract32 lastOut;
} hpf;

typedef struct {
  fract32 lastOut;
} lpf;


void hpf_init (hpf *myHpf);
fract32 hpf_next_dynamic (hpf *myHpf, fract32 in, fract32 freq);
void lpf_init (lpf *myLpf);
fract32 lpf_next_dynamic (lpf *myLpf, fract32 in, fract32 freq);

typedef struct {
  s32 phase;
} phasor ;

typedef struct {
  s32 cosPhase;
  s32 sinPhase;
} quadraturePhasor ;

fract32 osc (fract32 phase);

#define simple_slew(x, y, slew) x = add_fr1x32( y,		     \
						mult_fr1x32x32(slew,	\
							       sub_fr1x32(x, y)))
fract32 max (fract32 x, fract32 y);
fract32 min (fract32 x, fract32 y);
#define SR 48000
#define hzToDimensionless(hz) ((fract32)((fract32)hz * (FR32_MAX / SR)))

typedef struct {
  fract32 instantaneousPeriod;
  fract32 lastIn;
  fract32 period;
  fract32 phase;
  s32 nsamples;
  hpf dcBlocker;
  lpf adaptiveFilter;
} pitchDetector;

void pitchDetector_init (pitchDetector *p);
fract32 pitchTrack (pitchDetector *p, fract32 preIn);
