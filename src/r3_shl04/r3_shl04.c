#include "common.h"
#include "shl04.h"

// Module entry: ROCK_NEO.EXE's SHL table (0x80089A28) points every special weapon here.
void shl04_move(SHL_WORK *wp)
{
    shl04_move_tbl[wp->routine_0](wp);
}

// Routine 0: sets the shot up.
void shl04_init(SHL_WORK *wp)
{
    SVECTOR unused_s;
    VECTOR unused_v;

    Homing_target_clear(wp->bit_d);
    wp->timer_b = 0;
    wp->routine_0++;
    shl04_atari_set(wp);
    Sound_call2(0x123, &wp->pos);
}

// Routine 1: while the player stays in state 4 and has energy, keeps the hit box on the arm
// tip and spins it (shl04_tip_rotate, 1/8 turn per frame).
void shl04_main(SHL_WORK *wp)
{
    SHL_PLAYER *tp;
    MATRIX unused;
    SVECTOR rot;

    tp         = Obj_id_work(wp->index_c);
    wp->pos.vx = tp->pos_3c8.vx + (tp->pos_3c8.vx - tp->pos_3c0.vx) * 4;
    wp->pos.vy = tp->pos_3c8.vy + (tp->pos_3c8.vy - tp->pos_3c0.vy) * 4;
    wp->pos.vz = tp->pos_3c8.vz + (tp->pos_3c8.vz - tp->pos_3c0.vz) * 4;
    if (Shot_map_check(wp, 0x80))
    {
        func_800D8ADC(wp, 1);
        Sound_call(0x125, 0, 0);
    }
    if (tp->state_170 == 4)
    {
        if (tp->timer_51a != 0)
        {
            if (tp->timer_51a != 0x7FFF)
            {
                tp->timer_51a--;
            }
            if (tp->value_100 <= D_80089AB6 && tp->value_174 != 0)
            {
                goto rotate;
            }
        }
        Sound_call(0x124, 0, 0);
    }
    Close_PL_SHL_WORK(wp);
    return;

rotate:
    rot.vx = 0;
    rot.vy = (wp->timer_b & 7) << 9;
    rot.vz = 0;
    shl04_tip_rotate(tp, &rot);
    wp->timer_b--;
    shl04_atari_set(wp);
}

// Sets the hit capsule from the arm tip (pos_3c8) to 4 steps further along its motion.
void shl04_atari_set(SHL_WORK *wp)
{
    SHL_PLAYER *tp;
    SVECTOR pos;
    sint32 flags;

    flags  = Player_weapon_attack_calc(&Player_work, 4);
    tp     = Obj_id_work(wp->index_c);
    pos.vx = tp->pos_3c8.vx + (tp->pos_3c8.vx - tp->pos_3c0.vx) * 4;
    pos.vy = tp->pos_3c8.vy + (tp->pos_3c8.vy - tp->pos_3c0.vy) * 4;
    pos.vz = tp->pos_3c8.vz + (tp->pos_3c8.vz - tp->pos_3c0.vz) * 4;
    Set_Atari2(&tp->pos_3c8, 0x14, &pos, 8, 0x200, 0x4000, wp->value_1, &wp->col_c4,
               flags | 0x88000000, 0, 0x80);
    wp->col_c4 = 0;
    wp->col_c8 = 0;
}

void func_800D8ADC(SHL_WORK *wp, sint32 arg)
{
}

// Rebuilds the arm tip's matrix (mtx_248) rotated by rot, and its position pos_3c8.
void shl04_tip_rotate(SHL_PLAYER *tp, SVECTOR *rot)
{
    VECTOR v;
    SVECTOR unused_s;
    VECTOR unused_v;
    SVECTOR pos;
    MATRIX rm;
    MATRIX m;

    pos.vx = D_800DD600->pos_20.vx;
    pos.vy = D_800DD600->pos_20.vy;
    pos.vz = D_800DD600->pos_20.vz;
    m      = tp->mtx_228;
    ApplyMatrix(&m, &pos, &v);
    RotMatrix(rot, &rm);
    MulMatrix(&m, &rm);
    m.t[0]         = v.vx + tp->pos_3c0.vx;
    m.t[1]         = v.vy + tp->pos_3c0.vy;
    m.t[2]         = v.vz + tp->pos_3c0.vz;
    tp->pos_3c8.vx = m.t[0];
    tp->pos_3c8.vy = m.t[1];
    tp->pos_3c8.vz = m.t[2];
    tp->mtx_248    = m;
}
