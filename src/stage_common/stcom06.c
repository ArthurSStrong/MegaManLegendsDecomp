#include "common.h"
#include "stage_common/stcom06.h"

// Shared stage code, linked into 8 stages: r3_st00, r3_st05, r3_st06, r3_st09, r3_st0a,
// r3_st0b, r3_st13 and r3_st14. Each stage compiles this file against its own copy of the
// assembly (TARGET_ASM, set by tools/cc.sh); the names are the same in every stage's registry.
#define ASM_DIR TARGET_ASM "/stage_common/stcom06"

void stcom06_move(STCOM06_WORK *wp)
{
    STCOM_SVECTOR pos;

    stcom06_move_tbl[wp->routine_0](wp);
    if (!(wp->type_3 & 1))
    {
        wp->index_F++;
    }
    if (wp->index_F == 6)
    {
        wp->index_F = 0;
    }
    pos.x = wp->sprite.pos_8[0];
    pos.y = wp->sprite.pos_8[1];
    pos.z = wp->sprite.pos_8[2];
    if (!(wp->type_3 & 1))
    {
        if (wp->routine_2 != 0)
        {
            Set_Atari(&pos, 0x20, 0, 0x4000, wp->value_1, &wp->sprite.hit_14, 0x8000000, 0, 0x80);
        }
        else
        {
            Set_Atari(&pos, 0x20, 0, 0x8000, wp->value_1, &wp->sprite.hit_14, 0x8000000, 0, 0x80);
        }
        Solid_mover_set(wp);
    }
    Model_Shadow_Trans(wp, 0x3C, 0x200, 0);
}

void stcom06_move_init(STCOM06_WORK *wp)
{
    STCOM_FIXED_VECTOR unused; // never used; retail reserves its frame

    wp->routine_2 = wp->routine_3 = 0;
    wp->count_C                   = 0;
    wp->flags_E                   = 0;
    wp->index_F                   = 0;
    wp->value_7C                  = 0;
    wp->velocity.v.x              = (-rsin(wp->angle_64) * wp->sprite.hit_14) >> 4;
    wp->velocity.v.y              = -wp->sprite.value_18 << 8;
    wp->velocity.v.z              = (-rcos(wp->angle_64) * wp->sprite.hit_14) >> 4;
    wp->accel.x                   = 0;
    wp->accel.y                   = 0x200;
    wp->accel.z                   = 0;
    wp->sprite.hit_14             = 0;
    wp->sprite.mode_0             = 0x3D48000E;
    wp->sprite.color_4.word       = 0x2C808080;
    Obj_pos_adj_round(&wp->position_fixed, &wp->position);
    wp->position_fixed_prev.x = wp->position_fixed.x - (wp->velocity.v.x << 10);
    wp->position_fixed_prev.y = wp->position_fixed.y;
    wp->position_fixed_prev.z = wp->position_fixed.z - (wp->velocity.v.z << 10);
    Obj_pos_adj_round(&wp->position_fixed_prev, &wp->position_prev);
    wp->value_78 = 0;
    if (wp->type_3 & 0x80)
    {
        wp->sprite.value_E = stcom06_frame2_tbl[wp->index_F].a;
        wp->sprite.value_F = stcom06_frame2_tbl[wp->index_F].b;
    }
    else
    {
        wp->sprite.value_E = stcom06_frame_tbl[wp->index_F].a;
        wp->sprite.value_F = stcom06_frame_tbl[wp->index_F].b;
    }
    wp->sprite.value_10 = 0x20;
    wp->value_74        = 1;
    wp->flags_0 |= 2;
    if (stcom06_bounce(wp) != 0)
    {
        wp->routine_0 = 3;
        wp->timer_D   = 0;
    }
    wp->sprite.pos_8[0] = wp->position.x;
    wp->sprite.pos_8[1] = wp->position.y - 0x10;
    wp->sprite.pos_8[2] = wp->position.z;
    wp->flags_E &= ~4;
    wp->routine_0++;
}

