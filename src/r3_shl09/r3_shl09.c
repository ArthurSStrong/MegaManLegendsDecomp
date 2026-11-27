#include "common.h"
#include "shl09.h"

// Module entry: ROCK_NEO.EXE's SHL table (0x80089A28) points every special weapon here.
// type_3 selects the object (shl09_type_tbl): 0 the beam, 1 the shot, 2 the spark.
void shl09_move(SHL_WORK *wp)
{
    wp->type_3 &= 0x7F;
    shl09_type_tbl[wp->type_3](wp);
}

// Type 0, the beam: runs its routine.
void shl09_beam_move(SHL_WORK *wp)
{
    shl09_move_tbl[wp->routine_0](wp);
}

// Routine 0: sets the beam's model up at the player and lives 13 frames.
void shl09_beam_init(SHL_WORK *wp)
{
    wp->box_78 = &D_800D96F8;
    wp->col_bc = 0;
    wp->col_c0 = 0;
    wp->col_c4 = 0;
    wp->col_c8 = 0;
    wp->flags_0 |= 6;
    wp->flags_6 |= 0x20;
    Homing_target_clear(wp->bit_d);
    shl09_pos_from_player(wp);
    wp->old_fpos          = wp->fpos;
    wp->old_pos           = wp->pos;
    wp->draw.mdl.color_b4 = 0x02808080;
    Model_work_init(wp, &wp->type_3);
    shl09_beam_set(wp);
    Sound_call2(0x12E, &wp->pos);
    wp->value_f   = 0xD;
    wp->value_e   = 0;
    wp->routine_0 = 1;
}

// Routine 1: follows the player; at frame 9 opens the spark (and the shot when value_53e is
// set); frames 8-10 also hit just in front of the player.
void shl09_beam_main(SHL_WORK *wp)
{
    if (--wp->value_f == 0)
    {
        Close_PL_SHL_WORK(wp);
        return;
    }
    if (wp->value_f == 9)
    {
        if (Player_work.value_53e)
        {
            shl09_shot_open(wp);
        }
        shl09_spark_open(wp);
    }
    wp->old_fpos = wp->fpos;
    wp->old_pos  = wp->pos;
    shl09_pos_from_player(wp);
    shl09_beam_set(wp);
    if (wp->value_f >= 8 && wp->value_f <= 10)
    {
        shl09_front_atari_set(wp);
    }
}

// Type 1, the shot: runs its routine.
void shl09_shot_move(SHL_WORK *wp)
{
    shl09_shot_tbl[wp->routine_0](wp);
}

// Routine 0: launches the shot's model from the player along the aim, for the weapon's range
// / 50 frames.
void shl09_shot_init(SHL_WORK *wp)
{
    SVECTOR rot;
    VECTOR v;

    wp->box_78 = &D_800D96F8;
    wp->col_c4 = 0;
    wp->col_c8 = 0;
    wp->flags_0 |= 6;
    wp->flags_6 |= 0x20;
    rot.vx = wp->rot.vx = Camera_pitch_get();
    rot.vy = wp->rot.vy = Player_work.rot.vy;
    rot.vz = wp->rot.vz = 0x680;
    RotMatrixYXZ(&rot, &wp->draw.mdl.mtx_8c);
    v.vx = v.vy = v.vz = 0x200;
    ScaleMatrix(&wp->draw.mdl.mtx_8c, &v);
    shl09_aim_vec(&wp->vel, 0x32);
    wp->pos               = Player_work.pos_3c8;
    wp->draw.mdl.pos      = wp->pos;
    wp->fpos.w.x          = wp->pos.vx << 16;
    wp->fpos.w.y          = wp->pos.vy << 16;
    wp->fpos.w.z          = wp->pos.vz << 16;
    wp->old_fpos          = wp->fpos;
    wp->old_pos           = wp->pos;
    wp->draw.mdl.color_b4 = 0x02808080;
    Model_work_init(wp, &wp->type_3);
    shl09_shot_atari_set(wp);
    wp->value_e   = 4;
    wp->value_f   = Player_weapon_dist_calc(&Player_work, 9) / 50;
    wp->routine_0 = 1;
}

// Routine 1: flies until its time runs out or it hits the map, then fades.
void shl09_shot_main(SHL_WORK *wp)
{
    if (--wp->value_f == 0)
    {
        wp->routine_0 = 2;
        return;
    }
    if (Shot_map_check(wp, 0))
    {
        wp->routine_0 = 2;
        return;
    }
    wp->old_fpos = wp->fpos;
    wp->old_pos  = wp->pos;
    wp->fpos.w.x += wp->vel.vx << 8;
    wp->fpos.w.y += wp->vel.vy << 8;
    wp->fpos.w.z += wp->vel.vz << 8;
    wp->draw.mdl.pos.vx = wp->pos.vx = wp->fpos.h.x;
    wp->draw.mdl.pos.vy = wp->pos.vy = wp->fpos.h.y;
    wp->draw.mdl.pos.vz = wp->pos.vz = wp->fpos.h.z;
    shl09_shot_atari_set(wp);
}

