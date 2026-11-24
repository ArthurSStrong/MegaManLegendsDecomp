#ifndef STCOM14_H
#define STCOM14_H

#include "common.h"
#include "stage_common/rock_neo.h"

// Smoke: emitters on 0x48-byte effect slots that spawn puffs (more effect slots) drifting up and
// fading out.

#define STCOM14_MODE  0x3D48004E // a puff's draw mode word
#define STCOM14_COLOR 0x2E404040 // its GPU code and starting color

typedef void (*STCOM14_FUNC)(STCOM_EFFECT *wp);

extern STCOM14_FUNC stcom14_emit_tbl[]; // handlers by variant_3 while value_D is 0
extern STCOM14_FUNC stcom14_puff_tbl[]; // handlers by variant_3 otherwise

// ROCK_NEO.EXE: frees a 0x48-byte work slot.
void Close_EFFECT_WORK(STCOM_EFFECT *wp);

#endif // STCOM14_H
