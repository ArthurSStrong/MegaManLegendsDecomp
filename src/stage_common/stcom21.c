#include "common.h"
#include "stage_common/stcom21.h"

// Shared stage code, linked into 4 stages: r3_st05, r3_st08, r3_st10 and r3_st19. Each stage
// compiles this file against its own copy of the assembly (TARGET_ASM, set by tools/cc.sh); the
// names are the same in every stage's registry.
#define ASM_DIR TARGET_ASM "/stage_common/stcom21"

// A vending machine: 10 zenny buys a drink that refills life; a kick may give one free or
// break it.
void stcom21_vend(STCOM21_WORK *wp)
{
    STCOM_SVECTOR pos;

    Solid_set(wp, 7, 0, 0);
    stcom21_vend_tbl[wp->routine_0](wp);
    pos = wp->pos_38;
    pos.y -= 0x50;
    Set_Atari(&pos, 0x64, 0, 0x8000, wp->index_1, &wp->y_48, 0x4000000, 0, 0x80);
    wp->y_48.value = 0;
}

void stcom21_vend_init(STCOM21_WORK *wp)
{
    // Never used; it reserves retail's frame.
    STCOM_FIXED_VECTOR unused;
    sint32 flag;

    wp->flags_0 |= 6;
    Model_work_init(wp, &wp->variant_3);
    STCOM21_IDENTITY(&wp->matrix);
    RotMatrixY((wp->variant_3 & 3) << 10, &wp->matrix.m);
    wp->color_40   = 0x808080;
    wp->model_10   = stcom21_vend_model;
    wp->y_48.value = 0;
    wp->speed_4C   = 0;
    wp->routine_1  = 0;
    wp->routine_2  = 0;
    wp->value_C    = 0;
    wp->value_F    = 0x28;
    wp->routine_0++;
    if (STCOM_STAGE_ROOM == 0x105)
    {
        flag = wp->value_D * 4 + 0x681;
        if (Sce_flag_test(flag))
        {
            Close_WORK_50(wp);
        }
        else if (Sce_flag_test(flag + 1))
        {
            wp->value_C |= 2;
            wp->variant_3 += 4;
            Model_work_init(wp, &wp->variant_3);
        }
        else if (Sce_flag_test(flag + 2))
        {
            wp->value_C |= 1;
        }
    }
}

void stcom21_vend_wait(STCOM21_WORK *wp)
{
    if (stcom21_vend_front_check(wp, &Player_work))
    {
        if (stcom21_vend_talk_check(wp))
        {
            wp->routine_0++;
        }
        if (wp->y_48.value & 0x20000000)
        {
            wp->routine_1 = 3;
            wp->routine_2 = 0;
            wp->routine_0++;
        }
    }
    if (wp->y_48.value < 0)
    {
        wp->routine_1 = 3;
        wp->routine_2 = 0x1E;
        wp->value_C |= 8;
        wp->routine_0++;
    }
}

void stcom21_vend_use(STCOM21_WORK *wp)
{
    stcom21_vend_use_tbl[wp->routine_1](wp);
}

void stcom21_vend_message(STCOM21_WORK *wp)
{
    STCOM_PL_WORK *pl;

    pl = &Player_work;
    switch (wp->routine_2)
    {
        case 0:
            Player_work.routine_1 = 0x10;
            Player_work.value_0a  = 0;
            wp->routine_2++;
            break;
        case 1:
            MojiTaskExec(0, D_8008CACC[wp->value_B], -1);
            wp->routine_2++;
            break;
        case 2:
            if (!(Moji_flag & 0x8000000))
            {
                wp->routine_2++;
            }
            break;
        case 3:
            if (Camera_moving() == 0)
            {
                Player_work.target_184 = NULL;
                Player_work.routine_1  = 0;
                Player_work.value_0a   = 0;
                wp->routine_0          = 1;
                wp->routine_1          = 0;
                wp->routine_2          = 0;
            }
            break;
    }
    if (pl->routine_1 == 0xB || pl->routine_1 == 0xC)
    {
        MojiTaskKill();
        pl->target_184 = NULL;
        wp->routine_0  = 1;
        wp->routine_1  = 0;
        wp->routine_2  = 0;
    }
}

