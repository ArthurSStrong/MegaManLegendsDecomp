#ifndef SHL05_H
#define SHL05_H

#include "common.h"
#include "shl/shl.h"

// Per-level weapon values in ROCK_NEO.EXE (8-byte records).
typedef struct
{
    sint16 flags_0; // 00
    sint16 value_2; // 02
    sint16 speed_4; // 04
    sint16 value_6; // 06
} SHL05_LEVEL;

// Sprite scales, cycled by routine_1.
typedef struct
{
    sint32 scale_96;
    sint32 scale_97;
} SHL05_SCALE;

// An effect slot carrying a copy of the shot's sprite.
typedef struct
{
    sint8 routine_0; // 00
    uint8 reserved01;
    uint8 kind_2; // 02
    uint8 flag_3; // 03
    uint8 reserved04[0x10];
    SHL_SPRITE spr; // 14
} SHL05_FX;

extern SHL05_LEVEL D_80089C6C[];

extern SHL_BOX D_800D95B0;
extern SHL05_SCALE shl05_scale_tbl[6];
extern SHL_FUNC shl05_move_tbl[]; // routine_0 handlers
extern uint32 shl05_player_dist;  // distance to the player, scaled to 0x80

sint16 shl05_map_bounce(SHL_WORK *wp);
void shl05_pos_update(SHL_WORK *wp);
void shl05_explosion_set(SHL_WORK *wp, SHL_FX *fx);
void shl05_player_dist_set(SHL_WORK *wp);
sint32 shl05_vec_distance(SVECTOR *a, SVECTOR *b);
void func_800D9574(SHL_WORK *wp, SHL_FX *fx);

#endif // SHL05_H
