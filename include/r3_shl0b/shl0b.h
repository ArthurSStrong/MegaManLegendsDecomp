#ifndef SHL0B_H
#define SHL0B_H

#include "common.h"
#include "shl/shl.h"

extern SHL_BOX D_800D8F7C;
extern SHL_FUNC shl0b_move_tbl[]; // routine_0 handlers
extern uint8 D_800D8F98[];        // effect value by value_d0

void shl0b_atari_set(SHL_WORK *wp, uint16 arg);
void shl0b_explosion_set(SHL_WORK *wp);

#endif // SHL0B_H
