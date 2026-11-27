#include "common.h"
#include "shl07.h"

// Module entry: ROCK_NEO.EXE's SHL table (0x80089A28) points every special weapon here.
void shl07_move(SHL_WORK *wp)
{
    shl07_move_tbl[wp->routine_0](wp);
}

// Routine 0: clears the homing slot and starts the sound.
void shl07_init(SHL_WORK *wp)
{
    SVECTOR unused_s;
    VECTOR unused_v;

    Homing_target_clear(wp->bit_d);
    wp->timer_b = 0;
    wp->routine_0++;
    Sound_call2(0x12B, &wp->pos);
}

// Routine 1: each frame, item drops (effect kind 0x87) within the weapon's range of the
// player are pushed away by shl07_push_tbl[level], or, with a negative strength, pulled to the
// player. Ends when the player leaves state 7, the timer runs out or value_100 passes the limit.
void shl07_main(SHL_WORK *wp)
{
    SHL_PLAYER *tp;
    SHL_FX *fx;
    SVECTOR p;
    SVECTOR d;
    SVECTOR old;
    SVECTOR tmp;
    VECTOR diff;
    uint32 i;
    uint32 r;
    uint32 r2;
    sint32 push;

    wp->timer_b++;
    tp   = Obj_id_work(wp->index_c);
    p    = tp->pos;
    r    = Player_weapon_dist_calc(&Player_work, 7);
    r2   = r * 2;
    push = shl07_push_tbl[Player_work.value_530];
    for (i = 0; i < 64; i++)
    {
        fx = &Effect_work_tbl[i];
        if (fx->routine_0 == 0 || fx->kind_2 != 0x87 || fx->routine_8 >= 5)
        {
            continue;
        }
        d       = fx->pos_38;
        diff.vx = p.vx - d.vx;
        if (diff.vx + r > r2)
        {
            continue;
        }
        diff.vy = p.vy - d.vy;
        if (diff.vy + r > r2)
        {
            continue;
        }
        diff.vz = p.vz - d.vz;
        if (diff.vz + r > r2)
        {
            continue;
        }
        if (diff.vx * diff.vx + diff.vy * diff.vy + diff.vz * diff.vz >= r * r)
        {
            continue;
        }
        VectorNormalS(&diff, &d);
        old = fx->pos_38;
        if (push > 0)
        {
            fx->pos_38.vx += (d.vx * push) >> 12;
            fx->pos_38.vz += (d.vz * push) >> 12;
            tmp = fx->pos_38;
            Scr_move_check(&tmp, &old, 1, &D_8008A310[fx->index_a]);
            fx->pos_38.vx = tmp.vx;
            fx->pos_38.vy = tmp.vy;
            fx->pos_38.vz = tmp.vz;
        }
        else
        {
            fx->pos_38.vx = p.vx - ((d.vx * ((rand() & 0x1F) - 0x10)) >> 12);
            fx->pos_38.vy = p.vy - 0x8C;
            fx->pos_38.vz = p.vz - ((d.vz * ((rand() & 0x1F) - 0x10)) >> 12);
            break;
        }
    }
    if (tp->state_170 == 7)
    {
        if (tp->timer_532 == 0)
        {
            Sound_call(0x12C, 0, 0);
            Close_PL_SHL_WORK(wp);
        }
        else
        {
            if (tp->timer_532 != 0x7FFF)
            {
                tp->timer_532--;
            }
            if (tp->value_100 > D_80089ADA)
            {
                Sound_call(0x12C, 0, 0);
                Close_PL_SHL_WORK(wp);
            }
        }
    }
    else
    {
        Close_PL_SHL_WORK(wp);
    }
}

void func_800D8C20(void)
{
}
