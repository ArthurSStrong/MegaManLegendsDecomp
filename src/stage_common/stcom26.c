#include "common.h"
#include "stage_common/stcom26.h"

// Shared stage code, linked into 3 stages: r3_st05, r3_st06 and r3_st08. Each stage compiles
// this file against its own copy of the assembly (TARGET_ASM, set by tools/cc.sh); the names
// are the same in every stage's registry.
#define ASM_DIR TARGET_ASM "/stage_common/stcom26"

// Spawns a room's drivers (stcom25) and keeps them from running into each other.
void stcom26_move(STCOM26_WORK *wp)
{
    stcom26_move_tbl[wp->routine_0](wp);
}

// Picks the room's set of drivers and spawns them.
void stcom26_move_init(STCOM26_WORK *wp)
{
    STCOM_GAME_WORK *gw;
    STCOM25_WORK *driver;
    sint16 i;

    gw = &Game_work;
    switch (gw->value_50)
    {
        case 5:
            if (gw->value_52 == 0)
            {
                wp->variant_3 = 0;
            }
            else if (gw->value_52 == 1)
            {
                wp->variant_3 = 1;
            }
            else if (gw->value_52 < 8)
            {
                wp->variant_3 = 2;
            }
            else
            {
                wp->variant_3 = 3;
            }
            break;
        case 6:
            if (gw->value_52 == 0)
            {
                wp->variant_3 = 4;
            }
            else if (gw->value_52 < 6)
            {
                wp->variant_3 = 5;
            }
            else if (gw->value_52 < 8)
            {
                wp->variant_3 = 6;
            }
            else
            {
                wp->variant_3 = 7;
            }
            break;
        case 8:
            if (gw->value_52 < 8)
            {
                wp->variant_3 = 8;
            }
            else
            {
                wp->variant_3 = 9;
            }
            break;
        case 0x19:
            wp->variant_3 = 10;
            break;
        default:
            wp->variant_3 = 0xFF;
            break;
    }
    if (gw->value_51 != 0)
    {
        wp->variant_3 = 0xFF;
    }
    if (wp->variant_3 == 0xFF)
    {
        Close_WORK_50(wp);
        return;
    }
    for (i = 0; i < stcom26_count_tbl[wp->variant_3][0]; i++)
    {
        driver = Open_SHL_WORK();
        if (driver != NULL)
        {
            driver->flags_0  = 5;
            driver->kind_2   = 4;
            driver->course_C = wp->variant_3;
            driver->start_D  = i;
            *(i + wp->slot_14.bytes) =
                ((uint32)driver - (uint32)Shl_work_tbl) / sizeof(STCOM25_WORK);
            stcom26_work_tbl[i] = driver;
        }
    }
    wp->routine_0++;
}

// Spaces the drivers out and sets their speeds.
void stcom26_move_main(STCOM26_WORK *wp)
{
    stcom26_slot_unpack(wp);
    stcom26_gap_update(wp);
    stcom26_speed_all(wp);
}

// Frees the drivers, then itself.
void stcom26_erase(STCOM26_WORK *wp)
{
    sint16 i;

    for (i = 0; i < stcom26_count_tbl[wp->variant_3][0]; i++)
    {
        Close_SHL_WORK(stcom26_work_tbl[i]);
    }
    Close_WORK_50(wp);
}

// Empty (routine 3).
void stcom26_func_033C(void)
{
}

// Unpacks the drivers' slot numbers.
void stcom26_slot_unpack(STCOM26_WORK *wp)
{
    sint16 i;
    uint32 word;

    word = 0;
    for (i = 0; i < stcom26_count_tbl[wp->variant_3][0]; i++)
    {
        if ((i & 3) == 0)
        {
            word                = wp->slot_14.words[i >> 2];
            stcom26_slot_tbl[i] = word;
        }
        else
        {
            stcom26_slot_tbl[i] = word >> ((i & 3) * 8);
        }
    }
}

