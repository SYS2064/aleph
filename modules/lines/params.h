#ifndef _ALEPH_LINES_PARAMS_H_
#define _ALEPH_LINES_PARAMS_H_

// filter reciprocal of Q
#define PARAM_RQ_MIN 0x00000000
#define PARAM_RQ_MAX 0x7fffffff

//#define PARAM_BUFFER_MAX 0x002bf1ff // LINES_BUF_FRAMES - 1
// max time in seconds, 16.16
#define PARAM_SECONDS_MAX 0x003c0000
#define PARAM_SECONDS_RADIX 6
// rate
//...

// filter cutoff frequency
#define PARAM_FREQ_MAX 0x1F400000;  // fixme

// enumerate parameters
enum params {

  // delay 0
  eParam_delay0,		// 0
  eParam_loop0,			// 1 
  eParam_rMul0,			// 11
  eParam_rDiv0,			// 2
  eParam_write0,		// 3
  eParam_pre0,			// 4
  eParam_pos_write0,		// 5
  eParam_pos_read0,		// 6
  eParam_run_read0,		// 7
  eParam_run_write0,		// 8

  // delay 1
  eParam_delay1,		// 9
  eParam_loop1,			// 10
  eParam_rMul1,			// 11
  eParam_rDiv1,			// 11
  eParam_write1,		// 12
  eParam_pre1,			// 13
  eParam_pos_write1,		// 14
  eParam_pos_read1,		// 15
  eParam_run_read1,		// 16
  eParam_run_write1,		// 17

  // filter 1
  eParam_freq0,		// 18
  eParam_rq0,			// 19
  eParam_low0,			// 20
  eParam_high0,			// 21
  eParam_band0,			// 22
  eParam_notch0,		// 23
  eParam_fwet0,			// 24
  eParam_fdry0,			// 25

  // filter 1
  eParam_freq1,		// 26
  eParam_rq1,			// 27
  eParam_low1,			// 28
  eParam_high1,			// 29
  eParam_band1,			// 30
  eParam_notch1,		// 31
  eParam_fwet1,			// 32
  eParam_fdry1,			// 33

  // delay input mix
  eParam_adc0_del0,		// 34
  eParam_adc0_del1,		// 35
  eParam_adc1_del0,		// 36
  eParam_adc1_del1,		// 37
  eParam_adc2_del0,		// 38
  eParam_adc2_del1,		// 39
  eParam_adc3_del0,		// 40
  eParam_adc3_del1,		// 41

  // feedback mix
  eParam_del0_del0,		// 42
  eParam_del0_del1,		// 43
  eParam_del1_del0,		// 44
  eParam_del1_del1,		// 45

  // delay output mix
  eParam_del0_dac0,		// 46
  eParam_del0_dac1,		// 47
  eParam_del0_dac2,		// 48
  eParam_del0_dac3,		// 49
  eParam_del1_dac0,		// 
  eParam_del1_dac1,		// 50
  eParam_del1_dac2,		// 51
  eParam_del1_dac3,		// 52

  // dry mix
  eParam_adc0_dac0,		// 53
  eParam_adc0_dac1,		// 54
  eParam_adc0_dac2,		// 55
  eParam_adc0_dac3,		// 56
  eParam_adc1_dac0,		// 57
  eParam_adc1_dac1,		// 58
  eParam_adc1_dac2,		// 59
  eParam_adc1_dac3,		// 60
  eParam_adc2_dac0,		// 61
  eParam_adc2_dac1,		// 62
  eParam_adc2_dac2,		// 63
  eParam_adc2_dac3,		// 64
  eParam_adc3_dac0,		// 65
  eParam_adc3_dac1,		// 66
  eParam_adc3_dac2,		// 67
  eParam_adc3_dac3,		// 68

  eParamNumParams
};  

extern void fill_param_desc(void);

#endif // header guard 
// EOF
