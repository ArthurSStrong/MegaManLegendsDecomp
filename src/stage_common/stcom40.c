#include "common.h"
#include "stage_common/stcom40.h"

// Shared stage code, linked into 6 stages: r3_st04, r3_st05, r3_st08, r3_st0d, r3_st10 and
// r3_st19. Each stage compiles this file against its own copy of the assembly (TARGET_ASM, set by
// tools/cc.sh); the names are the same in every stage's registry. In r3_st05, r3_st08, r3_st10
// and r3_st19 it follows stcom21 (the vending machine); r3_st04 and r3_st0d link it alone.
#define ASM_DIR TARGET_ASM "/stage_common/stcom40"

// The chest.
void stcom40_chest(STCOM21_WORK *wp)
{
    stcom40_chest_tbl[wp->routine_0](wp);
}

void stcom40_chest_init(STCOM21_WORK *wp)
{
    // Never used; it reserves retail's frame.
    STCOM_FIXED_VECTOR unused;

    wp->flags_0 |= 6;
    if (wp->variant_3 == 0)
    {
        wp->u_44.flag  = wp->pos_38.pad;
        wp->pos_38.pad = 0;
    }
    Model_work_init(wp, &wp->variant_3);
    STCOM21_IDENTITY(&wp->matrix);
    RotMatrixY(wp->value_F << 6, &wp->matrix.m);
    wp->color_40   = 0x808080;
    wp->model_10   = stcom40_chest_model;
    wp->y_48.value = 0;
    wp->speed_4C   = 0;
    wp->routine_0++;
}

void stcom40_chest_main(STCOM21_WORK *wp)
{
    STCOM_SVECTOR pos;

    stcom40_chest_type_tbl[wp->variant_3](wp);
    if (wp->variant_3 != 2)
    {
        Solid_set(wp, 7, 0, 0);
        pos = wp->pos_38;
        pos.y -= 0x2D;
        Set_Atari(&pos, 0x28, 0, 0x8000, wp->index_1, &wp->y_48, 0x4000000, 0, 0x80);
        wp->y_48.value = 0;
    }
}

// Vanishes in a puff.
void stcom40_chest_erase(STCOM21_WORK *wp)
{
    STCOM_EFFECT *effect;

    effect = Open_EFFECT_WORK();
    if (effect != NULL)
    {
        effect->flags_0           = 5;
        effect->kind_2            = 0x80;
        effect->variant_3         = 0x41;
        effect->sprite.position   = wp->pos_38;
        effect->sprite.position.y = wp->pos_38.y - 0x28;
        Close_WORK_50(wp);
    }
}

void stcom40_chest_closed(STCOM21_WORK *wp)
{
    stcom40_chest_closed_tbl[wp->routine_1](wp);
}

void stcom40_chest_wait(STCOM21_WORK *wp)
{
    if (Player_work.target_184 == wp)
    {
        wp->routine_1 = 1;
        wp->routine_2 = 0;
    }
    else if (wp->y_48.value & 0x20000000)
    {
        if (stcom40_chest_item_check(wp))
        {
            wp->routine_1 = 2;
        }
        else
        {
            stcom40_lid_spawn(wp);
            Sound_call2(0x13D, &wp->pos_38);
        }
    }
    else if (wp->y_48.value < 0)
    {
        wp->routine_1 = 0;
        wp->routine_0++;
    }
}

// Opened by talking to it: hands over the contents.
void stcom40_chest_open(STCOM21_WORK *wp)
{
    STCOM_PL_WORK *pl;
    uint32 zenny;

    pl = &Player_work;
    switch (wp->routine_2)
    {
        case 0:
            if (Player_control_off(pl))
            {
                Player_work.routine_0 = 3;
                Player_work.routine_1 = 0;
                Player_work.value_448 = 0x30;
                Player_work.value_C   = 0;
                wp->routine_2++;
            }
            break;
        case 1:
            if (!stcom40_chest_item_check(wp))
            {
                Moji_param_exec(0, 2, 0, 0);
            }
            else if (wp->value_D == 0)
            {
                Message_event_set(&wp->pos_38, 2, 0, wp->value_E);
            }
            else if (wp->value_D != 1)
            {
                if (wp->value_D & 0x80)
                {
                    zenny = (((wp->value_D & 0x7F) << 8) | (uint8)wp->value_E) * 10;
                    Moji_param_exec(0, 1, zenny / 10, 0);
                    Sce_flag_on(wp->value_C + 0x400);
                }
                else if (wp->value_E != -1)
                {
                    Moji_param_exec(0, 0, (uint8)wp->value_E, 0);
                    Sce_flag_on(wp->value_C + 0x400);
                }
                else
                {
                    Moji_param_exec(0, 2, 0, 0);
                }
            }
            wp->routine_2++;
            break;
        case 2:
            if (!(Moji_flag & 0x8000000))
            {
                Player_control_on(pl);
                Player_work.target_184 = NULL;
                pl->flags_0 |= 2;
                wp->routine_1 = 0;
                wp->routine_2 = 0;
            }
            break;
    }
}

