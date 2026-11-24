#include "common.h"
#include "stage_common/stcom38.h"

// Shared stage code, linked into 4 stages: r3_st0a, r3_st10, r3_st19 and r3_st1b. Each stage
// compiles this file against its own copy of the assembly (TARGET_ASM, set by tools/cc.sh); the
// names are the same in every stage's registry.
#define ASM_DIR TARGET_ASM "/stage_common/stcom38"

// Room handlers: each starts its room's script once.
void stcom38_room_none(void)
{
}

void stcom38_room1(STCOM38_ROOM_WORK *wp)
{
    if (wp->started_4 == 0)
    {
        Sce_slot_open_list(stcom38_room1_list);
        wp->started_4 = 0xFF;
    }
}

void stcom38_room2(STCOM38_ROOM_WORK *wp)
{
    if (wp->started_4 == 0)
    {
        Sce_slot_open_list(stcom38_E_0000);
        wp->started_4 = 0xFF;
    }
}

void stcom38_room3(STCOM38_ROOM_WORK *wp)
{
    if (wp->started_4 == 0)
    {
        Sce_slot_open_list(stcom38_E2_0000);
        wp->started_4 = 0xFF;
    }
}
