#ifndef SHL07_H
#define SHL07_H

#include "common.h"
#include "shl/shl.h"

extern SHL_BOX D_8008A310[]; // indexed by an effect's index_a
extern uint8 D_80089ADA;

extern SHL_FUNC shl07_move_tbl[]; // routine_0 handlers
extern sint16 shl07_push_tbl[];   // push strength by Player_work.value_530

void Scr_move_check(SVECTOR *pos, SVECTOR *old, sint32 arg, SHL_BOX *box);

#endif // SHL07_H
