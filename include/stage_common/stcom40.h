#ifndef STCOM40_H
#define STCOM40_H

#include "common.h"
#include "stage_common/stcom21.h"

// A treasure chest whose lid opens (stcom40_chest) and a smaller box (stcom40_box). They share
// stcom21's work slot (STCOM21_WORK). Split from stcom21 because r3_st04 and r3_st0d link them
// without the vending machine.

// A 0x3C0-byte work slot (Open_EM_WORK) as stcom40_chest_em_spawn sets it up.
typedef struct
{
    uint8 flags_0;                     // 00
    uint8 index_1;                     // 01
    uint8 kind_2;                      // 02
    uint8 value_3;                     // 03
    uint8 handler_4;                   // 04
    uint8 reserved05[0xF];             // 05
    STCOM_SVECTOR position;            // 14
    STCOM_FIXED_VECTOR position_fixed; // 1C
    uint8 reserved2C[0x28];            // 2C
    STCOM_SVECTOR rotation;            // 54
} STCOM40_OBJ;

extern STCOM21_FUNC stcom40_chest_tbl[];        // chest: routine_0
extern STCOM21_FUNC stcom40_chest_type_tbl[];   // chest: by variant_3
extern STCOM21_FUNC stcom40_chest_closed_tbl[]; // chest: routine_1
extern uint32 stcom40_chest_model[];            // chest's model
extern uint16 stcom40_lid_angle_tbl[];          // lid angles
extern STCOM21_FUNC stcom40_box_tbl[];          // box: routine_0
extern uint32 stcom40_box_model[];              // box's model

void stcom40_lid_spawn(STCOM21_WORK *wp);
uint8 stcom40_chest_item_check(STCOM21_WORK *wp);
void stcom40_chest_em_spawn(STCOM21_WORK *wp);
uint8 stcom40_box_item_check(STCOM21_WORK *wp);

// ROCK_NEO.EXE.
STCOM40_OBJ *Open_EM_WORK(void);

#endif // STCOM40_H
