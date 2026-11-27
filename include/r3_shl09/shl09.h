#ifndef SHL09_H
#define SHL09_H

#include "common.h"
#include "shl/shl.h"

// Sprite scales by bit_d.
typedef struct
{
    uint8 scale_96;
    uint8 scale_97;
} SHL09_SCALE;

// Beam scale by level: length, then width.
typedef struct
{
    sint32 len;
    sint32 width;
} SHL09_SIZE;

extern SHL_BOX D_800D96F8;
extern sint32 shl09_shot_flags_tbl[];       // hit flags by Player_work.value_53c
extern SHL09_SCALE shl09_spark_scale_tbl[]; // type 2 sprite scales
extern SHL_FUNC shl09_type_tbl[];           // handlers by type_3
extern SHL_FUNC shl09_move_tbl[];           // type 0 routine_0 handlers
extern SHL_FUNC shl09_shot_tbl[];           // type 1 routine_0 handlers
extern SHL_FUNC shl09_spark_tbl[];          // type 2 routine_0 handlers
extern sint32 shl09_beam_reach_tbl[];       // beam reach by level
extern sint32 shl09_beam_size_tbl[];        // beam hit size by level
extern SHL09_SIZE shl09_beam_scale_tbl[];   // beam scale by level
extern sint32 shl09_front_size_tbl[];       // hit size by level

void shl09_beam_set(SHL_WORK *wp);
void shl09_front_atari_set(SHL_WORK *wp);
void shl09_shot_atari_set(SHL_WORK *wp);
void shl09_shot_open(SHL_WORK *wp);
void shl09_spark_open(SHL_WORK *wp);
void shl09_pos_from_player(SHL_WORK *wp);
void shl09_aim_vec(SVECTOR *out, sint32 speed);

#endif // SHL09_H
