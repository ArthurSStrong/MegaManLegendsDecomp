#include "common.h"
#include "shl03.h"

// Module entry: ROCK_NEO.EXE's SHL table (0x80089A28) points every special weapon here.
void shl03_move(SHL_WORK *wp)
{
    shl03_move_tbl[wp->routine_0](wp);
    Solid_mover_set(wp);
}

// Routine 0: sets the shot up.
void shl03_init(SHL_WORK *wp)
{
    SVECTOR dir;
    SVECTOR rot;
    VECTOR v;
    MATRIX m;

    wp->flags_0 |= 2;
    wp->flags_6 |= 0x20;
    Homing_target_clear(wp->bit_d);
    wp->col_c4 = 0;
    wp->col_c8 = 0;
    wp->box_78 = &D_800D8E18;
    rot.vx     = Camera_pitch_get();
    rot.vy     = wp->rot_y & 0xFFF;
    rot.vz     = 0;
    RotMatrixYXZ(&rot, &m);
    dir.vx = 0;
    dir.vy = 0;
    dir.vz = -0x1000;
    ApplyMatrix(&m, &dir, &v);
    dir.vx                = v.vx;
    dir.vy                = v.vy;
    dir.vz                = v.vz;
    wp->vel.vx            = 0;
    wp->vel.vy            = 0;
    wp->vel.vz            = 0;
    wp->draw.spr.pos_a4   = dir;
    wp->ax                = dir.vx / 2;
    wp->ay                = dir.vy / 2;
    wp->az                = dir.vz / 2;
    wp->draw.spr.code_88  = 0x3D08000E;
    wp->draw.spr.color_8c = 0x2C808080;
    wp->draw.spr.x_90 = wp->pos.vx = wp->fpos.h.x;
    wp->draw.spr.y_92 = wp->pos.vy = wp->fpos.h.y;
    wp->draw.spr.z_94 = wp->pos.vz = wp->fpos.h.z;
    wp->old_fpos                   = wp->fpos;
    wp->old_pos                    = wp->pos;
    wp->draw.spr.scale_96          = 0x80;
    wp->draw.spr.scale_97          = 0x80;
    wp->draw.spr.size_98           = D_800D8E24[Player_work.value_50c];
    wp->hit_7c                     = 0;
    wp->timer_b                    = 0;
    wp->routine_1                  = 0;
    wp->routine_0++;
    if (wp->mode_a)
    {
        wp->old_pos.pad = wp->pos.pad = 0x6208;
    }
    else
    {
        wp->old_pos.pad = wp->pos.pad = 0x4208;
    }
    shl03_atari_set(wp);
    Sound_call2(0x122, &wp->pos);
}

// Routine 1: runs the shot until it closes itself.
void shl03_main(SHL_WORK *wp)
{
    MATRIX unused;

    if (Shot_map_check(wp, 0x20) || wp->hit_7c)
    {
        shl03_explosion_set(wp);
        Swing_Camera2(0x14, 0x64, 0xFA0, &wp->pos);
        Close_PL_SHL_WORK(wp);
        return;
    }
    wp->old_fpos = wp->fpos;
    wp->old_pos  = wp->pos;
    if (wp->timer_b < 15)
    {
        wp->vel.vx += wp->ax;
        wp->vel.vy += wp->ay;
        wp->vel.vz += wp->az;
    }
    if (wp->timer_b++ < Player_weapon_dist_calc(&Player_work, 3))
    {
        if (wp->timer_b & 1)
        {
            wp->draw.spr.scale_96 = 0xA0;
            wp->draw.spr.scale_97 = 0x60;
        }
        else
        {
            wp->draw.spr.scale_96 = 0x80;
            wp->draw.spr.scale_97 = 0x80;
        }
        wp->fpos.w.x += wp->vel.vx << 8;
        wp->fpos.w.y += wp->vel.vy << 8;
        wp->fpos.w.z += wp->vel.vz << 8;
        wp->draw.spr.color_8c = 0x2C808080;
        wp->draw.spr.x_90 = wp->pos.vx = wp->fpos.h.x;
        wp->draw.spr.y_92 = wp->pos.vy = wp->fpos.h.y;
        wp->draw.spr.z_94 = wp->pos.vz = wp->fpos.h.z;
    }
    else
    {
        Close_PL_SHL_WORK(wp);
        return;
    }
    shl03_atari_set(wp);
    if (wp->timer_b & 1)
    {
        shl03_smoke_set(wp);
    }
}

// Opens an Effect_explosion (effect 0x80) at the shot, sized by D_800D8E2C[level].
void shl03_explosion_set(SHL_WORK *wp)
{
    SHL_FX *fx;

    fx = Open_EFFECT_WORK();
    if (fx)
    {
        fx->routine_0 = 5;
        fx->kind_2    = 0x80;
        fx->flag_3    = 2;
        fx->value_d   = D_800D8E2C[Player_work.value_50c];
        fx->value_e   = 1;
        fx->pos_38    = wp->pos;
    }
}

// Opens an Effect_smoke (effect 0x83) at the shot (every other frame, from shl03_main).
void shl03_smoke_set(SHL_WORK *wp)
{
    SHL_FX *fx;

    fx = Open_EFFECT_WORK();
    if (fx)
    {
        fx->routine_0 = 1;
        fx->kind_2    = 0x83;
        fx->flag_3    = 1;
        fx->x_1c      = wp->pos.vx;
        fx->y_1e      = wp->pos.vy;
        fx->z_20      = wp->pos.vz;
    }
}

// Sets the shot's hit capsule between its old and new position, its sprite's size wide.
void shl03_atari_set(SHL_WORK *wp)
{
    sint32 size;
    sint32 flags;

    size  = wp->draw.spr.size_98;
    flags = Player_weapon_attack_calc(&Player_work, 3);
    if (wp->index_c)
    {
        Set_Atari2(&wp->pos, size, &wp->old_pos, size, 0x200, 0x6000, wp->value_1, &wp->col_c4,
                   flags | 0x88000000, 0, 0x80);
    }
    else
    {
        Set_Atari2(&wp->pos, size, &wp->old_pos, size, 0x200, 0x4000, wp->value_1, &wp->col_c4,
                   flags | 0x88000000, 0, 0x80);
    }
}