void stcom21_vend_buy(STCOM21_WORK *wp)
{
    STCOM_PL_WORK *pl;

    pl = &Player_work;
    switch (wp->routine_2)
    {
        case 0:
            pl->routine_1 = 0x10;
            pl->value_0a  = 0;
            wp->routine_2++;
            break;
        case 1:
            MojiTaskExec(0, D_8008CACC[wp->value_B], -1);
            wp->routine_2++;
            break;
        case 2:
            if (!(Moji_flag & 0x8000000))
            {
                if ((uint8)Moji_flag == 0)
                {
                    wp->routine_2++;
                }
                else
                {
                    wp->routine_2 = 0x14;
                }
            }
            break;
        case 3:
            if (Money_check(-10) < 0)
            {
                MojiTaskExec(0, D_8008CE08, -1);
                wp->routine_2 = 0x14;
            }
            else
            {
                Money_add(-10, 0, 0);
                wp->value_E = 0x10;
                wp->routine_2++;
            }
            break;
        case 4:
            pl->routine_1 = 0x11;
            pl->value_0a  = 0;
            wp->routine_2++;
            break;
        case 5:
            if (STCOM21_PL_A7(pl) < 0)
            {
                wp->value_E = 0x10;
                wp->routine_2++;
            }
            break;
        case 6:
            if (wp->value_E != 0)
            {
                wp->value_E--;
            }
            else
            {
                Sound_call2(0x13E, &wp->pos_38);
                wp->value_E = 0x10;
                wp->routine_2++;
            }
            break;
        case 7:
            if (wp->value_E != 0)
            {
                wp->value_E--;
            }
            else
            {
                MojiTaskExec(0, D_8008CE00, -1);
                wp->routine_2++;
            }
            break;
        case 8:
            if (!(Moji_flag & 0x8000000))
            {
                pl->routine_1 = 0x14;
                pl->value_0a  = 0;
                MojiTaskExec(0, D_8008CE04, -1);
                Sound_call2(0x13F, &wp->pos_38);
                Pl_life_add(pl->value_B0, 0);
                wp->value_C |= 1;
                stcom21_vend_flag_on(wp->value_D, 2);
                wp->routine_2++;
            }
            break;
        case 9:
            if (STCOM21_PL_A7(pl) < 0)
            {
                wp->routine_2++;
            }
            break;
        case 10:
            if (!(Moji_flag & 0x8000000))
            {
                MojiTaskExec(0, D_8008CE3C, -1);
                pl->routine_1 = 0x13;
                pl->value_0a  = 0;
                wp->routine_2++;
            }
            break;
        case 11:
            if (!(Moji_flag & 0x8000000) && STCOM21_PL_A7(pl) < 0)
            {
                wp->routine_2 = 0x14;
            }
            break;
        case 20:
            if (!(Moji_flag & 0x8000000))
            {
                wp->routine_2++;
            }
            break;
        case 21:
            if (Camera_moving() == 0)
            {
                pl->routine_1  = 0;
                pl->value_0a   = 0;
                pl->target_184 = NULL;
                wp->routine_0  = 1;
                wp->routine_1  = 0;
                wp->routine_2  = 0;
            }
            break;
    }
    if (pl->routine_1 == 0xB || pl->routine_1 == 0xC)
    {
        MojiTaskKill();
        pl->target_184 = NULL;
        wp->routine_0  = 1;
        wp->routine_1  = 0;
        wp->routine_2  = 0;
    }
}