// Slows each driver down behind the others, and holds it while one is in its way.
void stcom26_gap_update(STCOM26_WORK *wp)
{
    STCOM25_WORK *driver;
    STCOM25_CTL *ctl;
    sint16 i;
    sint16 j;
    sint16 clear;
    uint8 flags;

    clear = 0;
    for (i = 0; i < stcom26_count_tbl[wp->variant_3][0]; i++)
    {
        driver = &Shl_work_tbl[stcom26_slot_tbl[i]];
        driver->ctl.flags_F &= ~6;
        ctl = &driver->ctl;
        for (j = 0; j < stcom26_count_tbl[wp->variant_3][0]; j++)
        {
            if (i != j)
            {
                stcom26_gap_check(driver, &Shl_work_tbl[stcom26_slot_tbl[j]]);
            }
        }
        flags = ctl->flags_F;
        if (!(flags & 0x20))
        {
            continue;
        }
        if ((flags & 0x50) == 0x50)
        {
            if (ctl->value_1C > 0)
            {
                ctl->value_1C--;
                continue;
            }
            ctl->flags_F  = flags & ~0x10;
            ctl->value_1C = 0xF;
        }
        for (j = 0; j < stcom26_count_tbl[wp->variant_3][0]; j++)
        {
            if (i != j)
            {
                clear = stcom26_way_clear(driver, &Shl_work_tbl[stcom26_slot_tbl[j]]);
                if (clear == 0)
                {
                    break;
                }
            }
        }
        if (clear != 0)
        {
            if (++ctl->value_1C >= 0x10)
            {
                ctl->value_1C = 0;
                if (!(ctl->flags_F & 0x40))
                {
                    ctl->flags_F |= 1;
                }
                ctl->flags_F &= ~0x30;
            }
        }
        else
        {
            ctl->value_1C = 0;
        }
    }
}

// Slows `a` down when it runs up behind `b`.
void stcom26_gap_check(STCOM25_WORK *a, STCOM25_WORK *b)
{
    uint8 bits;
    STCOM25_CTL *actl;
    sint16 cell_b;
    STCOM25_CTL *bctl;
    sint16 cell_a;
    sint16 speed;
    sint32 diff;
    sint32 limit;
    sint16 dist;
    uint8 flags;

    actl = &a->ctl;
    bctl = &b->ctl;
    bits = a->ctl.dir_D | (a->ctl.dir_C << 2) | (b->ctl.dir_D << 4) | (b->ctl.dir_C << 6);
    if (bits & 1)
    {
        cell_a = (a->position.z >> 9) + 0x40;
        cell_b = (b->position.z >> 9) + 0x40;
        if (a->routine_1 == 2 && (bits & 0xC) == 8)
        {
            cell_a++;
        }
        if (b->routine_1 == 2)
        {
            if (!(bits & 0x30) || (bits & 0xF3) == 0xB3 || (bits & 0xF3) == 0x91)
            {
                cell_b++;
            }
        }
    }
    else
    {
        cell_a = (a->position.x >> 9) + 0x40;
        cell_b = (b->position.x >> 9) + 0x40;
        if (a->routine_1 == 2 && (bits & 0xC) == 0xC)
        {
            cell_a++;
        }
        if (b->routine_1 == 2)
        {
            if ((bits & 0x30) == 0x10 || (bits & 0xF3) == 0xE2 || (bits & 0xF3) == 0xC0)
            {
                cell_b++;
            }
        }
    }
    if (cell_a == cell_b)
    {
        speed = a->velocity.v.x;
        diff  = speed - b->velocity.v.x;
        limit = (diff * ((diff >> 4) + 6)) >> 4;
        if (speed > 0x100 || speed == 0)
        {
            limit += 0x600;
        }
        else
        {
            limit += 0x400;
        }
        if (bits & 1)
        {
            dist = ((bits & 3) - 2) * (b->position.x - a->position.x);
        }
        else
        {
            dist = ((bits & 3) - 1) * (b->position.z - a->position.z);
        }
        if (dist >= 0 && dist < limit && dist < actl->dist_14)
        {
            // A one-pass loop: it weights actl's and dist's references (dist stays out of LO).
            do
            {
                if (a->velocity.v.x > bctl->speed_10)
                {
                    actl->speed_10 = bctl->speed_10;
                }
                flags         = actl->flags_F;
                actl->flags_F = flags | 2;
                if (dist < 0x3C0)
                {
                    actl->flags_F  = flags | 6;
                    actl->speed_10 = 0;
                }
                actl->dist_14 = dist;
            } while (0);
        }
    }
}

