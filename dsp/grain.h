/*
delay.h
aleph-audio

delay line object interface/

  single uninterpolated write head, single 24.8 interpolated read head

 */
#ifndef _ALEPH_LINES_DELAYLINE_H_
#define _ALEPH_LINES_DELAYLINE_H_

#include "buffer.h"
#include "types.h"
#include "echoTap.h"
#include "scrubTap.h"

// delay line data structure
typedef struct _grain {
  //-- audio buffer class
  audioBuffer buffer;
  echoTap echoTap;
  scrubTap scrubTap;
  bufferTapN tapWr;

  s32 echoTimeTarget;
  s32 echoTimeCountdown;

  s32 echoMaxTarget;
  s32 echoMaxCountdown;

  s32 echoMinTarget;
  s32 echoMinCountdown;

  s32 echoFadeLengthTarget;
  s32 echoFadeLengthCountdown;

  s32 scrubFadeLengthTarget;
  s32 scrubFadeLengthCountdown;

  s32 scrubLengthTarget;
  s32 scrubLengthCountdown;

} grain;


// initialize with pointer to audio buffer data
extern void grain_init(grain* dl, fract32* bufData, u32 frames);

// get next value given input
extern fract32 grain_next(grain* dl, fract32 in);

void  grain_set_scrubFadeLength(grain* dl, s32 subsamples) ;

void  grain_set_echoFadeLength(grain* dl, s32 subsamples) ;

//set scrubPitch (24.8 fraction of original frequency)
void grain_set_scrubPitch(grain* dl, s32 subsamples);

//set scrubLength (24.8 time in samples)
void grain_set_scrubLength(grain* dl, s32 subsamples);

//set scrubRandomise (24.8 time in samples)
void grain_set_scrubRandomise(grain* dl, s32 subsamples);

void grain_set_echoTime(grain* dl, s32 subsamples);

void grain_set_echoSpeed(grain* dl, s32 subsamples);

void grain_set_echoEdgeBehaviour(grain* dl, s32 edgeBehaviour);

void grain_set_echoMin(grain* dl, s32 subsamples);

void grain_set_echoMax(grain* dl, s32 subsamples);

void grain_set_writeEnable(grain* dl, s32 enable);
#endif // h guard