// Opened by a hit: the lid flies off.
void stcom40_chest_hit_open(STCOM21_WORK *wp)
{
    switch (wp->routine_2)
    {
        case 0:
            if (wp->value_D != 1)
            {
                Sound_call2(0x13C, &wp->pos_38);
            }
            wp->value_F = (wp->value_B + 0x10) & 0x3F;
            wp->routine_2++;
            // fall through
        case 1:
            wp->routine_2++;
            // fall through
        case 2:
            if (wp->value_F != wp->value_B)
            {
                STCOM21_IDENTITY(&wp->matrix);
                RotMatrixZ((sint16)stcom40_lid_angle_tbl[wp->value_B & 7] >> 2, &wp->matrix.m);
                RotMatrixY(wp->value_B << 6, &wp->matrix.m);
                wp->value_B = (wp->value_B + 1) & 0x3F;
            }
            else if (wp->routine_2 == 2)
            {
                wp->routine_2++;
            }
            break;
        case 3:
            if (wp->value_D == 1 && wp->value_E != -1)
            {
                Message_event_set(&wp->pos_38, 2, 0, wp->value_E);
            }
            wp->routine_2++;
            break;
        case 4:
            if (!(Moji_flag & 0x8000000))
            {
                wp->routine_1  = 0;
                wp->routine_2  = 0;
                wp->y_48.value = 0;
            }
            break;
    }
}

// Spawns the lid (variant 2) and shows the chest open.
void stcom40_lid_spawn(STCOM21_WORK *wp)
{
    STCOM21_WORK *lid;

    lid = Open_WORK_50();
    if (lid != NULL)
    {
        lid->flags_0    = wp->flags_0;
        lid->variant_3  = 2;
        lid->kind_2     = wp->kind_2;
        lid->pos_38     = wp->pos_38;
        lid->pos_38.y   = wp->pos_38.y - 0x50;
        lid->routine_1  = 0;
        lid->value_B    = 0;
        lid->u_44.chest = wp;
        Model_work_init(lid, &lid->variant_3);
        wp->variant_3 = 1;
        Model_work_init(wp, &wp->variant_3);
        STCOM21_IDENTITY(&wp->matrix);
        RotMatrixY(wp->value_F << 6, &wp->matrix.m);
        wp->routine_1 = 0;
    }
}

void stcom40_chest_opened(STCOM21_WORK *wp)
{
    STCOM21_IDENTITY(&wp->matrix);
    RotMatrixZ((sint16)stcom40_lid_angle_tbl[wp->value_B & 7] >> 2, &wp->matrix.m);
    RotMatrixY(wp->value_B << 6, &wp->matrix.m);
}

// The lid: rises, then falls back onto its chest.
void stcom40_lid(STCOM21_WORK *wp)
{
    STCOM21_WORK *chest;
    sint16 top;

    chest = wp->u_44.chest;
    switch (wp->routine_1)
    {
        case 0:
            wp->speed_4C   = 0x200000;
            wp->routine_1  = 1;
            wp->y_48.value = wp->pos_38.y << 16;
            // fall through
        case 1:
            if (chest->value_D == 1 && chest->value_E == 0x10)
            {
                stcom40_chest_em_spawn(chest);
            }
            wp->routine_1++;
            // fall through
        case 2:
            top = chest->pos_38.y - 0x50;
            wp->y_48.value -= wp->speed_4C;
            wp->speed_4C -= 0x40000;
            wp->pos_38.y = wp->y_48.words.integer;
            if (top < wp->pos_38.y)
            {
                chest->variant_3  = 0;
                chest->routine_0  = 1;
                chest->routine_1  = 0;
                chest->y_48.value = 0;
                chest->speed_4C   = 0;
                Model_work_init(chest, &chest->variant_3);
                STCOM21_IDENTITY(&chest->matrix);
                RotMatrixX(0, &chest->matrix.m);
                RotMatrixY(chest->value_B << 6, &chest->matrix.m);
                RotMatrixZ(0, &chest->matrix.m);
                Close_WORK_50(wp);
            }
            else
            {
                STCOM21_IDENTITY(&wp->matrix);
                RotMatrixZ((sint16)stcom40_lid_angle_tbl[chest->value_B & 7], &wp->matrix.m);
                RotMatrixY(wp->value_F << 6, &wp->matrix.m);
                chest->value_B++;
            }
            break;
    }
}

// Whether the chest still has something to give.
uint8 stcom40_chest_item_check(STCOM21_WORK *wp)
{
    sint32 flag;

    flag = wp->u_44.flag;
    if (wp->value_D == 1 && wp->value_E == 0x10)
    {
        return 0;
    }
    if ((flag == 0 || Sce_flag_test(flag)) && !Sce_flag_test(wp->value_C + 0x400) &&
        wp->value_E != -1)
    {
        return 1;
    }
    return 0;
}

