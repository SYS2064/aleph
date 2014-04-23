#include <stdlib.h>
#include "echoTap.h"
#include "pan.h"

// intialize tap
extern void echoTap24_8_init(echoTap24_8* tap, bufferTapN* tapWr){
  tap->tapWr = tapWr;
  tap->idx_last = tapWr->idx;

  tap->echoMin = 0;
  tap->echoMax = 256 * tapWr->loop;
  tap->echoTime = (tap->echoMax + tap->echoMin + 1 )/2;
  tap->shape = SHAPE_TOPHAT;
  tap->edge = EDGE_WRAP;

  //256 subsamples / sample = 1x in real time
  tap->playback_speed = 256;
}

extern void echoTap24_8_next(echoTap24_8* tap){
    if(tap->echoTime < tap->echoMax && tap->echoTime > tap->echoMin )
        tap->echoTime += tap->tapWr->inc*256 - tap->playback_speed;
    else {
    s32 echoRange;
        switch (tap->edge) {
            case EDGE_ONESHOT:
                tap->playback_speed = 0;
                break;
            case EDGE_WRAP:
                echoRange = tap->echoMax - tap->echoMin;
                tap->echoTime = abs (tap->echoTime - tap->echoMin + echoRange) % echoRange;
                tap->echoTime += tap->echoMin;
                break;
            case EDGE_BOUNCE:
                if(tap->echoTime < tap->echoMin) {
                    tap->playback_speed = abs(tap->playback_speed) * -1 ;
                }
                else if (tap->echoTime > tap->echoMax) {
                    tap->playback_speed = abs(tap->playback_speed) ;
                }
                tap->echoTime += tap->tapWr->inc*256 - tap->playback_speed;
                break;
        }
    }
}
extern fract32 echoTap24_8_envelope(echoTap24_8 *tap){
    fract32 amplitude;
    s32 center, dist_from_center, scale_factor;
    switch(tap->shape) {
        case SHAPE_TOPHAT:
            break;
        case SHAPE_TRIANGLE:
            center = (tap->echoMin + tap->echoMax) / 2;
            dist_from_center = abs(tap->echoTime - center);
            scale_factor = FR32_MAX/(tap->echoMax - center);
            amplitude = dist_from_center * scale_factor;
            amplitude =  sub_fr1x32 (FR32_MAX, amplitude);
            break;
        case SHAPE_LUMP:
            center = (tap->echoMin + tap->echoMax) / 2;
            scale_factor = FR32_MAX/(tap->echoMax - center);
            amplitude = dist_from_center * scale_factor;
            amplitude = mult_fr1x32x32(amplitude, amplitude);
            amplitude =  sub_fr1x32 (FR32_MAX, amplitude);
        case SHAPE_FATLUMP:
            center = (tap->echoMin + tap->echoMax) / 2;
            dist_from_center = abs(tap->echoTime - center);
            scale_factor = FR32_MAX/(tap->echoMax - center);
            amplitude = dist_from_center * scale_factor;
            amplitude = mult_fr1x32x32(amplitude, amplitude);
            amplitude = mult_fr1x32x32(amplitude, amplitude);
            amplitude =  sub_fr1x32 (FR32_MAX, amplitude);
        case SHAPE_OBESELUMP:
            center = (tap->echoMin + tap->echoMax) / 2;
            dist_from_center = abs(tap->echoTime - center);
            scale_factor = FR32_MAX/(tap->echoMax - center);
            amplitude = dist_from_center * scale_factor;
            amplitude = mult_fr1x32x32(amplitude, amplitude);
            amplitude = mult_fr1x32x32(amplitude, amplitude);
            amplitude = mult_fr1x32x32(amplitude, amplitude);
            amplitude =  sub_fr1x32 (FR32_MAX, amplitude);
        default:
            amplitude = FR32_MAX;
            break;
    }
    return amplitude;
}

// interpolated read
extern fract32 echoTap24_8_read(echoTap24_8* echoTap){
    s32 loop = echoTap->tapWr->loop * 256;
    s32 idx = (echoTap->tapWr->idx * 256 + loop - echoTap->echoTime) % loop;

    u32 samp1_index = idx;
    u32 samp2_index = (idx + 256) % loop;

    fract32 samp1 = echoTap->tapWr->buf->data[samp1_index / 256];
    fract32 samp2 = echoTap->tapWr->buf->data[samp2_index / 256];
    fract32 inter_sample = FR32_MAX/256 * (idx % 256);
    u8 samp1_sign = abs(samp1) / samp1;
    u8 samp2_sign = abs(samp2) / samp2;
    echoTap->zero_crossing = (samp1_sign != samp2_sign);
    return mult_fr1x32x32(echoTap24_8_envelope(&(echoTap)), pan_lin_mix(samp1, samp2, inter_sample) );
}

// set echo time directly in subsamples
extern void echoTap24_8_set_pos(echoTap24_8* tap, s32 echoTime){
    tap->echoTime = echoTime;
}