// Zero while `b` stands on a cell `a` is about to drive over.
sint16 stcom26_way_clear(STCOM25_WORK *a, STCOM25_WORK *b)
{
    STCOM25_CTL *ctl;
    uint16 cell[4];
    sint16 dir;
    uint8 mask;
    sint32 base;
    sint32 xbase;
    sint16 result;

    ctl   = &a->ctl;
    dir   = a->ctl.dir_D;
    xbase = (a->position.x >> 9) + 0x40;
    if (dir == 3)
    {
        cell[0] = xbase + 1;
    }
    else
    {
        cell[0] = xbase;
    }
    base = (a->position.z >> 9) + 0x40;
    if (dir == 2)
    {
        cell[1] = base + 1;
    }
    else
    {
        cell[1] = base;
    }
    cell[2] = (b->position.x >> 9) + 0x40;
    cell[3] = (b->position.z >> 9) + 0x40;
    mask    = ctl->nodes_0[ctl->node_E].dir_3;
    result  = 1;
    if (((mask & 1) && stcom26_cell_clear(cell, dir, 1, 0x10) == 0) ||
        ((mask & 0x10) && stcom26_cell_clear(cell, (dir + 2) & 3, -1, 0x10) == 0) ||
        ((mask & 2) && stcom26_cell_clear(cell, dir, 2, 0x10) == 0) ||
        ((mask & 0x20) && stcom26_cell_clear(cell, (dir + 2) & 3, -2, 0x10) == 0) ||
        ((mask & 4) && stcom26_cell_clear(cell, dir, 3, 0x10) == 0) ||
        ((mask & 0x40) && stcom26_cell_clear(cell, (dir + 2) & 3, -3, 0x10) == 0) ||
        ((mask & 8) && stcom26_cell_clear(cell, (dir + 3) & 3, -1, 0x10) == 0) ||
        ((mask & 0x80) && stcom26_cell_clear(cell, (dir + 3) & 3, 0, 6) == 0))
    {
        result = 0;
    }
    return result;
}

// Zero when cell (x1, z1) is `n` cells from (x0, z0) in direction `dir`, within `range` across.
sint16 stcom26_cell_clear(uint16 *cell, sint16 dir, sint32 n, sint32 range)
{
    uint16 x0;
    uint16 z0;
    uint16 x1;
    uint16 z1;
    sint16 result;

    x0     = cell[0];
    z0     = cell[1];
    x1     = cell[2];
    z1     = cell[3];
    result = 1;
    switch (dir)
    {
        case 0:
            if ((sint16)z1 == (sint16)z0 - (sint16)n && (sint16)x1 >= (sint16)x0 - (sint16)range &&
                (sint16)x0 + 1 >= (sint16)x1)
            {
                result = 0;
            }
            break;
        case 1:
            if ((sint16)x1 == (sint16)x0 - (sint16)n && (sint16)z1 >= (sint16)z0 - 1 &&
                (sint16)z0 + (sint16)range >= (sint16)z1)
            {
                result = 0;
            }
            break;
        case 2:
            if ((sint16)z1 == (sint16)z0 + (sint16)n && (sint16)x1 >= (sint16)x0 - 1 &&
                (sint16)x0 + (sint16)range >= (sint16)x1)
            {
                result = 0;
            }
            break;
        case 3:
            if ((sint16)x1 == (sint16)x0 + (sint16)n && (sint16)z1 >= (sint16)z0 - (sint16)range &&
                (sint16)z0 + 1 >= (sint16)z1)
            {
                result = 0;
            }
            break;
    }
    return result;
}

// Sets every driver's target speed.
void stcom26_speed_all(STCOM26_WORK *wp)
{
    sint16 i;

    for (i = 0; i < stcom26_count_tbl[wp->variant_3][0]; i++)
    {
        stcom26_speed_set(&Shl_work_tbl[stcom26_slot_tbl[i]]);
    }
}

// Sets a driver's target speed: stops it at a corner, then waits there.
void stcom26_speed_set(STCOM25_WORK *wp)
{
    uint8 kind;
    uint8 flags;
    uint8 corner;

    kind   = wp->ctl.nodes_0[wp->ctl.node_E].flags_2;
    flags  = wp->ctl.flags_F;
    corner = kind & 0xE0;
    if (!(flags & 0x20))
    {
        if (flags & 0x10)
        {
            wp->ctl.speed_10 = 0;
            if (wp->velocity.v.x == 0)
            {
                if (kind & 0x80)
                {
                    wp->ctl.flags_F |= 0x60;
                    if (corner == 0x80)
                    {
                        wp->ctl.value_1C = 0xF;
                    }
                    else if (corner == 0xA0)
                    {
                        wp->ctl.value_1C = 0x1E;
                    }
                    else
                    {
                        wp->ctl.value_1C = 0xF0;
                    }
                }
                else
                {
                    wp->ctl.flags_F |= 0x20;
                }
                wp->accel.v.x = 0;
            }
        }
        else if (!(flags & 2))
        {
            if (flags & 1)
            {
                wp->ctl.speed_10 = 0x140;
            }
            else
            {
                wp->ctl.speed_10 = wp->ctl.start_4[wp->start_D].flags_2 << 4;
            }
        }
    }
}
