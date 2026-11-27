#ifndef SHL04_H
#define SHL04_H

#include "common.h"
#include "shl/shl.h"

// Model data loaded with the module (ARM01R.BIN), outside the code chunk.
typedef struct
{
    uint8 reserved00[0x20];
    SVECTOR pos_20; // 20
} SHL04_MODEL;

extern SHL04_MODEL *D_800DD600;
extern uint8 D_80089AB6;

extern SHL_FUNC shl04_move_tbl[]; // routine_0 handlers

void shl04_atari_set(SHL_WORK *wp);
void func_800D8ADC(SHL_WORK *wp, sint32 arg);
void shl04_tip_rotate(SHL_PLAYER *tp, SVECTOR *rot);

#endif // SHL04_H
