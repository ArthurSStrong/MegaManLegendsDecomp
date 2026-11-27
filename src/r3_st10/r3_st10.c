#include "common.h"
#include "st10.h"

void st10_slot_tbl_set(void)
{
    STAGE_TABLE_3C0 = D_8010F048;
    STAGE_TABLE_5A4 = D_8010F1D4;
    STAGE_TABLE_10C = D_8010F1F0;
    STAGE_TABLE_48  = D_8010F2AC;
    STAGE_TABLE_24  = D_8010F2EC;
    STAGE_TABLE_50  = D_8010F2F0;
}

#ifdef __wasm32__
void st10_room_reset(void);
#endif

// Sets up the current room: map, spawns.
void st10_room_init(void)
{
    STAGE_PL_VIEW *pl;
    void *spawn;
    sint16 *cells;
    Map_layer_set(D_8010F394[Game_work.value_51]);
    STAGE_MAP_WORK_PTR->w     = STAGE_MAP_TBL[Game_work.value_51].w;
    STAGE_MAP_WORK_PTR->h     = STAGE_MAP_TBL[Game_work.value_51].h;
    STAGE_MAP_WORK_PTR->x0    = STAGE_MAP_TBL[Game_work.value_51].x0;
    STAGE_MAP_WORK_PTR->z0    = STAGE_MAP_TBL[Game_work.value_51].z0;
    STAGE_MAP_WORK_PTR->x1    = STAGE_MAP_WORK_PTR->x0 + STAGE_MAP_WORK_PTR->w;
    STAGE_MAP_WORK_PTR->z1    = STAGE_MAP_WORK_PTR->z0 + STAGE_MAP_WORK_PTR->h;
    cells                     = STAGE_MAP_TBL[Game_work.value_51].cells;
    pl                        = &STAGE_PL;
    STAGE_MAP_WORK_PTR->info  = STAGE_CELL_INFO;
    STAGE_MAP_WORK_PTR->cells = cells;
    if (!Sce_flag_test(0x21B) && Sce_flag_test(0x21A) && STAGE_GAME_COUNT_62 >= 11)
    {
        Sce_flag_on(0x21B);
    }
    if (!Sce_flag_test(0x218) && Sce_flag_test(0x217) && STAGE_GAME_COUNT_62 >= 11)
    {
        Sce_flag_on(0x218);
    }
    if (Game_work.value_51 == 0)
    {
        if (Sce_flag_test(0x21B))
        {
            D_8010F5D4 = D_8010F604;
            Map_obj_model_swap(Scr_cell_get(&D_8010F90C), 0x180300);
        }
        else if (Sce_flag_test(0x218))
        {
            D_8010F5D4 = D_8010F5EC;
            Map_obj_model_swap(Scr_cell_get(&D_8010F90C), 0x180200);
        }
        pl->value_44E = 1;
    }
    else
    {
        pl->value_44E = 0;
    }
    spawn                     = D_8010F58C[Game_work.value_51];
    STAGE_MAP_WORK_PTR->exits = D_8010F754[Game_work.value_51];
    Obj_spawn_list(spawn, D_800C356D);
    Fog_room_set(D_8010F85C[D_8010F8F4[Game_work.value_51]], D_8010F76C[Game_work.value_51]);
    Automap_room_set(0);
    st10_room_reset();
}

// Clears the room's slot list, locks the exits for Game_work.value_52 and resets the room script's
// routines.
void st10_room_reset(void)
{
    *(uint32 *)&D_800989F8.routine_4 = 0;
    Sce_slot_list_clear();
    Sce_exit_lock_list(D_80110094, D_8011009C[STAGE_GAME_VALUE_52]);
}

// Runs the current room's script (the second set once scene flag 0x1FF is set).
void st10_room_script(void)
{
    if (!Sce_flag_test(0x1FF))
    {
        st10_first_script_tbl[Game_work.value_51](&D_800989F8);
    }
    if (Sce_flag_test(0x1FF))
    {
        st10_room_script_tbl[STAGE_GAME_VALUE_52][Game_work.value_51](&D_800989F8);
    }
}

void func_8010079C(STAGE_ROOM *wp)
{
    STAGE_OBJ *slot;

    if (wp->routine_4 == 0)
    {
        Sce_slot_open_list(D_8011004C);
        if (!Sce_flag_test(0x48))
        {
            Sce_event_start(0x63);
            Sce_flag_on(0x48);
        }
        else
        {
            slot = Sce_slot_open(D_801101B4);
            slot->flags_6 &= ~0x20;
        }
        wp->routine_4++;
    }
}

void func_80100838(void)
{
}

void func_80100840(STAGE_ROOM *wp)
{
    if (wp->routine_4 == 0)
    {
        Sce_slot_open_list(D_8011005C);
        wp->routine_4++;
    }
}

void func_80100894(STAGE_ROOM *wp)
{
    if (wp->routine_4 == 0)
    {
        Sce_slot_open_list(D_80110084);
        wp->routine_4++;
    }
}

void func_801008E8(STAGE_ROOM *wp)
{
    if (wp->routine_4 == 0)
    {
        Sce_slot_open_list(D_80110054);
        wp->routine_4++;
    }
}
