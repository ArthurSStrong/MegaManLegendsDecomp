#include "common.h"
#include "stage_common/stcom29.h"

// Shared stage code, linked into 3 stages: r3_st03, r3_st13 and r3_st1b. Each stage compiles
// this file against its own copy of the assembly (TARGET_ASM, set by tools/cc.sh); the names
// are the same in every stage's registry.
#define ASM_DIR TARGET_ASM "/stage_common/stcom29"

// A model whose texture runs an animation script; variant 1 also runs a scene when scene flag
// 0x60D is set.
void stcom29_move(STCOM29_WORK *wp)
{
    stcom29_move_tbl[wp->variant_3][wp->routine_0](wp);
}

// Gets the model (frees the slot if missing) and picks the variant's script.
void stcom29_move_init(STCOM29_WORK *wp)
{
    wp->key_20 = Stage_hed_model_get(stcom29_D_0000[wp->variant_3]);
    if (wp->key_20 == 0)
    {
        Close_EVE_WORK(wp);
        return;
    }
    if (wp->variant_3 == 0)
    {
        wp->script_C = 0;
    }
    else if (wp->variant_3 == 2)
    {
        wp->script_C = 3;
    }
    else
    {
        wp->script_C = 1;
    }
    wp->step_D  = 0;
    wp->timer_E = stcom29_script_tbl[wp->script_C]->time_0;
    wp->routine_0++;
}

// Variant 1: waits for scene flag 0x60D.
void stcom29_scene_wait(STCOM29_WORK *wp)
{
    if (Sce_flag_test(0x60D) != 0)
    {
        wp->routine_1 = 0;
        wp->routine_0++;
    }
}

// Variant 1's scene: stops the player, plays the animation to step 4, shows a message, plays
// script 2 to step 4, then hands control back.
void stcom29_scene(STCOM29_WORK *wp)
{
    STCOM_PL_WORK *pl;

    pl = &Player_work;
    switch (wp->routine_1)
    {
        case 0:
            if (Player_control_off(pl) != 0)
            {
                pl->routine_0 = 3;
                pl->routine_1 = 0;
                pl->value_448 = 0x30;
                pl->value_C   = 0;
                wp->routine_1++;
            }
            break;
        case 2:
            stcom29_scene_message();
            wp->routine_1++;
            break;
        case 3:
            if (!(Moji_flag & 0x8000000))
            {
                wp->script_C = 2;
                wp->step_D   = 0;
                wp->timer_E  = stcom29_script_tbl[wp->script_C]->time_0;
                wp->routine_1++;
            }
            break;
        case 1:
        case 4:
            stcom29_anim_step(wp);
            if (wp->step_D == 4)
            {
                wp->routine_1++;
            }
            break;
        case 5:
            Player_control_on(pl);
            pl->flags_0 |= 2;
            Sce_flag_off(0x60D);
            wp->script_C  = 1;
            wp->step_D    = 0;
            wp->timer_E   = stcom29_script_tbl[wp->script_C]->time_0;
            wp->routine_0 = 1;
            break;
    }
}

// Steps the texture animation script.
void stcom29_anim_step(STCOM29_WORK *wp)
{
    STCOM29_UV4 uv;
    STCOM29_UNPACK unpack;
    STCOM29_STEP *step;

    step = &stcom29_script_tbl[wp->script_C][wp->step_D];
    if (step->time_0 == 0)
    {
        return;
    }
    unpack = Model_poly_group_get((void *)wp->key_20, 0, stcom29_D_010C[wp->variant_3]);
    if (--wp->timer_E == 0)
    {
        step++;
        wp->step_D++;
        if (step->time_0 == 0xFF)
        {
            wp->step_D = 0;
            step       = stcom29_script_tbl[wp->script_C];
        }
        else if (step->time_0 == 0)
        {
            return;
        }
        wp->timer_E = stcom29_script_tbl[wp->script_C]->time_0;
    }
    uv      = Quad_uv_get(unpack.c, 0);
    uv.b[0] = stcom29_D_000C[step->frame_1][0];
    uv.b[1] = stcom29_D_000C[step->frame_1][1];
    uv.b[2] = stcom29_D_000C[step->frame_1][2];
    uv.b[3] = stcom29_D_000C[step->frame_1][3];
    uv.b[4] = stcom29_D_000C[step->frame_1][4];
    uv.b[5] = stcom29_D_000C[step->frame_1][5];
    uv.b[6] = stcom29_D_000C[step->frame_1][6];
    uv.b[7] = stcom29_D_000C[step->frame_1][7];
    Quad_uv_set(unpack.c, 0, uv.w);
}

// Starts the scene's message: 0x22 unless stcom29_func_055C passes, else 1 the first time
// (setting its scene flags) and 2 after.
void stcom29_scene_message(void)
{
    if (stcom29_func_055C())
    {
        if (Sce_flag_test(0x235) == 0)
        {
            Message_event_set(NULL, 2, 0, 1);
            Sce_flag_on(0x7E5);
            Sce_flag_on(0x7E6);
            Sce_flag_on(0x7E7);
            Sce_flag_on(0x235);
            Sce_flag_on(0x212);
            Game_work.value_78 = 0;
            return;
        }
        Message_event_set(NULL, 2, 0, 2);
    }
    else
    {
        Message_event_set(NULL, 2, 0, 0x22);
    }
}

