/* scaler for amplitude (attentuation only)

 */

#include "param_scaler.h"
#include "types.h"

extern void scaler_amp_init(ParamScaler* sc, const ParamDesc* desc);
extern s32 scaler_amp_val(io_t in);
extern void scaler_amp_str(char* dst, io_t in);
//extern s32 scaler_val_amp(scaler* sc, io_t in);
//extern s32 scaler_rep_amp(scaler* sc, io_t in);