// Three creatures jump out of the chest.
void stcom40_chest_em_spawn(STCOM21_WORK *wp)
{
    STCOM40_OBJ *obj;
    uint8 i;

    for (i = 0; i < 3; i++)
    {
        obj = Open_EM_WORK();
        if (obj != NULL)
        {
            obj->flags_0          = 1;
            obj->kind_2           = 0x10;
            obj->handler_4        = 1;
            obj->position         = wp->pos_38;
            obj->rotation.y       = i * 0x600;
            obj->position_fixed.x = obj->position.x << 16;
            obj->position_fixed.y = obj->position.y << 16;
            obj->position_fixed.z = obj->position.z << 16;
        }
    }
}

// The box.
void stcom40_box(STCOM21_WORK *wp)
{
    STCOM_SVECTOR pos;

    Solid_set(wp, 7, 0, 0);
    stcom40_box_tbl[wp->routine_0](wp);
    pos = wp->pos_38;
    pos.y -= 0x2D;
    Set_Atari(&pos, 0x32, 0, 0x8000, wp->index_1, &wp->y_48, 0x4000000, 0, 0x80);
    wp->y_48.value = 0;
}

void stcom40_box_init(STCOM21_WORK *wp)
{
    // Never used; it reserves retail's frame.
    STCOM_FIXED_VECTOR unused;

    wp->flags_0 |= 6;
    wp->u_44.flag  = wp->pos_38.pad;
    wp->pos_38.pad = 0;
    Model_work_init(wp, &wp->variant_3);
    STCOM21_IDENTITY(&wp->matrix);
    RotMatrixY(wp->value_F << 10, &wp->matrix.m);
    wp->color_40   = 0x808080;
    wp->model_10   = stcom40_box_model;
    wp->y_48.value = 0;
    wp->speed_4C   = 0;
    wp->routine_0++;
}

void stcom40_box_wait(STCOM21_WORK *wp)
{
    if (Player_work.target_184 == wp)
    {
        wp->routine_0++;
    }
    if (wp->y_48.value < 0)
    {
        wp->routine_0 = 3;
    }
}

void stcom40_box_open(STCOM21_WORK *wp)
{
    STCOM_PL_WORK *pl;
    sint32 zenny;

    pl = &Player_work;
    switch (wp->routine_1)
    {
        case 0:
            if (Player_control_off(pl))
            {
                Player_work.routine_0 = 3;
                Player_work.routine_1 = 0;
                Player_work.value_448 = 0x30;
                Player_work.value_C   = 0;
                wp->routine_1++;
            }
            break;
        case 1:
            if (!stcom40_box_item_check(wp))
            {
                Moji_param_exec(1, 2, 0, 0);
            }
            else if (wp->value_D == 0)
            {
                Message_event_set(&wp->pos_38, 2, 0, wp->value_E);
            }
            else if (wp->value_D != 1)
            {
                if (wp->value_D & 0x80)
                {
                    zenny = (((wp->value_D & 0x7F) << 8) | (uint8)wp->value_E) * 10;
                    Moji_param_exec(1, 1, zenny / 10, 0);
                    Sce_flag_on(wp->value_C + 0x400);
                }
                else if (wp->value_E != -1)
                {
                    Moji_param_exec(1, 0, (uint8)wp->value_E, 0);
                    Sce_flag_on(wp->value_C + 0x400);
                }
                else
                {
                    Moji_param_exec(1, 2, 0, 0);
                }
            }
            wp->routine_1++;
            break;
        case 2:
            if (!(Moji_flag & 0x8000000))
            {
                Player_control_on(pl);
                Player_work.target_184 = NULL;
                pl->flags_0 |= 2;
                wp->routine_0 = 1;
                wp->routine_1 = 0;
            }
            break;
    }
}

// Vanishes in a puff.
void stcom40_box_erase(STCOM21_WORK *wp)
{
    STCOM_EFFECT *effect;

    effect = Open_EFFECT_WORK();
    if (effect != NULL)
    {
        effect->flags_0           = 5;
        effect->kind_2            = 0x80;
        effect->variant_3         = 0x41;
        effect->sprite.position   = wp->pos_38;
        effect->sprite.position.y = wp->pos_38.y - 0x28;
        Close_WORK_50(wp);
    }
}

// Whether the box still has something to give.
uint8 stcom40_box_item_check(STCOM21_WORK *wp)
{
    if ((wp->u_44.flag == 0 || Sce_flag_test(wp->u_44.flag)) &&
        !Sce_flag_test(wp->value_C + 0x400) && wp->value_E != -1)
    {
        return 1;
    }
    return 0;
}