void stcom06_move_fly(STCOM06_WORK *wp)
{
    STCOM_FIXED_VECTOR unused; // never used; retail reserves its frame
    STCOM_SVECTOR unused2;
    STCOM_EFFECT *effect;
    uint32 hit;

    hit = wp->sprite.hit_14;
    if (hit & 0x20000000)
    {
        wp->velocity.v.x  = (-rsin(Player_work.rotation.y) * 0x20) >> 4;
        wp->velocity.v.y  = -0x1000;
        wp->velocity.v.z  = (-rcos(Player_work.rotation.y) * 0x20) >> 4;
        wp->routine_2     = 1;
        wp->sprite.hit_14 = 0;
    }
    else if (wp->routine_2 != 0)
    {
        if (hit & 0x1C000000)
        {
            wp->sprite.hit_14 = 0;
            effect            = Open_EFFECT_WORK();
            if (effect != NULL)
            {
                stcom06_explosion_set(wp, effect);
            }
            wp->flags_0 = 0;
            Close_SHL_WORK(wp);
            return;
        }
    }
    else if (hit & 0x1A000000)
    {
        wp->sprite.hit_14 = 0;
        effect            = Open_EFFECT_WORK();
        if (effect != NULL)
        {
            stcom06_explosion_set(wp, effect);
        }
        wp->flags_0 = 0;
        Close_SHL_WORK(wp);
        return;
    }
    wp->sprite.hit_14 = 0;
    if (!(wp->type_3 & 1))
    {
        if (wp->value_7C != 0)
        {
            wp->sprite.hit_14 = 0;
            effect            = Open_EFFECT_WORK();
            if (effect != NULL)
            {
                stcom06_explosion_set(wp, effect);
            }
            wp->flags_0 = 0;
            Close_SHL_WORK(wp);
            return;
        }
    }
    else
    {
        wp->value_7C = 0;
    }
    wp->position_fixed_prev = wp->position_fixed;
    wp->position_prev       = wp->position;
    wp->sprite.color_4.word = 0x2C808080;
    if (wp->type_3 & 0x80)
    {
        wp->sprite.value_E = stcom06_frame2_tbl[wp->index_F].a;
        wp->sprite.value_F = stcom06_frame2_tbl[wp->index_F].b;
    }
    else
    {
        wp->sprite.value_E = stcom06_frame_tbl[wp->index_F].a;
        wp->sprite.value_F = stcom06_frame_tbl[wp->index_F].b;
    }
    stcom06_pos_update(wp);
    Obj_pos_adj_round(&wp->position_fixed_prev, &wp->position_prev);
    Obj_pos_adj_round(&wp->position_fixed, &wp->position);
    if (stcom06_bounce(wp) != 0)
    {
        wp->routine_0 = 3;
        wp->timer_D   = 0;
    }
    if (wp->count_C >= 150)
    {
        wp->routine_0 = 3;
        wp->timer_D   = 0;
    }
    else
    {
        wp->count_C++;
    }
    wp->sprite.pos_8[0] = wp->position.x;
    wp->sprite.pos_8[1] = wp->position.y - 0x10;
    wp->sprite.pos_8[2] = wp->position.z;
    wp->value_76        = 0;
}

void stcom06_move_explode(STCOM06_WORK *wp)
{
    STCOM_FIXED_VECTOR unused; // never used; retail reserves its frame
    STCOM_EFFECT *effect;

    wp->sprite.mode_0       = 0x3D48000E;
    wp->sprite.color_4.word = 0x2C808080;
    wp->sprite.pos_8[0] = wp->position.x = wp->position_fixed.x >> 16;
    wp->sprite.pos_8[1] = wp->position.y = wp->position_fixed.y >> 16;
    wp->sprite.pos_8[2] = wp->position.z = wp->position_fixed.z >> 16;
    if (wp->type_3 & 0x80)
    {
        wp->sprite.value_E = stcom06_frame2_tbl[wp->index_F].a;
        wp->sprite.value_F = stcom06_frame2_tbl[wp->index_F].b;
    }
    else
    {
        wp->sprite.value_E = stcom06_frame_tbl[wp->index_F].a;
        wp->sprite.value_F = stcom06_frame_tbl[wp->index_F].b;
    }
    wp->sprite.value_10 = 0x20;
    effect              = Open_EFFECT_WORK();
    if (effect != NULL)
    {
        stcom06_explosion_set(wp, effect);
    }
    wp->flags_0 = 0;
    Close_SHL_WORK(wp);
}