// Routine 2: shrinks and darkens over 4 frames, then closes.
void shl09_shot_fade(SHL_WORK *wp)
{
    SVECTOR rot;
    VECTOR v;

    wp->draw.mdl.color_b4 = SHL_RGB(2, wp->value_e << 5, wp->value_e << 5, wp->value_e << 5);
    rot.vx                = wp->rot.vx;
    rot.vy                = wp->rot.vy;
    rot.vz                = wp->rot.vz;
    RotMatrixYXZ(&rot, &wp->draw.mdl.mtx_8c);
    v.vx = v.vy = 0x200;
    v.vz        = (wp->value_e << 4) + 0x100;
    ScaleMatrix(&wp->draw.mdl.mtx_8c, &v);
    if (--wp->value_e == 0)
    {
        Close_PL_SHL_WORK(wp);
    }
}

// Type 2, the spark: runs its routine.
void shl09_spark_move(SHL_WORK *wp)
{
    shl09_spark_tbl[wp->routine_0](wp);
}

// Routine 0: sets the spark sprite up.
void shl09_spark_init(SHL_WORK *wp)
{
    wp->routine_0         = 1;
    wp->bit_d             = 0;
    wp->draw.spr.code_88  = 0x3D48002C;
    wp->draw.spr.color_8c = 0x2E808080;
    wp->flags_0 |= 0x22;
    wp->draw.spr.scale_96 = shl09_spark_scale_tbl[wp->bit_d].scale_96;
    wp->draw.spr.scale_97 = shl09_spark_scale_tbl[wp->bit_d].scale_97;
    wp->draw.spr.size_98  = 0x60;
    wp->draw.spr.value_9a = 0xB;
}

// Routine 1: steps the spark's scale; closes after 6 frames.
void shl09_spark_main(SHL_WORK *wp)
{
    wp->bit_d++;
    wp->draw.spr.scale_96 = shl09_spark_scale_tbl[wp->bit_d].scale_96;
    wp->draw.spr.scale_97 = shl09_spark_scale_tbl[wp->bit_d].scale_97;
    if (wp->bit_d >= 6)
    {
        Close_PL_SHL_WORK(wp);
    }
}

// Places the beam's model along the player's matrix, scaled by level, and sets its hit capsule.
void shl09_beam_set(SHL_WORK *wp)
{
    MATRIX m;
    SVECTOR dir;
    SVECTOR p;
    VECTOR v;
    sint32 level;
    sint32 flags;

    level = Player_work.value_53c;
    flags = Player_weapon_attack_calc(&Player_work, 9);
    m     = Player_work.mtx_248;
    p     = Player_work.pos_3c8;
    // Identity, written a word at a time.
    *(uint32 *)&wp->draw.mdl.mtx_8c.m[0][0] = 0x1000;
    *(uint32 *)&wp->draw.mdl.mtx_8c.m[0][2] = 0;
    *(uint32 *)&wp->draw.mdl.mtx_8c.m[1][1] = 0x1000;
    *(uint32 *)&wp->draw.mdl.mtx_8c.m[2][0] = 0;
    *(uint32 *)&wp->draw.mdl.mtx_8c.m[2][2] = 0x1000;
    wp->draw.mdl.mtx_8c.t[0]                = 0;
    wp->draw.mdl.mtx_8c.t[1]                = 0;
    wp->draw.mdl.mtx_8c.t[2]                = 0;
    v.vx = v.vy = v.vz = 0x1000;
    ScaleMatrix(&wp->draw.mdl.mtx_8c, &v);
    MulMatrix2(&m, &wp->draw.mdl.mtx_8c);
    dir.vx = 0;
    dir.vy = 0x1000;
    dir.vz = 0;
    ApplyMatrix(&wp->draw.mdl.mtx_8c, &dir, &v);
    dir.vx = (v.vx * 80) >> 4;
    dir.vy = (v.vy * 80) >> 4;
    dir.vz = (v.vz * 80) >> 4;
    v.vx   = (p.vx << 16) + (dir.vx << 8) * shl09_beam_reach_tbl[level];
    v.vy   = (p.vy << 16) + (dir.vy << 8) * shl09_beam_reach_tbl[level];
    v.vz   = (p.vz << 16) + (dir.vz << 8) * shl09_beam_reach_tbl[level];
    dir.vx = v.vx >> 16;
    dir.vy = v.vy >> 16;
    dir.vz = v.vz >> 16;
    v.vx   = shl09_beam_scale_tbl[level].width + 0x1000;
    v.vz   = shl09_beam_scale_tbl[level].width + 0x1000;
    v.vy   = shl09_beam_scale_tbl[level].len + 0x1000;
    ScaleMatrix(&wp->draw.mdl.mtx_8c, &v);
    Set_Atari2(&dir, shl09_beam_size_tbl[level], &wp->pos, shl09_beam_size_tbl[level], 0x200,
               0x4000, wp->value_1, &wp->col_bc, flags | 0x28000000, 0, 0x80);
    wp->col_bc = 0;
    wp->col_c0 = 0;
}

