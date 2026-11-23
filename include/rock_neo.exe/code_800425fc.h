#ifndef CODE_800425FC_H
#define CODE_800425FC_H

#include "common.h"

// Local view of a work record with two state bytes at +0x170 and +0x171
typedef struct
{
    uint8 reserved00[0x170];
    sint8 routine_170;
    sint8 routine_171;
} C800425FC_WORK;

// PL_WORK seen from its button masks: key[] is key_forward..+0x142 (one mask per action, from
// the control layout), pad[] is pad_tbl (PL_PAD_ENTRY[4], movement direction masks built from
// key[]).
typedef struct
{
    uint8 reserved00[0x124];
    uint16 key[16];
    uint16 pad[16];
} PL_KEY_VIEW;

typedef struct
{
    uint8 reserved00[6];
    sint16 bullet_left;
} C800425FC_PART;

// Local view of the actor fields touched by the routines in this file
typedef struct
{
    uint8 reserved00[9];
    uint8 b009;
    uint8 reserved0a[0x3A];
    union
    {
        uint16 u;
        sint16 s;
    } h44;
    uint8 reserved46[0x2E];
    sint8 s074;
    uint8 reserved75[0x37];
    uint8 b0ac;
    uint8 reserved0ad[0x53];
    uint8 b100;
    uint8 reserved101[2];
    sint8 s103;
    uint8 b104;
    uint8 reserved105[0xB];
    sint8 b110;
    uint8 reserved111[0xB];
    uint16 h11c;
    uint8 reserved11e[0x1C];
    uint16 h13a;
    uint16 h13c;
    uint8 reserved13e[0x32];
    sint8 routine_170;
    sint8 routine_171;
    uint8 reserved172[2];
    uint8 b174;
    uint8 b175;
    uint8 reserved176;
    uint8 b177;
    uint8 b178;
    uint8 reserved179[0x1F];
    uint32 w198;
    uint8 reserved19c[0x2B0];
    sint8 s44c;
    uint8 reserved44d[0xA7];
    C800425FC_PART weapon_data[16];
} C800425FC_ACT;

extern uint8 D_80089A84[];
extern uint8 Game_work[];
void Pl_move(C800425FC_ACT *ap, sint32 b, sint32 c, sint32 d);
void Pl_special_recoil(C800425FC_ACT *ap);
void Pl_special_lunge(C800425FC_ACT *ap);
extern uint8 D_80089A85[];
extern uint8 D_80089A86[];
extern uint8 D_80089A87[];

// Defined with the player's work type elsewhere; declared without parameters here.
void Pl_Char_Set();
void Pl_Char_Set_Upper();
sint32 Player_weapon_repeat_calc();
void Pl00_shot_enable_off();
uint8 Pl_anim_special(C800425FC_ACT *ap);
uint8 Pl_anim_buster(C800425FC_ACT *ap);

extern uint16 Pl_key_type_tbl[][16];

void Pl_key_type_set(PL_KEY_VIEW *wp, sint32 idx);

typedef void (*C800425FC_FUNC)(C800425FC_WORK *wp);

extern C800425FC_FUNC Pl_shot_special_tbl[];
extern C800425FC_FUNC Pl_shot_buster_tbl[];

void Pl_shot_special(C800425FC_WORK *wp);
void Pl_shot_buster(C800425FC_WORK *wp);

#endif // CODE_800425FC_H
