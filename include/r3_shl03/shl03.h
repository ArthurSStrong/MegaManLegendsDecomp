#ifndef SHL03_H
#define SHL03_H

#include "common.h"
#include "shl/shl.h"

extern SHL_BOX D_800D8E18;
extern uint8 D_800D8E24[];        // sprite size by Player_work.value_50c
extern uint8 D_800D8E2C[];        // effect value by Player_work.value_50c
extern SHL_FUNC shl03_move_tbl[]; // routine_0 handlers

void shl03_explosion_set(SHL_WORK *wp);
void shl03_smoke_set(SHL_WORK *wp);
void shl03_atari_set(SHL_WORK *wp);

#endif // SHL03_H
