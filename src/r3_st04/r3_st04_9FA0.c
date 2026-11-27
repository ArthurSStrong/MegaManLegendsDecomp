#include "common.h"
#include "st04.h"

void func_80109FA0(STAGE_WORK *wp)
{
    D_80111D14[wp->routine_0](wp);
}

void func_80109FDC(STAGE_OBJ *wp)
{
    wp->routine_0 = 1;
}

void func_80109FE8(STAGE_OBJ50 *wp)
{
    if (Message_event[0] != 0 && D_800987B5 < 0x10 && (wp->u_C.b.lo = D_80111CE0[D_800987B5]) != 0)
    {
        wp->routine_0 = 2;
        wp->routine_1 = 0;
        wp->value_F   = 0;
    }
}

void func_8010A044(STAGE_OBJ50 *wp)
{
    STCOM_PL_WORK *pl;

    pl = &Player_work;
    switch (wp->routine_1)
    {
        case 0:
            if (Player_control_off(pl))
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
                if (Moji_flag & 0xFF)
                {
                    wp->routine_1 = 10;
                }
                else
                {
                    Sound_fade_start(0x10);
                    wp->routine_1++;
                }
            }
            break;
        case 3:
            if (Sound_fade_flags == 0)
            {
                MojiTaskExec2(0, 0xB);
                Sound_work.volume_6 = D_80111D0C[wp->u_C.b.lo];
                Cd_xa_play_id(D_80111CF0[wp->u_C.b.lo]);
                wp->routine_1++;
            }
            break;
        case 4:
            if (Sce_cd_reading())
            {
                wp->routine_1++;
            }
            break;
        case 5:
            if (D_800C0C26 != 0 || !(Moji_flag & 0x8000000) || !Sce_cd_reading())
            {
                pl->value_448 = 0x31;
                Moji_task_kill(0);
                Sound_fade_start(0x40);
                wp->routine_1++;
            }
            break;
        case 6:
            if (Sound_fade_flags == 0)
            {
                Sound_work.volume_6 = 0x7F;
                wp->routine_1++;
            }
            break;
        case 7:
            Sound_fade_start(0x20);
            wp->routine_1 = 10;
            break;
        case 10:
            if (!Camera_moving())
            {
                Player_control_on(&Player_work);
                pl->target_184 = NULL;
                pl->flags_0 |= 2;
                wp->routine_0 = 1;
                wp->routine_1 = 0;
            }
            break;
    }
}

void func_8010A248(STAGE_WORK *wp)
{
    D_80111D20[wp->routine_0](wp);
}

void func_8010A284(ST04_EV *wp)
{
    ST04_SLOT *slot;

    slot = Sce_slot_open(D_80111D2C);
    slot->flags_6 &= ~0x20;
    wp->slot_1C   = slot;
    wp->routine_0 = 1;
}

void func_8010A2D4(ST04_EV *wp)
{
    if (wp->slot_1C->routine_2 == 0 && func_8010A484(wp->slot_1C))
    {
        wp->routine_0 = 2;
        wp->routine_1 = 0;
    }
}

void func_8010A32C(ST04_EV *wp)
{
    STCOM_PL_WORK *pl;

    pl = &Player_work;
    switch (wp->routine_1)
    {
        case 0:
            if (!(Moji_flag & 0x0F800000))
            {
                wp->routine_1++;
            }
            break;
        case 1:
            if (Player_control_off(pl))
            {
                pl->routine_0 = 3;
                pl->routine_1 = 0;
                pl->value_C   = 0;
                wp->routine_1++;
            }
            break;
        case 2:
            pl->value_448 = 0x30;
            wp->routine_1++;
            break;
        case 3:
            pl->value_448 = 0x35;
            MojiTaskExec2(0, 0xD2);
            wp->routine_1++;
            break;
        case 4:
            if (pl->flags_A7 < 0 && !(Moji_flag & 0x08000000))
            {
                MojiTaskExec2(0, 0xD3);
                wp->routine_1++;
            }
            break;
        case 5:
            if (!(Moji_flag & 0x08000000))
            {
                wp->routine_1++;
            }
            break;
        case 6:
            if (!Camera_moving())
            {
                Player_control_on(pl);
                pl->flags_0 |= 2;
                Sce_light_add(0x1A);
                wp->routine_1++;
            }
            break;
        case 7:
        case 8:
            break;
    }
}

uint8 func_8010A484(ST04_SLOT *slot)
{
    if ((D_80111D40.x >= slot->position.x && slot->position.x >= D_80111D48.x) &&
        (D_80111D40.y >= slot->position.y && slot->position.y >= D_80111D48.y) &&
        (D_80111D40.z >= slot->position.z && slot->position.z >= D_80111D48.z))
    {
        return 1;
    }
    return 0;
}