// Hit capsule just in front of the player, along the aim.
void shl09_front_atari_set(SHL_WORK *wp)
{
    SVECTOR d;
    SVECTOR a;
    SVECTOR b;
    SHL_FIXED f;
    sint32 size;
    sint32 flags;

    f = Player_work.fpos;
    f.w.y -= 0x500000;
    size  = shl09_front_size_tbl[Player_work.value_53c];
    flags = Player_weapon_attack_calc(&Player_work, 9);
    shl09_aim_vec(&d, 0x3C);
    a.vx = (f.w.x + (d.vx << 9)) >> 16;
    a.vy = (f.w.y + (d.vy << 9)) >> 16;
    a.vz = (f.w.z + (d.vz << 9)) >> 16;
    b.vx = (f.w.x + (d.vx << 8)) >> 16;
    b.vy = (f.w.y + (d.vy << 8)) >> 16;
    b.vz = (f.w.z + (d.vz << 8)) >> 16;
    Set_Atari2(&a, size, &b, size, 0x200, 0x4000, wp->value_1, &wp->col_c4, flags | 0x28000000, 0,
               0x80);
    wp->col_c4 = 0;
    wp->col_c8 = 0;
}

// The level goes through a sint8 local: retail loads it with lbu and sign-extends it in a
// register (sll 24/sra 22); indexing with the field directly gives lb.
// Sets the shot's hit capsule between its old and new position.
void shl09_shot_atari_set(SHL_WORK *wp)
{
    sint8 level;

    level = Player_work.value_53c;
    Set_Atari2(&wp->pos, 8, &wp->old_pos, 0x5C, 0x200, 0x4000, wp->value_1, &wp->col_c4,
               shl09_shot_flags_tbl[level] | 0x8000000, 0, 0x80);
    wp->col_c4 = 0;
    wp->col_c8 = 0;
}

// Opens the type 1 shot.
void shl09_shot_open(SHL_WORK *wp)
{
    SHL_WORK *np;

    np = Open_PL_SHL_WORK();
    if (np)
    {
        np->flags_0 = 1;
        np->value_2 = wp->value_2;
        np->type_3  = 1;
        np->index_c = wp->value_1;
    }
}

// Opens the type 2 spark 100 units ahead of the player along the aim.
void shl09_spark_open(SHL_WORK *wp)
{
    SVECTOR d;
    SVECTOR p;
    VECTOR v;
    SHL_WORK *np;

    p = Player_work.pos_3a8;
    shl09_aim_vec(&d, 100);
    v.vx = (p.vx << 16) + (d.vx << 8);
    v.vy = (p.vy << 16) + (d.vy << 8);
    v.vz = (p.vz << 16) + (d.vz << 8);
    np   = Open_PL_SHL_WORK();
    if (np)
    {
        np->flags_0       = 1;
        np->value_2       = wp->value_2;
        np->type_3        = 2;
        np->index_c       = wp->value_1;
        np->draw.spr.x_90 = v.vx >> 16;
        np->draw.spr.y_92 = v.vy >> 16;
        np->draw.spr.z_94 = v.vz >> 16;
    }
}

// Moves the object to Player_work.pos_3c8.
void shl09_pos_from_player(SHL_WORK *wp)
{
    SVECTOR p;

    p                   = Player_work.pos_3c8;
    wp->fpos.w.x        = p.vx << 16;
    wp->fpos.w.y        = p.vy << 16;
    wp->fpos.w.z        = p.vz << 16;
    wp->draw.mdl.pos.vx = wp->pos.vx = p.vx;
    wp->draw.mdl.pos.vy = wp->pos.vy = p.vy;
    wp->draw.mdl.pos.vz = wp->pos.vz = p.vz;
}

// out = the aim direction (camera pitch, player facing) times speed / 16.
void shl09_aim_vec(SVECTOR *out, sint32 speed)
{
    MATRIX m;
    SVECTOR rot;
    VECTOR v;

    rot.vx = Camera_pitch_get();
    rot.vy = Player_work.rot.vy;
    rot.vz = 0;
    RotMatrixYXZ(&rot, &m);
    v.vx = v.vy = v.vz = 0x1000;
    ScaleMatrix(&m, &v);
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = -0x1000;
    ApplyMatrix(&m, &rot, &v);
    out->vx = (v.vx * speed) >> 4;
    out->vy = (v.vy * speed) >> 4;
    out->vz = (v.vz * speed) >> 4;
}
