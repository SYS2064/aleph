#ifndef _ALEPH_BEES_OP_MIDI_OUT_NOTE_H_
#define _ALEPH_BEES_OP_MIDI_OUT_NOTE_H_

#include "op.h"

/*
  note: the ordering of output nodes in the op spec
  is significant for order of execution.

  need to decide on optimal triggering behavior.
  velocity-triggered for now.
 */

//--- op_midi_out_note_t : midi note output
typedef struct op_midi_out_note_struct {
  // operator base class
  op_t super;
  // input: chan, vel, num 
  volatile io_t* in_val[6];
  // outputs: none
  op_out_t outs[1];
  // midi 'cable number' for midi interfaces w/ multiple cables
  volatile io_t cable;
  // output channel (-1 == all channels)
  volatile io_t chan;
  // output note num
  volatile io_t num;
  // output velocity
  volatile io_t vel;
  // output velocity
  volatile io_t pitch;
  // output program
  volatile io_t prog;
} op_midi_out_note_t;

// init
void op_midi_out_note_init(void* op);
// de-init
void op_midi_out_note_deinit(void* op);

#endif // h guard