void stcom21_vend_hit(STCOM21_WORK *wp)
{
    STCOM_PL_WORK *pl;
    STCOM_EFFECT *effect;

    pl = &Player_work;
    switch (wp->routine_2)
    {
        case 0:
            pl->flags_bd |= 0x40;
            if (Pl_shot_busy(pl) == 0)
            {
                pl->routine_1 = 0x10;
                pl->value_0a  = 0;
                wp->routine_2++;
            }
            break;
        case 1:
            if (wp->value_C & 2)
            {
                wp->routine_2 = 0x1E;
            }
            else if (rand() & 1)
            {
                stcom21_vend_flag_on(wp->value_D, 1);
                wp->routine_2 = 0x14;
                wp->value_C |= 2;
            }
            else
            {
                wp->value_E   = 0x10;
                wp->routine_2 = 10;
            }
            break;
        case 10:
            if (wp->value_E != 0)
            {
                wp->value_E--;
            }
            else
            {
                Sound_call2(0x13E, &wp->pos_38);
                wp->value_E = 0x10;
                wp->routine_2++;
            }
            break;
        case 11:
            if (wp->value_E != 0)
            {
                wp->value_E--;
            }
            else
            {
                MojiTaskExec(0, D_8008CE00, -1);
                wp->routine_2++;
            }
            break;
        case 12:
            if (!(Moji_flag & 0x8000000))
            {
                pl->routine_1 = 0x14;
                pl->value_0a  = 0;
                MojiTaskExec(0, D_8008CE04, -1);
                Sound_call2(0x13F, &wp->pos_38);
                Pl_life_add(pl->value_B0, 0);
                wp->routine_2++;
            }
            break;
        case 13:
            if (STCOM21_PL_A7(pl) < 0)
            {
                wp->routine_2++;
            }
            break;
        case 14:
            if (!(Moji_flag & 0x8000000))
            {
                Sce_light_add(0);
                wp->routine_2++;
            }
            break;
        case 15:
            if (!(Moji_flag & 0x8000000))
            {
                MojiTaskExec(0, D_8008CE3C, -1);
                pl->routine_1 = 0x13;
                pl->value_0a  = 0;
                wp->routine_2++;
            }
            break;
        case 16:
            if (!(Moji_flag & 0x8000000) && STCOM21_PL_A7(pl) < 0)
            {
                pl->routine_1 = 0x10;
                pl->value_0a  = 0;
                wp->routine_2++;
            }
            break;
        case 17:
            if (!(Moji_flag & 0x8000000) && STCOM21_PL_A7(pl) < 0)
            {
                wp->routine_2 = 0x28;
            }
            break;
        case 20:
            wp->variant_3 += 4;
            Model_work_init(wp, &wp->variant_3);
            STCOM21_IDENTITY(&wp->matrix);
            RotMatrixY((wp->variant_3 & 3) << 10, &wp->matrix.m);
            wp->value_B = 0;
            wp->routine_2++;
            Sound_call2(0xA4, &wp->pos_38);
            break;
        case 21:
            effect = Open_EFFECT_WORK();
            if (effect != NULL)
            {
                effect->flags_0   = 1;
                effect->kind_2    = 0xB;
                effect->variant_3 = 3;
                effect->timer_E   = 0x14;
                effect->value_C   = 0;
                effect->value_D   = 0;
                effect->value_F   = 8;
                effect->sprite.pos_8[0] =
                    wp->pos_38.x + stcom21_vend_smoke_tbl[wp->variant_3 & 3].x;
                effect->sprite.pos_8[1] = wp->pos_38.y - 0xC8;
                effect->sprite.pos_8[2] = wp->pos_38.z;
                effect->routine_1       = 4;
                wp->value_E             = 2;
                wp->routine_2++;
            }
            break;
        case 22:
            if (wp->value_E != 0)
            {
                wp->value_E--;
            }
            else if (wp->value_B < 8)
            {
                wp->routine_2--;
                wp->value_B++;
            }
            else
            {
                wp->value_B = 0;
                wp->routine_2++;
            }
            break;
        case 23:
            MojiTaskExec(0, D_8008CE0C, -1);
            wp->routine_2++;
            break;
        case 24:
            if (!(Moji_flag & 0x8000000))
            {
                wp->routine_2 = 0x28;
            }
            break;
        case 30:
            effect = Open_EFFECT_WORK();
            if (effect != NULL)
            {
                effect->flags_0           = 5;
                effect->kind_2            = 0x80;
                effect->variant_3         = 0x41;
                effect->sprite.position   = wp->pos_38;
                effect->sprite.position.y = wp->pos_38.y - 0x64;
                wp->u_44.effect           = effect;
                wp->flags_0 &= ~2;
                if (!(wp->value_C & 8))
                {
                    stcom21_vend_break_count();
                }
                Sce_light_add(1);
                wp->value_C |= 4;
                stcom21_vend_flag_on(wp->value_D, 0);
                wp->routine_2++;
            }
            break;
        case 31:
            effect = wp->u_44.effect;
            if ((sint8)effect->flags_0 == 0)
            {
                wp->routine_2 = 0x28;
            }
            break;
        case 40:
            if (Camera_moving() == 0)
            {
                if (!(wp->value_C & 8))
                {
                    pl->routine_1  = 0x10;
                    pl->value_0a   = 2;
                    pl->target_184 = NULL;
                    pl->flags_bd &= ~0x40;
                }
                wp->routine_2++;
            }
            break;
        case 41:
            wp->routine_0 = 1;
            wp->routine_1 = 0;
            wp->routine_2 = 0;
            if (wp->value_C & 4)
            {
                Close_WORK_50(wp);
            }
            break;
    }
    if (pl->routine_1 == 0xB || pl->routine_1 == 0xC)
    {
        MojiTaskKill();
        pl->target_184 = NULL;
        pl->flags_bd &= ~0x40;
        wp->routine_0 = 1;
        wp->routine_1 = 0;
        wp->routine_2 = 0;
        if (wp->value_C & 4)
        {
            Close_WORK_50(wp);
        }
    }
}

