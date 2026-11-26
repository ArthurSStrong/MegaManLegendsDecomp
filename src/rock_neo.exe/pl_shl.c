#include "common.h"
#include "pl_shl.h"

sint32 Pl_shot_busy(CODE43_FLAGS *wp)
{
    if ((wp->flags_174 & 0xC0) || wp->value_177)
    {
        return 1;
    }
    return 0;
}

sint32 Money_check(sint32 amount)
{
    return -(Game_work.money_1c < (uint32)-amount);
}

void Money_add(sint32 amount, sint32 arg1, sint32 alt)
{
    if (arg1 == 0)
    {
        if (amount > 0)
        {
            Game_work.money_1c += amount;
            if (Game_work.money_1c > CODE43_MONEY_MAX)
            {
                Game_work.money_1c = CODE43_MONEY_MAX;
            }
        }
        else
        {
            Game_work.money_1c += amount;
            if (Game_work.money_1c > CODE43_MONEY_MAX)
            {
                Game_work.money_1c = 0;
            }
            if (alt)
            {
                Game_work.spent_24 -= amount;
            }
            else
            {
                Game_work.spent_20 -= amount;
            }
        }
    }
}

void Pl_life_add(sint32 amount, sint32 index)
{
    CODE43_PLAYER *pl = &Player_work[index];

    pl->hp_6e += amount;
    if ((sint16)pl->hp_6e > pl->hp_max_b0)
    {
        pl->hp_6e = pl->hp_max_b0;
    }
    if (amount < 0 && index == 0)
    {
        Game_work.hp_lost_4c -= amount;
    }
}

void Pl_shl_normal_arm(CODE43_WORK *wp)
{
    Pl_shl_normal_arm_tbl[wp->routine_8](wp);
}

void Pl_shl_normal_arm_init(CODE43_WORK *wp)
{
    CODE43_SVECTOR unused[3];
    CODE43_TARGET *target;

    wp->mode_e        = wp->pos_14.pad;
    target            = Obj_id_work(wp->index_c);
    target->state_198 = 0;
    Homing_target_clear(wp->bit_d);
    wp->timer_b = 0xB;
    wp->word_c4 = 0;
    wp->word_c8 = 0;
    wp->ptr_78  = 0;
    wp->routine_8++;
    if (wp->mode_e)
    {
        wp->pos_2c = target->pos_400;
    }
    else
    {
        wp->pos_2c = target->pos_418;
    }
    wp->pos_14 = wp->pos_2c;
    normal_arm_atari_set(wp);
    Sound_call2(0x96, &wp->pos_14);
}

void Pl_shl_normal_arm_main(CODE43_WORK *wp)
{
    CODE43_TARGET *target;

    target     = Obj_id_work(wp->index_c);
    wp->pos_2c = Obj_id_work(wp->index_c)->pos_14;
    if (--wp->timer_b == 0)
    {
        Close_PL_SHL_WORK(wp);
    }
    else if (Shot_map_check(wp, 0))
    {
        normal_arm_hit_effect(wp);
        wp->routine_8++;
        target->state_198 = 1;
    }
    else
    {
        normal_arm_atari_set(wp);
    }
}

void Pl_shl_normal_arm_end(CODE43_WORK *wp)
{
    Obj_id_work(wp->index_c)->state_198 = 0;
    Close_PL_SHL_WORK(wp);
}

void normal_arm_atari_set(CODE43_WORK *wp)
{
    sint32 id;
    CODE43_TARGET *target;

    id     = wp->index_c;
    target = Obj_id_work(id);
    if (wp->mode_e)
    {
        wp->pos_14 = target->pos_400;
        if (id)
        {
            Set_Atari2(&wp->pos_14, 0x20, &wp->pos_2c, 0x20, 0x200, 0x6000, wp->value_1,
                       &wp->word_c4, 0x28000008, 0, 0x80);
        }
        else
        {
            Set_Atari2(&wp->pos_14, 0x20, &wp->pos_2c, 0x20, 0x200, 0x4000, wp->value_1,
                       &wp->word_c4, 0x28000008, 0, 0x80);
        }
    }
    else
    {
        wp->pos_14 = target->pos_418;
        if (id)
        {
            Set_Atari2(&wp->pos_14, 0x20, &wp->pos_2c, 0x20, 0x200, 0x6000, wp->value_1,
                       &wp->word_c4, 0x28000008, 0, 0x80);
        }
        else
        {
            Set_Atari2(&wp->pos_14, 0x20, &wp->pos_2c, 0x20, 0x200, 0x4000, wp->value_1,
                       &wp->word_c4, 0x28000008, 0, 0x80);
        }
    }
    wp->pos_2c = wp->pos_14;
}

