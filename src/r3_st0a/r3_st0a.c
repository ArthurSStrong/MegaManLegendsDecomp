#include "common.h"
#include "st0a.h"

void st0a_slot_tbl_set(void)
{
    STAGE_TABLE_3C0 = D_801146A8;
    STAGE_TABLE_5A4 = D_801147E8;
    STAGE_TABLE_10C = D_80114818;
    STAGE_TABLE_48  = D_801148D8;
    STAGE_TABLE_24  = D_80114918;
    STAGE_TABLE_50  = D_80114950;
}

#ifdef __wasm32__
void st0a_room_reset(void);
#endif

// Sets up the current room: map, spawns.
void st0a_room_init(void)
{
    void *spawn;
    sint16 *cells;
    Map_layer_set(D_8011499C[Game_work.value_51]);
    STAGE_MAP_WORK_PTR->w     = STAGE_MAP_TBL[Game_work.value_51].w;
    STAGE_MAP_WORK_PTR->h     = STAGE_MAP_TBL[Game_work.value_51].h;
    STAGE_MAP_WORK_PTR->x0    = STAGE_MAP_TBL[Game_work.value_51].x0;
    STAGE_MAP_WORK_PTR->z0    = STAGE_MAP_TBL[Game_work.value_51].z0;
    STAGE_MAP_WORK_PTR->x1    = STAGE_MAP_WORK_PTR->x0 + STAGE_MAP_WORK_PTR->w;
    STAGE_MAP_WORK_PTR->z1    = STAGE_MAP_WORK_PTR->z0 + STAGE_MAP_WORK_PTR->h;
    cells                     = STAGE_MAP_TBL[Game_work.value_51].cells;
    STAGE_PL_BYTE(0x44E)      = 1;
    spawn                     = D_801149C8[Game_work.value_51];
    STAGE_MAP_WORK_PTR->info  = STAGE_CELL_INFO;
    STAGE_MAP_WORK_PTR->cells = cells;
    STAGE_MAP_WORK_PTR->exits = D_80114A14[Game_work.value_51];
    Obj_spawn_list(spawn, D_800C356D);
    Fog_room_set(D_80114A40[D_80114AD8[Game_work.value_51]], D_80114A18[Game_work.value_51]);
    Automap_room_set(0);
    st0a_room_reset();
}

// Clears the room's slot list, locks the exits for Game_work.value_52 and resets the room script's
// routines.
void st0a_room_reset(void)
{
    Sce_slot_list_clear();
    *(uint32 *)&D_800989F8.routine_4 = 0;
    Sce_exit_lock_list(D_80114EE8, D_80114EF0[STAGE_GAME_VALUE_52]);
}

// Runs the current room's script.
void st0a_room_script(void)
{
    st0a_room_script_tbl[STAGE_GAME_VALUE_52][Game_work.value_51](&D_800989F8);
}

void func_80100584(void)
{
}

void func_8010058C(STAGE_ROOM *wp)
{
    STAGE_OBJ *obj;

    switch (wp->routine_4)
    {
        case 0:
            if (Sce_flag_test(0x26) == 0)
            {
                Sce_slot_open_list(D_80114EB8);
                obj = Open_EVE_WORK();
                if (obj != NULL)
                {
                    obj->flags_0   = 1;
                    obj->kind_2    = 0xD;
                    obj->value_4   = 0;
                    obj->variant_3 = 0;
                }
                wp->routine_4 = 1;
            }
            else
            {
                wp->routine_4 = 0xFF;
            }
            break;
        case 1:
            if (Sce_flag_test(0x26) != 0)
            {
                Sce_event_start(0x14);
                Sce_exit_lock(1, 1, 1);
                wp->routine_4 = 2;
            }
            break;
        case 2:
            if (Sce_flag_test(0x10) != 0)
            {
                Sce_event_start(0x15);
                Sce_exit_lock(1, 0, 0);
                wp->routine_4 = 3;
            }
            break;
        case 3:
            obj = Open_EVE_WORK();
            if (obj != NULL)
            {
                obj->flags_0   = 1;
                obj->kind_2    = 0xD;
                obj->value_4   = 0;
                obj->variant_3 = 1;
            }
            wp->routine_4 = 0xFF;
            break;
    }
}

void func_801006D4(void)
{
}

void func_801006DC(void)
{
}

void func_801006E4(void)
{
}