// Lying on the ground: blinks and explodes after 30 frames; a kick sends it flying again.
void stcom06_move_land(STCOM06_WORK *wp)
{
    STCOM_FIXED_VECTOR unused; // never used; retail reserves its frame
    STCOM_EFFECT *effect;
    uint32 hit;
    sint8 timer;
    sint16 blink;

    hit = wp->sprite.hit_14;
    if (hit & 0x20000000)
    {
        wp->velocity.v.x = (-rsin(Player_work.rotation.y) * 0x20) >> 4;
        wp->velocity.v.y = -0x1000;
        wp->velocity.v.z = (-rcos(Player_work.rotation.y) * 0x20) >> 4;
        wp->routine_0 = wp->routine_2 = 1;
        wp->accel.x                   = 0;
        wp->accel.y                   = 0x200;
        wp->accel.z                   = 0;
        wp->sprite.hit_14             = 0;
    }
    else if (wp->routine_2 != 0)
    {
        if (hit & 0x1C000000)
        {
            wp->sprite.hit_14 = 0;
            effect            = Open_EFFECT_WORK();
            if (effect != NULL)
            {
                stcom06_explosion_set(wp, effect);
            }
            wp->flags_0 = 0;
            Close_SHL_WORK(wp);
            return;
        }
    }
    else if (hit & 0x1A000000)
    {
        wp->sprite.hit_14 = 0;
        effect            = Open_EFFECT_WORK();
        if (effect != NULL)
        {
            stcom06_explosion_set(wp, effect);
        }
        wp->flags_0 = 0;
        Close_SHL_WORK(wp);
        return;
    }
    wp->sprite.hit_14 = 0;
    if (wp->velocity.words[0] != 0 || wp->velocity.v.z != 0)
    {
        if (wp->type_3 & 0x80)
        {
            wp->sprite.value_E = stcom06_frame2_tbl[wp->index_F].a;
            wp->sprite.value_F = stcom06_frame2_tbl[wp->index_F].b;
        }
        else
        {
            wp->sprite.value_E = stcom06_frame_tbl[wp->index_F].a;
            wp->sprite.value_F = stcom06_frame_tbl[wp->index_F].b;
        }
    }
    wp->position_fixed_prev = wp->position_fixed;
    wp->position_prev       = wp->position;
    stcom06_pos_update(wp);
    Obj_pos_adj_round(&wp->position_fixed_prev, &wp->position_prev);
    Obj_pos_adj_round(&wp->position_fixed, &wp->position);
    stcom06_bounce(wp);
    wp->sprite.pos_8[0] = wp->position.x;
    wp->sprite.pos_8[1] = wp->position.y - 0x10;
    wp->sprite.pos_8[2] = wp->position.z;
    timer               = wp->timer_D;
    if (timer < 30)
    {
        blink = -1;
        if (timer < 15)
        {
            blink = 1;
        }
        if (timer & blink)
        {
            if (wp->sprite.color_4.rgb[0] == 0xFF)
            {
                wp->sprite.color_4.word = 0x2C808080;
            }
            else
            {
                wp->sprite.color_4.word = 0x2CFFFFFF;
            }
        }
        wp->timer_D++;
        return;
    }
    effect = Open_EFFECT_WORK();
    if (effect != NULL)
    {
        stcom06_explosion_set(wp, effect);
    }
    wp->flags_0 = 0;
    Close_SHL_WORK(wp);
}

void stcom06_func_0934(STCOM06_WORK *wp)
{
    STCOM_FIXED_VECTOR unused; // never used; retail reserves its frame

    wp->sprite.mode_0       = 0x3D48000E;
    wp->sprite.color_4.word = 0x2C808080;
    wp->value_7C            = 0;
    wp->value_76            = 0;
    Obj_pos_adj_round(&wp->position_fixed, &wp->position);
    wp->sprite.pos_8[0]     = wp->position.x;
    wp->sprite.pos_8[1]     = wp->position.y - 0x10;
    wp->sprite.pos_8[2]     = wp->position.z;
    wp->position_fixed_prev = wp->position_fixed;
    wp->position_prev       = wp->position;
    if (wp->type_3 & 0x80)
    {
        wp->sprite.value_E = stcom06_frame2_tbl[wp->index_F].a;
        wp->sprite.value_F = stcom06_frame2_tbl[wp->index_F].b;
    }
    else
    {
        wp->sprite.value_E = stcom06_frame_tbl[wp->index_F].a;
        wp->sprite.value_F = stcom06_frame_tbl[wp->index_F].b;
    }
    wp->sprite.value_10 = 0x20;
    wp->flags_0 |= 2;
}

