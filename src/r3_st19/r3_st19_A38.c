#include "common.h"
#include "st19.h"

void func_80100A38(STAGE_ROOM *wp)
{
    if (wp->routine_4 == 0)
    {
        Sce_slot_open_list(D_80117190);
        wp->routine_4 = 0xFF;
    }
}

void func_80100A84(STAGE_ROOM *wp)
{
    if (wp->routine_4 == 0)
    {
        Sce_slot_open_list(D_801171B8);
        wp->routine_4 = 0xFF;
    }
}

void func_80100AD0(STAGE_ROOM *wp)
{
    if (wp->routine_4 == 0)
    {
        Sce_slot_open_list(D_80117198);
        Sce_slot_close(0xA);
        Sce_slot_close(0xB);
        wp->routine_4 = 0xFF;
    }
}

void func_80100B2C(STAGE_ROOM *wp)
{
    switch (wp->routine_4)
    {
        case 0:
            if (Sce_flag_test(0x52))
            {
                wp->routine_4 = 0xFF;
            }
            else if (Sce_flag_test(0x34))
            {
                Sce_slot_close_all();
                func_8001E460(0, 1, 0);
                wp->routine_4++;
            }
            break;
        case 1:
            if (Sce_flag_test(0x52))
            {
                Player_work.flags_bd |= 0x40;
                wp->routine_4++;
            }
            break;
        case 2:
            Sound_fade_start(0x10);
            func_80017C30(0xD);
            MojiTaskKill();
            wp->routine_4++;
            break;
        case 3:
            if (D_80098A72 == 0 && Sound_fade_flags == 0)
            {
                Sce_event_start(0x42);
                func_8001E460(0, 0, 0);
                wp->routine_4++;
            }
            break;
        case 4:
            wp->routine_4 = 0xFF;
            break;
    }
}

void func_80100C6C(STAGE_ROOM *wp)
{
    if ((wp->routine_4 == 0) && (Sce_flag_test(0x34) == 0))
    {
        Sce_flag_on(0x34);
        Sce_event_start(0x41);
        wp->routine_4++;
    }
}

void func_80100CD0(STAGE_ROOM *wp)
{
    if (wp->routine_4 == 0)
    {
        if (Sce_flag_test(0x52))
        {
            Sce_slot_open_list(D_801171C0);
        }
        else
        {
            Sce_slot_open_list(D_801171B8);
        }
        wp->routine_4 = 0xFF;
    }
}

void func_80100D34(STAGE_ROOM *wp)
{
    if (wp->routine_4 == 0)
    {
        Sce_slot_open_list(D_80117198);
        if (Sce_flag_test(0x52))
        {
            Sce_slot_close(0);
            Sce_slot_close(1);
            Sce_slot_close(6);
            Sce_slot_close(7);
            Sce_slot_close(8);
            Sce_slot_close(9);
        }
        else
        {
            Sce_slot_close(10);
            Sce_slot_close(11);
            Sce_slot_close(6);
        }
        wp->routine_4 = 0xFF;
    }
}

void func_80100DDC(STAGE_ROOM *wp)
{
    if (wp->routine_4 == 0)
    {
        Sce_slot_open_list(D_801171C8);
        wp->routine_4 = 0xFF;
    }
}

void func_80100E28(STAGE_ROOM *wp)
{
    if (wp->routine_4 == 0)
    {
        Sce_slot_open_list(D_801171A0);
        wp->routine_4 = 0xFF;
    }
}
