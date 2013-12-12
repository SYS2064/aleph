
#ifndef _ALEPH_MODULE_MONO_PARAMS_H_
#define _ALEPH_MODULE_MONO_PARAMS_H_

//---------- defines
// ranges and radix
// radix should be minimal bits to accommodate entire integer range.
#define OSC_FREQ_MIN 0x100000      // 16
#define OSC_FREQ_MAX 0x40000000    // 16384
#define OSC_FREQ_RADIX 15
#define RATIO_MIN 0x2000     // 1/8
#define RATIO_MAX 0x80000    // 8
#define RATIO_RADIX 4
#define ENV_DUR_MIN 0x0040   // 1/1024
#define ENV_DUR_MAX 0x100000 // 32
#define ENV_DUR_RADIX 5
#define SMOOTH_FREQ_MIN 0x2000 // 1/8
#define SMOOTH_FREQ_MAX 0x400000 // 64
#define SMOOTH_FREQ_RADIX 7


// parameters
enum params {
  eParamFreq1,
  eParamFreq2,
  eParamRatio2,
  eParamAmp1,
  eParamAmp2,

  eParamIoAmp0,
  eParamIoAmp1, 
  eParamIoAmp2,
  eParamIoAmp3,

  eParamPm,
  eParamWave1,
  eParamWave2,

  /* eParamGate, */
  /* eParamAtkDur, */
  /* eParamRelDur, */
  /* eParamAtkCurve, */
  /* eParamRelCurve, */
  eParamFreq1Smooth,
  eParamFreq2Smooth,
  eParamPmSmooth,
  eParamWave1Smooth,
  eParamWave2Smooth,
  eParamAmp1Smooth,
  eParamAmp2Smooth,
  eParamNumParams
};

#define NUM_PARAMS eParamNumParams

extern void fill_param_desc(void);

#endif // h guard
