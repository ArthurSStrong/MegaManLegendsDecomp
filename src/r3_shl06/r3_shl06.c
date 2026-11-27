#include "common.h"
#include "shl06.h"

// Module entry: ROCK_NEO.EXE's SHL table (0x80089A28) points every special weapon here.
void shl06_move(SHL_WORK *wp)
{
    shl06_move_tbl[wp->routine_0](wp);
    Solid_mover_set(wp);
}

// Routine 0: launches the shot and its spread (value_528: 0, 2 or 4 more).
void shl06_init(SHL_WORK *wp)
{
    SVECTOR dir;
    SVECTOR rot;
    VECTOR v;
    MATRIX m;
    SHL_WORK *np;

    Homing_target_clear(wp->bit_d);
    rot.vx = Camera_pitch_get();
    shl06_shot_launch(wp, rot.vx, wp->rot_y);
    np = Open_PL_SHL_WORK();
    if (np)
    {
        shl06_shot_copy(wp, np);
        shl06_shot_launch(np, rot.vx, wp->rot_y + 0x80 + (rand() & 0x1F));
    }
    np = Open_PL_SHL_WORK();
    if (np)
    {
        shl06_shot_copy(wp, np);
        shl06_shot_launch(np, rot.vx, wp->rot_y - 0x80 + (rand() & 0x1F));
    }
    if (Player_work.value_528 > 0)
    {
        np = Open_PL_SHL_WORK();
        if (np)
        {
            shl06_shot_copy(wp, np);
            shl06_shot_launch(np, rot.vx, wp->rot_y + 0x100 + (rand() & 0x1F));
        }
        np = Open_PL_SHL_WORK();
        if (np)
        {
            shl06_shot_copy(wp, np);
            shl06_shot_launch(np, rot.vx, wp->rot_y - 0x100 + (rand() & 0x1F));
        }
        if (Player_work.value_528 >= 2)
        {
            np = Open_PL_SHL_WORK();
            if (np)
            {
                shl06_shot_copy(wp, np);
                shl06_shot_launch(np, rot.vx, wp->rot_y + 0x180 + (rand() & 0x1F));
            }
            np = Open_PL_SHL_WORK();
            if (np)
            {
                shl06_shot_copy(wp, np);
                shl06_shot_launch(np, rot.vx, wp->rot_y - 0x180 + (rand() & 0x1F));
            }
        }
    }
    Sound_call2(0x12A, &wp->pos);
}

// Routine 1: runs the shot until it closes itself.
void shl06_main(SHL_WORK *wp)
{
    MATRIX unused;

    if (Shot_map_check(wp, 0x20) || wp->hit_7c)
    {
        shl06_explosion_set(wp);
        Close_PL_SHL_WORK(wp);
        return;
    }
    wp->old_fpos = wp->fpos;
    wp->old_pos  = wp->pos;
    wp->hit_7c   = 0;
    wp->vel.vx += wp->ax;
    wp->vel.vy += wp->ay;
    wp->vel.vz += wp->az;
    if (wp->timer_b++ >= 90)
    {
        Close_PL_SHL_WORK(wp);
        return;
    }
    wp->fpos.w.x += wp->vel.vx << 8;
    wp->fpos.w.y += wp->vel.vy << 8;
    wp->fpos.w.z += wp->vel.vz << 8;
    wp->draw.spr.color_8c = 0x2C808080;
    wp->draw.spr.x_90 = wp->pos.vx = wp->fpos.h.x;
    wp->draw.spr.y_92 = wp->pos.vy = wp->fpos.h.y;
    wp->draw.spr.z_94 = wp->pos.vz = wp->fpos.h.z;
    shl06_atari_set(wp);
    if (++wp->routine_1 >= 6)
    {
        wp->routine_1 = 0;
    }
    wp->draw.spr.scale_96 = D_800D909C[wp->routine_1].scale_96;
    wp->draw.spr.scale_97 = D_800D909C[wp->routine_1].scale_97;
}

