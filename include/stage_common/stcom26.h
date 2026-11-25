#ifndef STCOM26_H
#define STCOM26_H

#include "common.h"
#include "stage_common/rock_neo.h"
#include "stage_common/stcom25.h"

// The 0x50-byte work slot (freed through Close_WORK_50) that spawns a room's drivers
// (stcom25) and keeps them from running into each other.
typedef struct
{
    uint8 flags_0;         // 00
    uint8 index_1;         // 01
    uint8 kind_2;          // 02
    uint8 variant_3;       // 03: the room's set of drivers (0xFF: none)
    uint8 reserved04[4];   // 04
    uint8 routine_0;       // 08
    uint8 reserved09[0xB]; // 09
    union
    {
        uint8 bytes[0x10];
        uint32 words[4];
    } slot_14;              // 14: the drivers' slot numbers in Shl_work_tbl
    uint8 reserved24[0x2C]; // 24
} STCOM26_WORK;

typedef void (*STCOM26_FUNC)(STCOM26_WORK *wp);

extern sint16 stcom26_count_tbl[][4];        // by variant_3: [0] the number of drivers
extern STCOM26_FUNC stcom26_move_tbl[];      // routine_0 (stcom26_move)
extern STCOM25_WORK *stcom26_work_tbl[0x10]; // the drivers
extern uint8 stcom26_slot_tbl[0x10];         // the drivers' slot numbers, unpacked each frame

void stcom26_slot_unpack(STCOM26_WORK *wp);
void stcom26_gap_update(STCOM26_WORK *wp);
void stcom26_gap_check(STCOM25_WORK *a, STCOM25_WORK *b);
sint16 stcom26_way_clear(STCOM25_WORK *a, STCOM25_WORK *b);
sint16 stcom26_cell_clear(uint16 *cell, sint16 dir, sint32 n, sint32 range);
void stcom26_speed_all(STCOM26_WORK *wp);
void stcom26_speed_set(STCOM25_WORK *wp);

// ROCK_NEO.EXE.
// The 0x10C-byte work slots.
extern STCOM25_WORK Shl_work_tbl[];
// Allocates a 0x10C-byte work slot; NULL when none is free.
STCOM25_WORK *Open_SHL_WORK(void);
// Frees a 0x50-byte work slot.
void Close_WORK_50(STCOM26_WORK *wp);

#endif // STCOM26_H
