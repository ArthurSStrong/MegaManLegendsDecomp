#ifndef STCOM35_H
#define STCOM35_H

#include "common.h"
#include "stage_common/rock_neo.h"

typedef void (*STCOM35_FUNC)(STCOM_EFFECT *wp);

extern STCOM35_FUNC stcom35_puff_tbl[]; // a puff's routine_0 handlers (stcom35_move)
extern STCOM35_FUNC stcom35_emit_tbl[]; // the emitter's, by variant_3
extern uint32 stcom35_color_tbl[];      // color steps by timer_E

// ROCK_NEO.EXE.
void Close_EFFECT_WORK(STCOM_EFFECT *wp);

#endif // STCOM35_H
