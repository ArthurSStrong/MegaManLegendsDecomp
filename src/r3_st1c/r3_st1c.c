#include "common.h"
#include "st1c.h"

// Installs the stage's slot routine tables.
void st1c_slot_tbl_set(void)
{
    STAGE_TABLE_3C0      = D_80103650;
    STAGE_TABLE_5A4      = D_80103790;
    STAGE_TABLE_10C      = D_801037A4;
    STAGE_TABLE_48       = D_801037A8;
    STAGE_TABLE_24       = D_801037E0;
    STAGE_TABLE_50       = D_801037E4;
    STAGE_PL_BYTE(0x170) = 0;
}

// Sets up the current room: models, map, spawns.
void st1c_room_init(void)
{
    void *spawn;
    sint16 *cells;

    STAGE_MAP_WORK_PTR->w     = STAGE_MAP_TBL[Game_work.value_51].w;
    STAGE_MAP_WORK_PTR->h     = STAGE_MAP_TBL[Game_work.value_51].h;
    STAGE_MAP_WORK_PTR->x0    = STAGE_MAP_TBL[Game_work.value_51].x0;
    STAGE_MAP_WORK_PTR->z0    = STAGE_MAP_TBL[Game_work.value_51].z0;
    STAGE_MAP_WORK_PTR->x1    = STAGE_MAP_WORK_PTR->x0 + STAGE_MAP_WORK_PTR->w;
    STAGE_MAP_WORK_PTR->z1    = STAGE_MAP_WORK_PTR->z0 + STAGE_MAP_WORK_PTR->h;
    cells                     = STAGE_MAP_TBL[Game_work.value_51].cells;
    STAGE_PL_BYTE(0x44E)      = 0;
    spawn                     = D_801038C0[Game_work.value_51];
    STAGE_MAP_WORK_PTR->info  = STAGE_CELL_INFO;
    STAGE_MAP_WORK_PTR->cells = cells;
    STAGE_MAP_WORK_PTR->exits = D_80103940[Game_work.value_51];
    Obj_spawn_list(spawn, D_800C356D);
    Fog_room_set(D_80103998[D_80103A18[Game_work.value_51]], D_80103948[Game_work.value_51]);
    Automap_room_set(0);
    st1c_room_reset();
}

// Resets the room script's routines (routine_4..7 of D_800989F8).
void st1c_room_reset(void)
{
    *(uint32 *)&D_800989F8.routine_4 = 0;
}

// Runs the current room's script.
void st1c_room_script(void)
{
    st1c_room_script_tbl[STAGE_GAME_VALUE_52][Game_work.value_51](&D_800989F8);
}

// Room 0: opens the slot list for the exhibit level once (and closes slot 0 while
// Game_work.value_52 < 8).
void st1c_room0_script(STAGE_ROOM_WORK *wp)
{
    sint32 level;

    level = st1c_exhibit_level();
    if (wp->routine_4 == 0)
    {
        Sce_slot_open_list(&D_80103C3C[level]);
        if (STAGE_GAME_VALUE_52 < 8)
        {
            Sce_slot_close(0);
        }
        wp->routine_4++;
    }
}

// Room 1: opens the slot list for the exhibit level, and an exhibit (WORK_50 kind 0x22) for each
// donated item (scene flags 0x38-0x3F).
void st1c_room1_script(STAGE_ROOM_WORK *wp)
{
    sint32 level;
    sint32 i;
    ST1C_OBJ50 *op;

    level = st1c_exhibit_level();
    if (wp->routine_4 == 0)
    {
        Sce_slot_open_list(&D_80103C5C[level]);
        for (i = 0; i < 8; i++)
        {
            if (Sce_flag_test(i + 0x38))
            {
                op = (ST1C_OBJ50 *)Open_WORK_50();
                if (op != NULL)
                {
                    op->kind_2  = 0x22;
                    op->index_C = i;
                    op->flags_0 |= 1;
                }
            }
        }
        wp->routine_4 = 0xFF;
    }
}

// The exhibit level (0 to 3) from the donated items (scene flags 0x38 to 0x3F), weighted by
// st1c_exhibit_weight_tbl.
uint8 st1c_exhibit_level(void)
{
    sint32 i;
    sint32 sum;

    sum = 0;
    for (i = 0x38; i < 0x40; i++)
    {
        if (Sce_flag_test(i))
        {
            sum += st1c_exhibit_weight_tbl[i];
        }
    }
    if (sum >= 0x2E)
    {
        return 3;
    }
    if (sum < 0x19)
    {
        return sum >= 0xA;
    }
    return 2;
}