uint8 stcom29_func_055C(void)
{
    if (Game_work.value_50 == 0x13)
    {
        return 0;
    }
    if (Sce_flag_test(0x214) != 0)
    {
        return 0;
    }
    if (Sce_flag_test(0x52) == 0)
    {
        return 0;
    }
    if (Sce_flag_test(0x522) == 0 || Map_obj_state_get(6, 0xFC) != 3)
    {
        return 0;
    }
    return Map_obj_state_get(6, 0x120) == 3;
}

// Five texture layers, each stepping its own script.
void stcom29_layers_move(STCOM29_ANIM_WORK *wp)
{
    stcom29_layers_tbl[wp->routine_0](wp);
}

// Gets both models; frees the slot if either is missing.
void stcom29_layers_init(STCOM29_ANIM_WORK *wp)
{
    wp->key_1C[0] = Stage_hed_model_get(0x130000);
    if (wp->key_1C[0] == 0 || (wp->key_1C[1] = Stage_hed_model_get(0x130200)) == 0)
    {
        Close_EVE_WORK(wp);
        return;
    }
    wp->step_C[1]   = 0;
    wp->step_C[4]   = 0;
    wp->step_C[3]   = 0;
    wp->step_C[2]   = 0;
    wp->step_C[0]   = 0;
    wp->timer_11[0] = stcom29_D_0110[0][wp->step_C[0]][1];
    wp->timer_11[1] = stcom29_D_0110[1][wp->step_C[1]][1];
    wp->timer_11[2] = stcom29_D_0110[2][wp->step_C[2]][1];
    wp->timer_11[3] = stcom29_D_0110[3][wp->step_C[3]][1];
    wp->timer_11[4] = stcom29_D_0110[4][wp->step_C[4]][1];
    wp->routine_0++;
}

// Steps each layer's script and moves its texture.
void stcom29_layers_main(STCOM29_ANIM_WORK *wp)
{
    STCOM29_UV4 uv;
    STCOM29_UNPACK unpack;
    sint16 i;

    for (i = 0; i < 5; i++)
    {
        if (--wp->timer_11[i] == 0)
        {
            wp->step_C[i]++;
            if (stcom29_D_0110[i][wp->step_C[i]][0] == 0xFF)
            {
                wp->step_C[i] = 0;
            }
            wp->timer_11[i] = stcom29_D_0110[i][wp->step_C[i]][1];
            unpack          = Model_poly_group_get((void *)wp->key_1C[stcom29_D_0250[i]], 0, 2);
            uv              = Quad_uv_get(unpack.c, stcom29_D_0248[i]);
            uv.b[0]         = stcom29_D_0234[i][0] + (stcom29_D_0110[i][wp->step_C[i]][0] << 6);
            uv.b[2]         = stcom29_D_0234[i][1] + (stcom29_D_0110[i][wp->step_C[i]][0] << 6);
            uv.b[4]         = stcom29_D_0234[i][2] + (stcom29_D_0110[i][wp->step_C[i]][0] << 6);
            uv.b[6]         = stcom29_D_0234[i][3] + (stcom29_D_0110[i][wp->step_C[i]][0] << 6);
            Quad_uv_set(unpack.c, stcom29_D_0248[i], uv.w);
        }
    }
}

// Refills the player's life in a scene once scene flag 0x60C is set.
void stcom29_heal_move(STCOM29_WORK *wp)
{
    stcom29_heal_tbl[wp->routine_0](wp);
}

void stcom29_heal_init(STCOM29_WORK *wp)
{
    wp->routine_0 = 1;
}

// Waits for scene flag 0x60C.
void stcom29_heal_wait(STCOM29_WORK *wp)
{
    if (Sce_flag_test(0x60C) != 0)
    {
        wp->routine_0 = 2;
        wp->routine_1 = 0;
    }
}

// Stops the player, shows a message, refills the life gauge, shows a second message, then
// hands control back once the camera settles.
void stcom29_heal(STCOM29_WORK *wp)
{
    STCOM_PL_WORK *pl;

    pl = &Player_work;
    switch (wp->routine_1)
    {
        case 0:
            if (Player_control_off(pl) != 0)
            {
                pl->routine_0 = 3;
                pl->routine_1 = 0;
                pl->value_C   = 0;
                wp->routine_1++;
            }
            break;
        case 1:
            pl->value_448 = 0x30;
            wp->routine_1++;
            break;
        case 2:
            if (!(Moji_flag & 0x8000000))
            {
                pl->value_448 = 0x36;
                MojiTaskExec(0, D_8008CE04, -1);
                Sound_call(0x13F, 0, 0);
                wp->routine_1++;
            }
            break;
        case 3:
            if (pl->flags_A7 < 0)
            {
                Pl_life_add(pl->value_B0, 0);
                wp->routine_1++;
            }
            break;
        case 4:
            if (!(Moji_flag & 0x8000000))
            {
                MojiTaskExec(0, D_8008CE3C, -1);
                pl->value_448 = 0x35;
                wp->routine_1++;
            }
            break;
        case 5:
            if (!(Moji_flag & 0x8000000) && pl->flags_A7 < 0)
            {
                wp->routine_1++;
            }
            break;
        case 6:
            if (Camera_moving() == 0)
            {
                Player_control_on(&Player_work);
                pl->target_184 = NULL;
                pl->flags_0 |= 2;
                wp->routine_0 = 1;
                wp->routine_1 = 0;
                Sce_flag_off(0x60C);
            }
            break;
    }
}