// Opens an Effect_explosion (effect 0x80) at the shot, sized by the weapon level (up to 2).
void shl06_explosion_set(SHL_WORK *wp)
{
    SHL_FX *fx;
    sint32 level;

    fx = Open_EFFECT_WORK();
    if (fx)
    {
        fx->routine_0 = 5;
        fx->kind_2    = 0x80;
        fx->flag_3    = 2;
        level         = Player_work.value_524;
        if (level >= 3)
        {
            level = 2;
        }
        fx->value_d = level;
        fx->value_e = 1;
        fx->pos_38  = wp->pos;
    }
}

void func_800D8CB8(SHL_WORK *wp, MATRIX *m)
{
}

// Opens an Effect_smoke (effect 0x83) at the shot.
void shl06_smoke_set(SHL_WORK *wp)
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

// Sets the shot's hit capsule between its old and new position.
void shl06_atari_set(SHL_WORK *wp)
{
    sint32 flags;

    flags = Player_weapon_attack_calc(&Player_work, 6);
    if (wp->index_c)
    {
        Set_Atari2(&wp->pos, 0x20, &wp->old_pos, 0x20, 0x200, 0x6000, wp->value_1, &wp->col_c4,
                   flags | 0x88000000, 0, 0x80);
    }
    else
    {
        Set_Atari2(&wp->pos, 0x20, &wp->old_pos, 0x20, 0x200, 0x4000, wp->value_1, &wp->col_c4,
                   flags | 0x88000000, 0, 0x80);
    }
}

// Starts the extra shot np from wp's owner, position and facing.
void shl06_shot_copy(SHL_WORK *wp, SHL_WORK *np)
{
    np->flags_0 = 1;
    np->value_2 = wp->value_2;
    np->index_c = wp->index_c;
    np->pos     = wp->pos;
    np->fpos    = wp->fpos;
    np->rot_y   = wp->rot_y;
}

// Launches a shot at pitch rx (a little up, random) and yaw ry, with the weapon's range as
// speed and gravity ay 0x200; init fires 2, 4 or 6 extra ones fanned out by level.
void shl06_shot_launch(SHL_WORK *wp, sint32 rx, sint32 ry)
{
    SVECTOR dir;
    SVECTOR rot;
    VECTOR v;
    MATRIX m;
    sint32 speed;

    wp->box_78 = &D_800D9090;
    wp->col_c4 = 0;
    wp->col_c8 = 0;
    wp->flags_0 |= 2;
    wp->flags_6 |= 0x20;
    rot.vx = rx - 0x10 - (rand() & 0xF);
    rot.vy = ry & 0xFFF;
    rot.vz = 0;
    RotMatrixYXZ(&rot, &m);
    dir.vx = 0;
    dir.vy = 0;
    dir.vz = -0x1000;
    ApplyMatrix(&m, &dir, &v);
    dir.vx                = v.vx;
    dir.vy                = v.vy;
    dir.vz                = v.vz;
    speed                 = Player_weapon_dist_calc(&Player_work, 6) + (rand() & 0x1F);
    wp->vel.vx            = (uint32)(dir.vx * speed) >> 4;
    wp->vel.vy            = (uint32)(dir.vy * speed) >> 4;
    wp->vel.vz            = (uint32)(dir.vz * speed) >> 4;
    wp->draw.spr.pos_a4   = dir;
    wp->ax                = 0;
    wp->ay                = 0x200;
    wp->az                = 0;
    wp->draw.spr.code_88  = 0x3D48000E;
    wp->draw.spr.color_8c = 0x2C808080;
    wp->draw.spr.x_90 = wp->pos.vx = wp->fpos.h.x;
    wp->draw.spr.y_92 = wp->pos.vy = wp->fpos.h.y;
    wp->draw.spr.z_94 = wp->pos.vz = wp->fpos.h.z;
    wp->old_fpos                   = wp->fpos;
    wp->old_pos                    = wp->pos;
    wp->draw.spr.scale_96          = D_800D909C[0].scale_96;
    wp->draw.spr.scale_97          = D_800D909C[0].scale_97;
    wp->draw.spr.size_98           = 0x18;
    wp->timer_b                    = 0;
    func_800D8CB8(wp, &m);
    wp->routine_1 = 0;
    wp->routine_0++;
    if (wp->mode_a)
    {
        wp->old_pos.pad = wp->pos.pad = 0x6208;
    }
    else
    {
        wp->old_pos.pad = wp->pos.pad = 0x4208;
    }
    shl06_atari_set(wp);
}
