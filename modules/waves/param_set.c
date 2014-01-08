// set param values.
// this is a separate file for convenience only.

void module_set_param(u32 idx, ParamValue v) {
  switch(idx) {
  case eParamFreq1:
    /* set_freq1(v); */
    break;
  case eParamFreq2:
    /* set_freq2(v); */
    break;
  case eParamWave1:
    /*     filter_1p_lo_in(wave1Lp, BIT_ABS_32(FIX16_FRACT_TRUNC(v))); */
    /* //    filter_1p_lo_in(wave1Lp, BIT_ABS_32((v))); */
    break;
  case eParamWave2:
        /* filter_1p_lo_in(wave2Lp, BIT_ABS_32(FIX16_FRACT_TRUNC(v))); */
	/* //    filter_1p_lo_in(wave2Lp, BIT_ABS_32((v))); */
    break;
  case eParamPm:
       /* filter_1p_lo_in(pmLp, BIT_ABS_32(FIX16_FRACT_TRUNC(v))); */
       /* //    filter_1p_lo_in(pmLp, BIT_ABS_32((v))); */
    break;
  case eParamAmp1:
    /* filter_1p_lo_in(amp1Lp, v); */
    break;
  case eParamAmp2:
    /* filter_1p_lo_in(amp2Lp, v); */
    break;
  case eParamFreq1Smooth:
    /* filter_1p_lo_set_slew(freq1Lp, v); */
    break;
  case eParamFreq2Smooth:
    /* filter_1p_lo_set_slew(freq2Lp, v); */
    break;
  case eParamPmSmooth:
    /* filter_1p_lo_set_slew(pmLp, v); */
    break;
  case eParamWave1Smooth:
    /* filter_1p_lo_set_slew(wave1Lp, v); */
    break;
  case eParamWave2Smooth:
    /* filter_1p_lo_set_slew(wave2Lp, v); */
    break;
  case eParamAmp1Smooth:
    /* filter_1p_lo_set_slew(amp1Lp, v); */
    break;
  case eParamAmp2Smooth:
    /* filter_1p_lo_set_slew(amp2Lp, v); */
    break;
  case eParamIoAmp0:
    ioAmp0 = (v);
    break;
  case eParamIoAmp1:
    ioAmp1 = (v);
    break;
  case eParamIoAmp2:
    ioAmp2 = (v);
    break;
  case eParamIoAmp3:
    ioAmp3 = (v);
    break;
  default:
    break;
  }
}