// Bounces off what it hits; returns 1 when it comes to rest on the floor.
sint16 stcom06_bounce(STCOM06_WORK *wp)
{
    STCOM_EFFECT *effect;
    sint32 hit;
    sint16 landed;

    landed = 0;
    hit    = Scr_collision_check(wp);
    if (hit != 0)
    {
        if (hit & 0x1000)
        {
            if (STCOM_ABS(wp->velocity.v.y) <= 0xA00)
            {
                landed           = 1;
                wp->velocity.v.x = 0;
                wp->velocity.v.y = 0;
                wp->velocity.v.z = 0;
                wp->accel.x      = 0;
                wp->accel.y      = 0;
                wp->accel.z      = 0;
            }
            else
            {
                wp->velocity.v.y = STCOM_ABS(wp->velocity.v.y * 160) >> 8;
                wp->velocity.v.x >>= 1;
                wp->velocity.v.z >>= 1;
            }
            if (Game_work.value_50 == 0xB && wp->position.y == -1)
            {
                Close_PL_SHL_WORK(wp);
                Sound_call2(0x102, &wp->position);
                effect = Open_EFFECT_WORK();
                if (effect != NULL)
                {
                    effect->sprite = wp->sprite;
                }
                stcom06_func_0E68(wp, effect);
            }
            else
            {
                Sound_call2(0xB3, &wp->position);
                effect = Open_EFFECT_WORK();
                if (effect != NULL)
                {
                    effect->flags_0   = 1;
                    effect->kind_2    = 0x83;
                    effect->variant_3 = 0;
                    effect->sprite    = wp->sprite;
                }
            }
        }
        else
        {
            if (Game_work.value_50 != 0xB || wp->position.y != -1)
            {
                Sound_call2(0xB3, &wp->position);
            }
            effect = Open_EFFECT_WORK();
            if (effect != NULL)
            {
                effect->flags_0   = 1;
                effect->kind_2    = 0x83;
                effect->variant_3 = 0;
                effect->sprite    = wp->sprite;
            }
        }
        Scr_vec_reflect(&wp->velocity.v, hit);
    }
    return landed;
}

// Adds the acceleration (velocity clamped to +-0x6000) and moves.
void stcom06_pos_update(STCOM06_WORK *wp)
{
    wp->velocity.v.x += wp->accel.x;
    wp->velocity.v.y += wp->accel.y;
    wp->velocity.v.z += wp->accel.z;
    if (wp->velocity.v.x < -0x6000)
    {
        wp->velocity.v.x = -0x6000;
    }
    else if (wp->velocity.v.x > 0x6000)
    {
        wp->velocity.v.x = 0x6000;
    }
    if (wp->velocity.v.y < -0x6000)
    {
        wp->velocity.v.y = -0x6000;
    }
    else if (wp->velocity.v.y > 0x6000)
    {
        wp->velocity.v.y = 0x6000;
    }
    if (wp->velocity.v.z < -0x6000)
    {
        wp->velocity.v.z = -0x6000;
    }
    else if (wp->velocity.v.z > 0x6000)
    {
        wp->velocity.v.z = 0x6000;
    }
    wp->position_fixed.x += wp->velocity.v.x << 8;
    wp->position_fixed.y += wp->velocity.v.y << 8;
    wp->position_fixed.z += wp->velocity.v.z << 8;
    // Retail reloads and stores the position here (kept: the build needs these stores).
    wp->position.x = wp->position.x;
    wp->position.y = wp->position.y;
    wp->position.z = wp->position.z;
}

void stcom06_explosion_set(STCOM06_WORK *wp, STCOM_EFFECT *effect)
{
    effect->flags_0 = 5;
    effect->kind_2  = 0x80;
    if (wp->routine_2 != 0)
    {
        effect->variant_3 = 2;
    }
    else
    {
        effect->variant_3 = 0x42;
    }
    effect->sprite.position = wp->position;
}

void stcom06_func_0E68(STCOM06_WORK *wp, STCOM_EFFECT *effect)
{
    effect->flags_0         = 1;
    effect->kind_2          = 0x19;
    effect->variant_3       = 0;
    effect->sprite.position = wp->position;
}
