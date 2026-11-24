#ifndef STCOM04_H
#define STCOM04_H

#include "common.h"
#include "stage_common/rock_neo.h"

// The block at +0x348 of the work slot.
typedef struct
{
    sint32 value_0;          // 348: cleared after every routine (stcom04_bird)
    sint32 value_4;          // 34C
    sint32 value_8;          // 350
    sint32 value_C;          // 354
    uint8 reserved10[8];     // 358
    STCOM_SVECTOR rot_speed; // 360: added to the rotation each frame
} STCOM04_CTL;

// The 0x3C0-byte work slot stcom04 runs on (freed through ROCK_NEO's Close_EM_WORK): a bird
// that walks, takes off when Mega Man comes close, or (type 1) can be examined.
typedef struct
{
    uint8 flags_0;                          // 00
    uint8 reserved01[2];                    // 01
    uint8 index_3;                          // 03: stcom04_type_tbl handler
    uint8 reserved04[4];                    // 04
    uint8 routine_0;                        // 08
    uint8 routine_1;                        // 09: also a counter
    uint8 reserved0A[2];                    // 0A
    sint8 count_C;                          // 0C
    uint8 reserved0D[3];                    // 0D
    uint32 value_10;                        // 10
    STCOM_SVECTOR position;                 // 14
    STCOM_FIXED_VECTOR position_fixed;      // 1C: position as 16.16
    STCOM_SVECTOR position_prev;            // 2C
    STCOM_FIXED_VECTOR position_fixed_prev; // 34
    STCOM_SVECTOR velocity;                 // 44
    uint8 reserved4C[8];                    // 4C
    STCOM_SVECTOR rotation;                 // 54
    STCOM_SVECTOR rotation_prev;            // 5C
    sint16 turn_64;                         // 64: turn speed
    sint16 rot_goal_66;                     // 66
    uint8 reserved68[6];                    // 68
    sint16 value_6E;                        // 6E
    uint8 reserved70[4];                    // 70
    uint8 value_74;                         // 74
    uint8 reserved75[3];                    // 75
    void *data_78;                          // 78
    uint8 reserved7C[0x28];                 // 7C
    uint8 frame_A4;                         // A4
    uint8 reservedA5[2];                    // A5
    sint8 state_A7;                         // A7
    uint8 reservedA8[8];                    // A8
    sint16 value_B0;                        // B0
    sint16 value_B2;                        // B2
    uint8 reservedB4[0x4B];                 // B4
    sint8 value_FF;                         // FF
    uint8 reserved100[3];                   // 100
    sint8 value_103;                        // 103
    uint8 reserved104[3];                   // 104
    sint8 value_107;                        // 107
    uint8 reserved108[0x50];                // 108
    STCOM_SVECTOR_W scale;                  // 158
    uint8 reserved160[0x20];                // 160
    STCOM_SVECTOR value_180;                // 180
    uint8 reserved188[0x1C0];               // 188
    STCOM04_CTL ctl;                        // 348
    uint8 reserved368[0x58];                // 368
} STCOM04_WORK;

typedef void (*STCOM04_FUNC)(STCOM04_WORK *wp);

extern STCOM04_FUNC stcom04_type_tbl[]; // handlers by index_3 (stcom04_move)
extern STCOM04_FUNC stcom04_flee_tbl[]; // routine_0 handlers (stcom04_flee)
extern uint8 stcom04_D_0010[];          // data_78 (stcom04_work_init)
// Camera target for Camera_move_set_pause (stcom04_talk_check).
extern STCOM_FIXED_VECTOR stcom04_cam_target;

// ROCK_NEO.EXE: steps the angle `rot` toward `goal` by at most `speed`; returns the step (0 when
// there). Its result isn't sign-extended here (see rock_neo.h).
sint32 Muku_angle(sint32 goal, sint32 rot, sint32 speed);

void stcom04_work_init(STCOM04_WORK *wp);
void stcom04_turn_set(STCOM04_WORK *wp, sint32 hit);
void stcom04_player_check(STCOM04_WORK *wp);
void stcom04_func_0898(STCOM04_WORK *wp, uint8 on);
void stcom04_func_08B8(STCOM04_WORK *wp, uint8 on);
sint32 stcom04_talk_check(STCOM04_WORK *wp);
void stcom04_circle_turn(STCOM04_WORK *wp, sint32 hit);
uint8 stcom04_pos_move(STCOM04_WORK *wp, uint32 *hit);

// ROCK_NEO.EXE (see rock_neo.h).
sint16 Muku(STCOM_SVECTOR *pos, STCOM_SVECTOR *target, sint32 rot, sint32 speed);
sint16 Scr_height_check(void *wp);

#endif // STCOM04_H
