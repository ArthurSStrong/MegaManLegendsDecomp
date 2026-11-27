#ifndef SHL06_H
#define SHL06_H

#include "common.h"
#include "shl/shl.h"

// Sprite scales, cycled by routine_1.
typedef struct
{
    sint32 scale_96;
    sint32 scale_97;
} SHL06_SCALE;

extern SHL_BOX D_800D9090;
extern SHL06_SCALE D_800D909C[6];
extern SHL_FUNC shl06_move_tbl[]; // routine_0 handlers

void shl06_explosion_set(SHL_WORK *wp);
void func_800D8CB8(SHL_WORK *wp, MATRIX *m);
void shl06_atari_set(SHL_WORK *wp);
void shl06_shot_copy(SHL_WORK *wp, SHL_WORK *np);
void shl06_shot_launch(SHL_WORK *wp, sint32 rx, sint32 ry);

#endif // SHL06_H
