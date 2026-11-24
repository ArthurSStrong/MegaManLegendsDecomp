#include "common.h"
#include "st1a.h"

void StXX_eve19_move(STAGE_OBJ *wp)
{
    eve19_move_tbl[wp->routine_0](wp);
}

void eve19_main(STAGE_OBJ *wp)
{
    eve19_main_tbl[wp->variant_3](wp);
}

void eve19_end(STAGE_OBJ *wp)
{
    Close_EVE_WORK(wp);
}

void eve19_main_area_00(STAGE_EV *wp)
{
    switch (wp->routine_1)
    {
        case 0:
            if (Sce_flag_test(0x56) != 0)
            {
                wp->routine_0 = 1;
            }
            else
            {
                wp->routine_1++;
            }
            break;
        case 1:
            if (STAGE_GAME_U16(0x17A) & 0x40)
            {
                wp->routine_1++;
            }
            break;
        case 2:
            if (!(Moji_flag & 0x8000000))
            {
                MojiTaskExec2(0, 1);
                STAGE_GAME_U16(0x17A) &= ~0x40;
                STAGE_PL_BYTE(0xBD) |= 0x40;
                wp->routine_1++;
            }
            break;
        case 3:
            if (!(Moji_flag & 0x8000000))
            {
                if (Sce_flag_test(0x56) != 0)
                {
                    Sound_call(0x208, 0, 0);
                    wp->routine_2 = 0;
                    wp->key_1C    = 0;
                    wp->routine_1++;
                }
                else
                {
                    STAGE_PL_BYTE(0xBD) &= ~0x40;
                    wp->routine_1 = 1;
                }
            }
            break;
        case 4:
            if (check_swing_camera(wp) != 0)
            {
                Sound_call(0x209, 0, 0);
                wp->routine_1++;
            }
            break;
        case 5:
            if (!(Moji_flag & 0x8000000))
            {
                STAGE_GAME_S16(0x17C) = 1;
                MojiTaskExec2(0, 0x1D);
                wp->routine_1++;
            }
            break;
        case 6:
            if (!(Moji_flag & 0x8000000))
            {
                STAGE_PL_BYTE(0xBD) &= ~0x40;
                wp->routine_0 = 1;
            }
            break;
    }
}

void eve19_main_area_01(STAGE_OBJ *wp)
{
    ST1A_AREA area = {-2, -2, 4, 1};

    if (Sce_flag_test(0x35) == 0)
    {
        if (check_block(wp, &area) == 0)
        {
            return;
        }
        Sce_flag_on(0x604);
    }
    STAGE_GAME_U16(0x17A) |= 1;
    wp->routine_0 = 1;
}

void eve19_main_area_02(STAGE_OBJ *wp)
{
    ST1A_AREA area = {0x0A, -6, 3, 1};

    switch (wp->routine_1)
    {
        case 0:
            if (Sce_flag_test(0x6D) != 0)
            {
                wp->routine_0 = 1;
            }
            else if (check_block(wp, &area) != 0)
            {
                wp->routine_1++;
            }
            break;
        case 1:
            if (!(Moji_flag & 0x8000000))
            {
                if (check_block(wp, &area) != 0)
                {
                    wp->routine_1++;
                }
                else
                {
                    wp->routine_1 = 0;
                }
            }
            break;
        case 2:
            MojiTaskExec2(0, 0x1B);
            STAGE_PL_BYTE(0xBD) |= 0x40;
            Sce_flag_on(0x6D);
            wp->routine_1++;
            break;
        case 3:
            if (!(Moji_flag & 0x8000000))
            {
                STAGE_PL_BYTE(0xBD) &= ~0x40;
                wp->routine_0 = 1;
            }
            break;
    }
}

void eve19_main_area_05(STAGE_OBJ *wp)
{
    ST1A_AREA area = {-5, -22, 1, 1};

    switch (wp->routine_1)
    {
        case 0:
            if ((STAGE_GAME_U16(0x17A) & 0x8) || Sce_flag_test(0x56) != 0)
            {
                STAGE_GAME_U16(0x17A) |= 0x8;
                wp->routine_0 = 1;
            }
            else if (check_block(wp, &area) != 0)
            {
                wp->routine_1++;
            }
            break;
        case 1:
            if (!(Moji_flag & 0x800000))
            {
                if (check_block(wp, &area) != 0)
                {
                    wp->routine_1++;
                }
                else
                {
                    wp->routine_1 = 0;
                }
            }
            break;
        case 2:
            MojiTaskExec2(4, 6);
            STAGE_GAME_U16(0x17A) |= 0x8;
            wp->routine_1++;
            break;
        case 3:
            if (!(Moji_flag & 0x800000))
            {
                wp->routine_0 = 1;
            }
            break;
    }
}