// Whether the player faces the vending machine from the front.
uint8 stcom21_vend_front_check(STCOM21_WORK *wp, STCOM_PL_WORK *pl)
{
    return Angle_range_check2(&wp->pos_38, ((wp->variant_3 << 10) - 0x400) & 0xC00, 0x1FF,
                              &pl->position, pl->rotation.y, 0x1FF);
}

// Whether the player examines the vending machine: picks the conversation.
sint32 stcom21_vend_talk_check(STCOM21_WORK *wp)
{
    sint32 message;

    if (Player_work.target_184 == wp)
    {
        if (wp->value_C & 2)
        {
            wp->routine_1 = 0;
            message       = 0xCB;
        }
        else if (wp->value_C & 1)
        {
            wp->routine_1 = 1;
            message       = 0xCA;
        }
        else
        {
            wp->routine_1 = 2;
            message       = 0xCC;
        }
        wp->value_B = message;
        return 1;
    }
    return 0;
}

// Steps the two-flag counter in scene flags 0x220 and 0x221.
void stcom21_vend_break_count(void)
{
    if (Sce_flag_test(0x220) && Sce_flag_test(0x221))
    {
        Sce_flag_off(0x220);
        Sce_flag_off(0x221);
        Sce_flag_select(0x2E0);
    }
    else if (!Sce_flag_test(0x220) && Sce_flag_test(0x221))
    {
        Sce_flag_on(0x220);
    }
    else if (Sce_flag_test(0x220) && !Sce_flag_test(0x221))
    {
        Sce_flag_off(0x220);
        Sce_flag_on(0x221);
    }
    else
    {
        Sce_flag_on(0x220);
    }
}

void stcom21_vend_flag_on(uint8 index, uint8 n)
{
    if (STCOM_STAGE_ROOM == 0x105)
    {
        Sce_flag_on(index * 4 + (0x681 + n));
    }
}
