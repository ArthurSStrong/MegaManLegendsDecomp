#include "common.h"
#include "st19.h"

void func_8010A0BC(STAGE_OBJ *wp)
{
    D_80118094[wp->routine_0](wp);
}

void func_8010A0F8(ST19_10C_B *wp)
{
    wp->routine_0           = 1;
    wp->routine_3           = 0x18;
    wp->sprite.value_E      = 0x60;
    wp->sprite.value_F      = 0x20;
    wp->sprite.value_10     = 8;
    wp->sprite.mode_0       = 0x3D48002E;
    wp->sprite.color_4.word = 0x2E40E040;
    wp->routine_1           = 0;
    wp->routine_2           = 0;
    wp->byte_C              = 0;
    wp->byte_E              = 0;
    wp->byte_F              = 0;
    wp->count_CC            = 0;
    wp->flags_0 |= 2;
    wp->sprite.pos_8[0] = wp->position.x = wp->position_fixed.f.x.words.integer;
    wp->sprite.pos_8[1] = wp->position.y = wp->position_fixed.f.y.words.integer;
    wp->sprite.pos_8[2] = wp->position.z = wp->position_fixed.f.z.words.integer;
    wp->position_fixed_prev              = wp->position_fixed.v;
    wp->position_prev                    = wp->position;
    wp->sprite.hit_14                    = 0;
    Sound_call2(0xF0, &wp->position);
}

void func_8010A1EC(ST19_10C_B *wp)
{
    wp->count_CC++;
    wp->sprite.pos_8[0] = wp->position.x;
    wp->sprite.pos_8[1] = wp->position.y;
    wp->sprite.pos_8[2] = wp->position.z;
    if (wp->routine_3 == 0)
    {
        wp->byte_C = 1;
        wp->routine_0++;
        wp->sprite.dir_1C = func_8010A78C(wp);
        Sound_call2(0xF1, &wp->position);
    }
    else
    {
        func_8010A924(wp);
    }
}

void func_8010A298(ST19_10C_B *wp)
{
    STCOM_SVECTOR dir;
    STCOM_SVECTOR target;
    sint32 hit;

    hit = Scr_line_check(wp);
    if (hit != 0)
    {
        Map_obj_hit_damage(hit, D_80118088[wp->byte_D]);
        wp->routine_3 = 0x18;
        wp->count_CC  = 0;
        wp->routine_0++;
        Sound_call2(0xF2, &wp->position);
        return;
    }
    if (wp->sprite.hit_14 & 0x2000000)
    {
        wp->routine_3 = 0x18;
        wp->count_CC  = 0;
        wp->routine_0++;
        Sound_call2(0xF2, &wp->position);
        return;
    }
    wp->position_fixed_prev = wp->position_fixed.v;
    wp->position_prev       = wp->position;
    if (wp->byte_C >= 0)
    {
        wp->sprite.pos_8[0] = wp->position.x;
        wp->sprite.pos_8[1] = wp->position.y;
        wp->sprite.pos_8[2] = wp->position.z;
        wp->trail_D4[0]     = wp->position;
        wp->trail_D4[1]     = wp->trail_D4[0];
        wp->trail_D4[2]     = wp->trail_D4[1];
        wp->trail_D4[3]     = wp->trail_D4[2];
        wp->trail_D4[4]     = wp->trail_D4[3];
        wp->trail_D4[5]     = wp->trail_D4[4];
        return;
    }
    if (--wp->routine_3 == 0)
    {
        Close_SHL_WORK(wp);
    }
    else
    {
        target = func_8010A78C((ST19_10C_C *)wp);
        Vec_turn_toward(&dir, &target, &wp->sprite.dir_1C, 0x32, 0xFFFB10B);
        wp->sprite.dir_1C    = dir;
        wp->value_44.half[0] = (dir.x * 25) >> 2;
        wp->value_44.half[1] = (dir.y * 25) >> 2;
        wp->value_48.half[0] = (dir.z * 25) >> 2;
        wp->position_fixed.v.x += wp->value_44.half[0] << 8;
        wp->position_fixed.v.y += wp->value_44.half[1] << 8;
        wp->position_fixed.v.z += wp->value_48.half[0] << 8;
        Obj_pos_adj_round(&wp->position_fixed.v, &wp->position);
        func_8010A86C((ST19_TRAIL_B *)wp);
        wp->trail_D4[5]     = wp->trail_D4[4];
        wp->trail_D4[4]     = wp->trail_D4[3];
        wp->trail_D4[3]     = wp->trail_D4[2];
        wp->trail_D4[2]     = wp->trail_D4[1];
        wp->trail_D4[1]     = wp->trail_D4[0];
        wp->trail_D4[0]     = wp->position;
        wp->sprite.pos_8[0] = wp->position.x;
        wp->sprite.pos_8[1] = wp->position.y;
        wp->sprite.pos_8[2] = wp->position.z;
    }
    if (wp->flags_0 & 1)
    {
        Set_Atari(&wp->position, 0xC0, 0, 0xA000, wp->reserved01, &wp->sprite.hit_14,
                  D_8011807C[wp->byte_D] | 0x49000000, 0, 0x80);
    }
    wp->sprite.hit_14 = 0;
}

void func_8010A6D8(ST19_10C_B *wp)
{
    uint8 c;
    uint8 g;

    if (wp->count_CC < 4)
    {
        c                       = 0x40 - wp->count_CC * 16;
        g                       = 0xE0 - wp->count_CC * 56;
        wp->sprite.color_4.word = 0x2E000000 | (c << 16) | (g << 8) | c;
    }
    else
    {
        wp->sprite.color_4.word = 0x2E000000;
        wp->flags_0 &= ~2;
    }
    wp->count_CC++;
    if (wp->routine_3 == 0)
    {
        Close_SHL_WORK(wp);
    }
    else
    {
        func_8010ABC0(wp);
    }
}

