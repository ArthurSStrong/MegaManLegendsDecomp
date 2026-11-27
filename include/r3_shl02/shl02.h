#ifndef SHL02_H
#define SHL02_H

#include "common.h"
#include "shl/shl.h"

extern SHL_BOX D_800D9008;
extern SHL_FUNC shl02_move_tbl[]; // routine_0 handlers

void shl02_atari_set(SHL_WORK *wp);
void shl02_hit_effect_set(SHL_WORK *wp, sint32 arg);

#endif // SHL02_H