void eve19_main_area_06(STAGE_OBJ *wp)
{
    ST1A_AREA area = {1, -6, 1, 1};

    switch (wp->routine_1)
    {
        case 0:
            if ((STAGE_GAME_U16(0x17A) & 0x10) || Sce_flag_test(0x56) != 0)
            {
                STAGE_GAME_U16(0x17A) |= 0x10;
                wp->routine_0 = 1;
            }
            else if (check_block(wp, &area) != 0)
            {
                wp->routine_1++;
            }
            break;
        case 1:
            if (!(Moji_flag & 0x800000))
            {
                if (check_block(wp, &area) != 0)
                {
                    wp->routine_1++;
                }
                else
                {
                    wp->routine_1 = 0;
                }
            }
            break;
        case 2:
            MojiTaskExec2(4, 8);
            STAGE_GAME_U16(0x17A) |= 0x10;
            wp->routine_1++;
            break;
        case 3:
            if (!(Moji_flag & 0x800000))
            {
                wp->routine_0 = 1;
            }
            break;
    }
}

void eve19_main_area_09(STAGE_OBJ *wp)
{
    ST1A_AREA area = {-5, 0x24, 1, 1};

    switch (wp->routine_1)
    {
        case 0:
            if (STAGE_GAME_U16(0x17A) & 0x20)
            {
                wp->routine_0 = 1;
            }
            else if (Sce_flag_test(0x56) != 0)
            {
                STAGE_GAME_U16(0x17A) |= 0x24;
                wp->routine_0 = 1;
            }
            else if (check_block(wp, &area) != 0)
            {
                wp->routine_1++;
            }
            break;
        case 1:
            if (!(Moji_flag & 0x800000))
            {
                if (check_block(wp, &area) != 0)
                {
                    wp->routine_1++;
                }
                else
                {
                    wp->routine_1 = 0;
                }
            }
            break;
        case 2:
            MojiTaskExec2(4, 7);
            STAGE_GAME_U16(0x17A) |= 0x20;
            wp->routine_1++;
            break;
        case 3:
            if (!(Moji_flag & 0x800000))
            {
                wp->routine_0 = 1;
            }
            break;
    }
}

void eve19_main_dummy(STAGE_OBJ *wp)
{
    wp->routine_0 = 1;
}

// The player's cell (position >> 9 fits in a byte) is held in a sint8: the (uint16) subtraction
// then extends it as retail does, and the nested ifs set the zero result before the first test.
sint32 check_block(void *wp, ST1A_AREA *area)
{
    sint8 x;
    sint8 z;

    x = Player_work.position.x >> 9;
    if ((uint16)(x - area->x_0) < area->w_2)
    {
        z = Player_work.position.z >> 9;
        if ((uint16)(z - area->z_1) < area->d_3)
        {
            return 1;
        }
    }
    return 0;
}

sint32 check_swing_camera(STAGE_EV *wp)
{
    sint32 ret = 0;

    switch (wp->routine_2)
    {
        case 0:
            Swing_Camera2(4, 0x200, 0x400, &Player_work.position);
            wp->routine_2++;
            break;
        case 1:
            if (D_800A395F == 0)
            {
                if (wp->key_1C >= 0x5A)
                {
                    ret = 1;
                }
                else
                {
                    wp->routine_2 = 0;
                }
            }
            break;
    }
    // A do-while(0) (a macro body?) keeps the result copy after the increment, as in retail.
    do
    {
        wp->key_1C++;
    } while (0);
    return ret;
}

void func_8012C2EC(STAGE_EV *wp)
{
    STAGE_OBJ *obj;
    sint32 i;

    for (i = 0x1F; i >= 0; i--)
    {
        obj = Open_WORK_50_index(i);
        if (obj != NULL)
        {
            obj->flags_0 = 1;
            obj->kind_2  = 0;
            break;
        }
    }
    Close_EVE_WORK(wp);
}