STCOM_SVECTOR func_8010A78C(ST19_10C_B *wp)
{
    STCOM_SVECTOR dir;
    STCOM_FIXED_VECTOR d;

    d.x = Player_work.position.x - wp->position.x;
    d.y = Player_work.position.y - wp->position.y;
    d.z = Player_work.position.z - wp->position.z;
    if (d.x > 0x687E || d.x < -0x687E || d.y > 0x687E || d.y < -0x687E || d.z > 0x687E ||
        d.z < -0x687E)
    {
        d.x >>= 2;
        d.y >>= 2;
        d.z >>= 2;
    }
    VectorNormalS(&d, &dir);
    return dir;
}

void func_8010A86C(ST19_10C_B *wp)
{
    STCOM23_PACKET *p;
    uint8 i;

    for (i = 0; i < 6; i++)
    {
        p          = Billboard_alloc();
        p->code_4  = 0x2E80C080;
        p->xy_8    = *(uint32 *)&wp->trail_D4[i].x;
        p->z_10    = *(uint32 *)&wp->trail_D4[i].z;
        p->uv_C    = 0x3D482080;
        p->uv_14   = 0x2E209F;
        p->uv_1C   = 0x3F80;
        p->uv_24   = 0x3F9F;
        p->size_18 = 0xC0 - i * 24;
        p->pad_20  = 0;
    }
}

void func_8010A924(ST19_10C_C *wp)
{
    STCOM23_PACKET *p;
    STCOM_SVECTOR pos;
    sint16 t;
    sint16 angle;
    sint16 radius;
    sint16 height;
    uint8 r;
    uint8 g;
    uint8 b;
    uint8 i;

    for (i = 0; i < 24; i++)
    {
        t = wp->count_CC;
        if (D_801180A4[i].start_0 < t && t < D_801180A4[i].start_0 + 0x10)
        {
            p      = Billboard_alloc();
            t      = wp->count_CC - D_801180A4[i].start_0;
            angle  = (D_801180A4[i].angle_4 + t * 24) & 0xFFF;
            height = D_801180A4[i].height_6 - t * (D_801180A4[i].height_6 / 16);
            radius = D_801180A4[i].radius_2 - t * (D_801180A4[i].radius_2 / 16);
            if (t < 8)
            {
                r = (t + 1) * 12;
                g = (t + 1) * 28;
                b = r;
            }
            else
            {
                r = 0x60;
                g = 0xE0;
                b = 0x60;
            }
            pos.x      = wp->position.x + ((rsin(angle) * radius) >> 12);
            pos.z      = wp->position.z + ((rcos(angle) * radius) >> 12);
            pos.y      = wp->position.y + height;
            p->code_4  = 0x2E000000 | (b << 16) | (g << 8) | r;
            p->xy_8    = (pos.y << 16) + pos.x;
            p->z_10    = pos.z;
            p->uv_C    = 0x3D482040;
            p->uv_1C   = 0x3F40;
            p->uv_24   = 0x3F5F;
            p->uv_14   = 0x2E205F;
            p->size_18 = 0x40;
            p->pad_20  = 0;
        }
        if (t == D_801180A4[i].start_0 + 0x10)
        {
            wp->routine_3--;
            wp->sprite.value_10 += 8;
        }
    }
}

void func_8010ABC0(ST19_10C_C *wp)
{
    STCOM23_PACKET *p;
    STCOM_SVECTOR pos;
    sint16 t;
    sint16 angle;
    sint16 radius;
    sint16 height;
    uint8 r;
    uint8 g;
    uint8 b;
    uint8 i;

    for (i = 0; i < 24; i++)
    {
        if (wp->count_CC < D_801180A4[i].start_0 + 0x10)
        {
            p = Billboard_alloc();
            if (wp->count_CC < D_801180A4[i].start_0)
            {
                r      = 0x60;
                g      = 0xE0;
                b      = 0x60;
                angle  = D_801180A4[i].angle_4;
                height = (D_801180A4[i].height_6 / 16) * wp->count_CC;
                radius = (D_801180A4[i].radius_2 / 16) * wp->count_CC;
            }
            else
            {
                t      = wp->count_CC - D_801180A4[i].start_0;
                r      = 0x60 - t * 6;
                g      = -0x20 - t * 14;
                b      = r;
                angle  = D_801180A4[i].angle_4;
                height = D_801180A4[i].start_0 * (D_801180A4[i].height_6 / 16);
                radius = D_801180A4[i].start_0 * (D_801180A4[i].radius_2 / 16);
                height += t * (D_801180A4[i].height_6 / 32);
                radius += t * (D_801180A4[i].radius_2 / 32);
            }
            pos.x      = wp->position.x + ((rsin(angle) * radius) >> 12);
            pos.z      = wp->position.z + ((rcos(angle) * radius) >> 12);
            pos.y      = wp->position.y + height;
            p->code_4  = 0x2E000000 | (b << 16) | (g << 8) | r;
            p->xy_8    = (pos.y << 16) + pos.x;
            p->z_10    = pos.z;
            p->uv_C    = 0x3D482040;
            p->uv_1C   = 0x3F40;
            p->uv_24   = 0x3F5F;
            p->uv_14   = 0x2E205F;
            p->size_18 = 0x30;
            p->pad_20  = 0;
        }
        if (wp->count_CC == D_801180A4[i].start_0 + 0x10)
        {
            wp->routine_3--;
        }
    }
}