void normal_arm_hit_effect(CODE43_WORK *wp)
{
    CODE43_FX *fx;

    fx = Open_EFFECT_WORK();
    if (fx)
    {
        fx->flags_0 = 1;
        fx->kind_2  = 0x84;
        fx->value_3 = 0;
        fx->x_1c    = wp->pos_14.x;
        fx->y_1e    = wp->pos_14.y;
        fx->z_20    = wp->pos_14.z;
    }
}

void Pl_shl_buster(CODE43_WORK *wp)
{
    Pl_shl_buster_tbl[wp->routine_8](wp);
    Solid_mover_set(wp);
}

void Pl_shl_buster_init(CODE43_WORK *wp)
{
    CODE43_SVECTOR rot;
    CODE43_VECTOR vec;
    CODE43_MATRIX mtx;
    sint32 speed;
    uint32 word;

    wp->flags_0 |= 0x2A;
    wp->flags_6 |= 0x20;
    Gauge_buster_recharge_reset();
    wp->word_c4 = 0;
    wp->word_c8 = 0;
    wp->ptr_78  = D_80097DA0;
    rot.vx      = Camera_pitch_get();
    rot.vy      = wp->angle_64 & 0xFFF;
    rot.vz      = 0;
    RotMatrixYXZ(&rot, &mtx);
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = -0x1000;
    ApplyMatrix(&mtx, &rot, &vec);
    rot.vy       = vec.vy;
    rot.vx       = vec.vx;
    rot.vz       = vec.vz;
    speed        = Buster_speed_tbl[Player_work[0].speed_idx_4fe];
    wp->vel_44   = (rot.vx * speed) >> 4;
    wp->vel_46   = (rot.vy * speed) >> 4;
    wp->vel_48   = (rot.vz * speed) >> 4;
    wp->pos_a4   = rot;
    wp->value_4c = 0;
    wp->value_4e = 0;
    wp->value_50 = 0;
    wp->word_88  = 0x3D48002E; // overwritten below; retail stores both
    word         = D_80097DAC[Player_work[0].speed_idx_4fc];
    wp->x_90 = wp->pos_14.x = wp->pos_1c.h.x_1e;
    wp->word_8c             = 0x2E808080;
    wp->y_92 = wp->pos_14.y = wp->pos_1c.h.y_22;
    wp->z_94 = wp->pos_14.z = wp->pos_1c.h.z_26;
    wp->word_88             = word;
    wp->vec_34              = wp->pos_1c.v;
    wp->pos_2c              = wp->pos_14;
    wp->value_96            = 0xC0;
    wp->value_97            = 0x60;
    wp->value_98            = Buster_size_tbl[Player_work[0].speed_idx_4fc];
    wp->value_7c            = 0;
    wp->timer_b             = 0;
    buster_dir_set(wp, &mtx);
    wp->routine_9 = 0;
    wp->routine_8++;
    if (wp->mode_a)
    {
        wp->pos_2c.pad = wp->pos_14.pad = 0x6208;
    }
    else
    {
        wp->pos_2c.pad = wp->pos_14.pad = 0x4208;
    }
    if (wp->value_3 >= 0x80 && (wp->target.w.w_bc | wp->target.w.w_c0) == 0)
    {
        Homing_target_clear(wp->bit_d);
        wp->value_3 = 0;
    }
    buster_atari_set(wp);
    Sound_call2(0x9A, &wp->pos_14);
}

// The first arm ends with its own copy of the timeout tail (Close_PL_SHL_WORK and the
// Homing_target_clear check); cross-jumping then merges the two buster_hit_effect calls into one,
// as in retail.
void Pl_shl_buster_main(CODE43_WORK *wp)
{
    sint32 result;
    sint32 timer;
    sint32 speed;
    CODE43_SVECTOR step;
    CODE43_SVECTOR dir;
    CODE43_VECTOR delta;

    result = Shot_map_check(wp, 0);
    if (result != 0 || wp->value_7c != 0)
    {
        if (result == 0x7FFF)
        {
            buster_hit_effect(wp, 0);
        }
        else
        {
            buster_hit_effect(wp, 1);
        }
        Close_PL_SHL_WORK(wp);
        if (wp->value_3 >= 0x80)
        {
            Homing_target_clear(wp->bit_d);
        }
        return;
    }
    else
    {
        wp->vec_34 = wp->pos_1c.v;
        wp->pos_2c = wp->pos_14;
        timer      = wp->timer_b++;
        if (timer < Player_weapon_dist_calc(Player_work, 1))
        {
            if (wp->value_3 >= 0x80)
            {
                if (wp->target.w.w_bc | wp->target.w.w_c0)
                {
                    delta.vx = wp->target.h.x_bc - (sint16)wp->pos_14.x;
                    delta.vy = wp->target.h.y_be - (sint16)wp->pos_14.y;
                    delta.vz = wp->target.h.z_c0 - (sint16)wp->pos_14.z;
                    if ((uint32)(delta.vx + 0x687E) > 0xD0FC ||
                        (uint32)(delta.vy + 0x687E) > 0xD0FC ||
                        (uint32)(delta.vz + 0x687E) > 0xD0FC)
                    {
                        delta.vx >>= 2;
                        delta.vy >>= 2;
                        delta.vz >>= 2;
                    }
                    VectorNormalS(&delta, &dir);
                    if (wp->timer_b < 4)
                    {
                        Vec_turn_toward(&step, &dir, &wp->pos_a4, 0x424, 0xF746EA4);
                    }
                    else if (wp->timer_b < 9)
                    {
                        Vec_turn_toward(&step, &dir, &wp->pos_a4, 0x8F, 0xFFD813C);
                    }
                    else
                    {
                        Vec_turn_toward(&step, &dir, &wp->pos_a4, 0x47, 0xFFF604C);
                        Homing_target_clear(wp->bit_d);
                        wp->value_3 = 0;
                    }
                    wp->pos_a4 = step;
                    speed      = Buster_speed_tbl[Player_work[0].speed_idx_4fe];
                    wp->vel_44 = (step.vx * speed) >> 4;
                    wp->vel_46 = (step.vy * speed) >> 4;
                    wp->vel_48 = (step.vz * speed) >> 4;
                }
                else
                {
                    Homing_target_clear(wp->bit_d);
                    wp->value_3 = 0;
                }
            }
            wp->pos_1c.v.vx += wp->vel_44 << 8;
            wp->pos_1c.v.vy += wp->vel_46 << 8;
            wp->pos_1c.v.vz += wp->vel_48 << 8;
            wp->x_90 = wp->pos_14.x = wp->pos_1c.h.x_1e;
            wp->y_92 = wp->pos_14.y = wp->pos_1c.h.y_22;
            wp->value_9a += 5;
            wp->z_94 = wp->pos_14.z = wp->pos_1c.h.z_26;
            goto move;
        }
    }
    Close_PL_SHL_WORK(wp);
    if (wp->value_3 >= 0x80)
    {
        Homing_target_clear(wp->bit_d);
    }
    return;
move:
    buster_atari_set(wp);
}

// Empty in retail. Pl_shl_buster_init calls it once with the shot's rotation matrix; the name
// follows that argument (a guess at its intended job, not something the code shows).
void buster_dir_set(CODE43_WORK *wp, CODE43_MATRIX *mtx)
{
}

void buster_hit_effect(CODE43_WORK *wp, sint32 value)
{
    CODE43_FX *fx;

    fx = Open_EFFECT_WORK();
    if (fx)
    {
        fx->flags_0 = 9;
        fx->kind_2  = 0x82;
        fx->x_1c    = wp->pos_14.x;
        fx->y_1e    = wp->pos_14.y;
        fx->z_20    = wp->pos_14.z;
        fx->value_3 = 1;
        fx->value_c = value;
    }
}

void buster_atari_set(CODE43_WORK *wp)
{
    sint32 size;
    sint32 flags;

    size  = wp->value_98;
    flags = Player_weapon_attack_calc(Player_work, 1);
    if (wp->index_c)
    {
        Set_Atari2(&wp->pos_14, size, &wp->pos_2c, size, 0x200, 0x6000, wp->value_1,
                   &wp->word_c4, flags | 0x08000000, 0, 0x80);
    }
    else
    {
        Set_Atari2(&wp->pos_14, size, &wp->pos_2c, size, 0x200, 0x4000, wp->value_1,
                   &wp->word_c4, flags | 0x08000000, 0, 0x80);
    }
}
