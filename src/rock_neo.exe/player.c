#include "common.h"
#include "rock_neo.exe/player.h"
#include "rock_neo.exe/game.h"
#include "rock_neo.exe/code_80014ac0.h"
#include "rock_neo.exe/code_800133d8.h"

#ifdef __wasm32__
void Gauge_show_hold(void);
void Camera_height_set(sint32 arg);
#endif

void Player_init(sint32 arg)
{
    pl_init(&Player_work, arg);
}

#define PL_AT(pl, off, type) (*(type *)((uint8 *)(pl) + (off)))
void pl_init(PL_WORK *pl, sint32 arg)
{
    uint8 *save;

    D_8009888C = 0x80000000;
    save       = Cd_ring_buf;
    if (Game_work.flags_82 & 0x80)
    {
        pl->routine_0 = 0;
        pl->flags06   = 0;
    }
    else
    {
        pl->routine_0          = 3;
        pl->flags06            = 3;
        PL_AT(pl, 0x2, uint16) = 0;
        PL_AT(pl, 0x4, uint16) = 0;
        PL_AT(pl, 0xC, uint32) = 0;
        PL_AT(pl, 0x8, uint32) = 0;
    }
    pl->info78               = &D_80097D90;
    pl->flags_bd             = 0;
    pl->value75              = 0;
    pl->value74              = 0;
    pl->value77              = 0;
    pl->value76              = 0;
    PL_AT(pl, 0x184, uint32) = 0;
    pl->value110             = 0;
    pl->value111             = 0;
    pl->value449             = 0;
    if (pl == &Player_work)
    {
        Player_work.reserved02[0] = 0;
    }
    else
    {
        pl->reserved02[0] = 1;
    }
    Pl_key_type_set(pl, (sint8)Game_work.flags_54);
    if (arg == 0)
    {
        pl->life_max             = 0x50;
        pl->life                 = 0x50;
        pl->reserved16a          = 0;
        pl->flags16b             = 0;
        pl->reserved16c[0]       = 0;
        pl->weapon_right_no      = 0;
        pl->part_index           = 0;
        pl->weapon_left_no       = 0;
        pl->reserved173          = 1;
        pl->reserved44C[0]       = 0;
        pl->reserved44E[0]       = 0;
        PL_AT(pl, 0x450, uint32) = 0;
        func_800177CC((uint8 *)pl + 0x454, (uint8 *)pl + 0x458);
        func_800177CC((uint8 *)pl + 0x474, (uint8 *)pl + 0x4B0);
        Player_weapon_init(pl);
    }
    else
    {
        pl->life = pl->life_max = PL_AT(save, 0x1CE, uint16);
        pl->reserved16a                       = save[0x1D4];
        pl->flags16b                          = save[0x1D5];
        pl->reserved16c[0]                    = save[0x1D6];
        pl->weapon_right_no                   = save[0x1D8];
        pl->weapon_left_no                    = save[0x1D9];
        pl->part_index                        = save[0x1DA];
        pl->reserved173                       = save[0x1DB];
        pl->reserved44C[0]                    = save[0x1DC];
        pl->reserved44E[0]                    = save[0x1DE];
        pl->reserved44E[1]                    = save[0x1DF];
        PL_AT(pl, 0x450, uint32)              = PL_AT(save, 0x208, uint32);
        func_800176DC(save + 0x20C, (uint8 *)pl + 0x454, 1);
        func_800176DC(save + 0x1C80, (uint8 *)pl + 0x474, 4);
        func_800176DC(save + 0x22C, (uint8 *)pl + 0x4F4, 8);
        Player_weapon_refill();
    }
}
#undef PL_AT

void Player_move(void)
{
    uint8 flags;

    if ((sint8)Player_work.routine_0 & Player_work.flags06 & 1)
    {
        flags = Player_work.flags_bd;
        if (!(flags & 0x40))
        {
            if (!(flags & 0x20))
            {
                Player_work.pad_cur  = D_800C0C26;
                Player_work.pad_trg  = D_800C0C2A;
                Player_work.value120 = D_800C0C2C;
                Player_work.value122 = D_800C0C2E;
            }
            else
            {
                Player_work.pad_cur  = Pl_demo_pad[0];
                Player_work.pad_trg  = D_800C0F9A;
                Player_work.value120 = D_800C0F9C;
                Player_work.value122 = D_800C0F9E;
                if (flags & 0x10)
                {
                    Player_work.position       = Player_work.position_saved;
                    Player_work.position_fixed = Player_work.position_saved_fixed;
                    Player_work.rotation       = Player_work.rotation_saved;
                    Player_work.flags_bd &= 0xEF;
                }
            }
        }
        else
        {
            *(uint32 *)&Player_work.pad_cur  = 0;
            *(uint32 *)&Player_work.value120 = 0;
            if (flags & 0x10)
            {
                Player_work.position       = Player_work.position_saved;
                Player_work.position_fixed = Player_work.position_saved_fixed;
                Player_work.rotation       = Player_work.rotation_saved;
                Player_work.flags_bd &= 0xEF;
            }
        }
        pl_move_exec(&Player_work);
    }
    if ((sint8)Player_work2.routine_0 & Player_work2.flags06 & 1)
    {
        if (!(Player_work2.flags_bd & 0x40))
        {
            Player_work2.pad_cur  = D_800C0FE6;
            Player_work2.pad_trg  = D_800C0FEA;
            Player_work2.value120 = D_800C0FEC;
            Player_work2.value122 = D_800C0FEE;
        }
        else
        {
            *(uint32 *)&Player_work2.pad_cur  = 0;
            *(uint32 *)&Player_work2.value120 = 0;
        }
        pl_move_exec(&Player_work2);
    }
}

void Player_atari_set(void)
{
    if ((sint8)Player_work.routine_0 & Player_work.flags06 & 1)
    {
        pl_world_update(&Player_work);
        if (Player_work.value449 == 0 && Player_work.value110 == 0)
        {
            Set_Atari(&Player_work.table3a8[8], 0x20, 0, 0, Player_work.value01,
                          &Player_work.value188, 0x2000000, 0x800000, 0);
        }
    }
    if ((sint8)Player_work2.routine_0 & Player_work2.flags06 & 1)
    {
        pl_world_update(&Player_work2);
        if (Player_work2.value449 == 0 && Player_work2.value110 == 0)
        {
            Set_Atari(&Player_work2.table3a8[8], 0x20, 0, 0x2000, Player_work2.value01,
                          &Player_work2.value188, 0x2000000, 0x800000, 0);
        }
    }
}

void pl_move_exec(PL_WORK *pl)
{
    Pl_move_tbl[pl->kind](pl);
    if (Pl_shot_busy(pl) == 0)
    {
        pl->value104 = pl->value_ac;
        pl->value100 = pl->value_a4;
    }
    if (pl->value104 != pl->value105)
    {
        Pl_Char_Set_Upper(pl, pl->shot_status_flag & 3, pl->value104, pl->value100);
        pl->value105 = pl->value104;
    }
    else
    {
        Pl_Char_Move_Upper(pl, pl->shot_status_flag & 3, pl->value104);
    }
    if (pl->value_ac != pl->value_ad)
    {
        Pl_Char_Set(pl, pl->value_ac, pl->value_a4);
        return;
    }
    Pl_Char_Move(pl, pl->value_ac);
}

void pl_world_update(PL_WORK *pl)
{
    sint32 y;
    sint32 floor;

    if (pl->value74 == 1 && (pl->value7c & 0x2000) && (uint32)(pl->routine_1 - 7) < 2 &&
        pl->routine_2 < 4)
    {
        pl_anim_set(pl, 0x15, 0, 1);
        pl->routine_2 = 4;
        pl->value46   = 0;
        pl->value4e   = 0;
    }
    if ((uint32)(pl->kind - 1) < 2 && pl->value76 != 0)
    {
        Ride_carry(pl, pl->value_c0);
    }
    if (pl->kind == 3 && pl->value76 != 0)
    {
        Ride_carry_saved(pl, pl->value_c0);
    }
    if (Sce_flag_test(0x200) == 0)
    {
        Scr_event_check(pl);
    }
    if ((uint32)(pl->kind - 1) < 2 && !(*(uint32 *)&pl->value74 & 0xFF00FF))
    {
        y     = pl->position.y;
        floor = Scr_height_check(pl) - pl->info78->value0a;
        if (y < floor - 0x20)
        {
            if (pl->value75 == 0)
            {
                if (pl->routine_1 == 0xE)
                {
                    func_800394AC();
                    pl->value108 = 0;
                }
                if (pl->routine_1 == 3)
                {
                    if (pl->routine_2 < 5)
                    {
                        Sound_call(0x8E, 0, 0);
                        Sound_call(0x8F, 0, 0);
                    }
                }
                pl->routine_1             = 7;
                *(uint16 *)&pl->routine_2 = 4;
                pl->value46               = 0;
                pl->value168              = 0;
                pl->value169              = 0;
                pl->value110              = 1;
                pl->value111              = 0;
            }
            pl->value74 = 1;
        }
        else if (pl->position.y < floor)
        {
            pl->position.y       = floor;
            pl->position_fixed.y = floor << 16;
        }
    }
    if (Scr_collision_check(pl) < 0)
    {
        pl->value48          = 0;
        pl->value44          = 0;
        pl->position.x       = pl->position_prev.x;
        pl->position.z       = pl->position_prev.z;
        pl->position_fixed.x = pl->position_fixed_prev.x;
        pl->position_fixed.z = pl->position_fixed_prev.z;
        if (pl->value74 != 0)
        {
            pl->value74          = 0;
            pl->value46          = 0;
            pl->position.y       = pl->position_prev.y;
            pl->position_fixed.y = pl->position_fixed_prev.y;
        }
    }
    func_8002D004(pl, pl->value168, pl->value169);
#ifdef __wasm32__
    Pl_key_unpack(pl, 0);
#else
    Pl_key_unpack(pl);
#endif
    Pl_World_Pos_Calc(pl);
    if (pl->routine_0 & 2)
    {
        Pl_Shadow_Trans(pl, 0x32, 0x2AB, 0);
    }
}

void Pl_move_init(PL_WORK *pl)
{
    *(uint32 *)&pl->kind          = 1;
    pl->value6c                   = 0x3C;
    pl->value44                   = 0;
    pl->value46                   = 0;
    pl->value48                   = 0;
    pl->value114                  = 0;
    pl->value116                  = 0;
    pl->value118                  = 0;
    pl->value112                  = 0;
    pl->value113                  = 0;
    pl->value110                  = 0;
    pl->value71                   = 0;
    pl->value73                   = 0;
    pl->value188                  = 0;
    pl->value18c                  = 0;
    pl->value190                  = 0;
    pl->value194                  = 0;
    pl->value168                  = 0;
    pl->value169                  = 0;
    pl->value104                  = 0;
    pl->value_ac                  = 0;
    pl->value105                  = 0;
    pl->value_ad                  = 0;
    pl->shot_status_flag          = 0;
    pl->value177                  = 0;
    pl->value175                  = 7;
    pl->value108                  = 0;
    pl->value10a                  = 0;
    pl->value44b                  = 0;
    pl->value44a                  = 0;
    *(uint32 *)&pl->table180[0].z = 0;
    pl->flags06 |= 3;
    pl_anim_set(pl, 0, 0, 1);
    Pl_Char_Set_Upper(pl, 0, 5, 0);
}

// Word-sized locals with narrowing casts load life/74/76 straight into a1-a3. The
// value_ac copy goes through mode, which is assigned again later: a pseudo set twice isn't a
// sched1 "birthing" insn, so its load stays first, as in retail.
void Pl_move_main(PL_WORK *pl)
{
    uint32 a1;
    uint32 a2;
    uint32 a3;
    sint32 hit;
    sint8 timer;
    sint32 mode;

    pl->position_prev       = pl->position;
    pl->position_fixed_prev = pl->position_fixed;
    pl->rotation_prev       = pl->rotation;
    mode                    = pl->value_ac;
    pl->value_ad            = mode;
    a1                      = (uint16)pl->life;
    a2                      = (uint8)pl->value74;
    a3                      = (uint8)pl->value76;
    pl->value_b2            = a1;
    pl->value75             = a2;
    pl->value77             = a3;
    pl->flags_bd &= 0xF7;
    hit = func_8004AF28(&pl->position, a1, a2, a3);
    if (hit != 0)
    {
        pl->routine_1 = 0xA;
        pl->routine_2 = 0;
        pl->routine_3 = hit;
    }
    else if ((*(uint32 *)&pl->kind & 0xFFFF00) == 0xA00)
    {
        pl->routine_2 = 1;
    }
    if (pl->value449 == 0)
    {
        if (pl->value110 == 0)
        {
            pl->flags06 |= 3;
            pl_damage_check(pl);
        }
        else if (pl->value110 > 0)
        {
            timer = --pl->value111;
            if (timer < 0)
            {
                pl->value71  = 0;
                pl->value110 = 0;
                pl->flags06 |= 3;
            }
            else if (timer & 2)
            {
                pl->flags06 &= 0xFD;
            }
            else
            {
                pl->flags06 |= 3;
            }
        }
        else
        {
            pl->flags06 |= 3;
        }
    }
    else
    {
        pl->flags06 |= 3;
    }
    mode         = pl->shot_status_flag & 0xC0;
    pl->value188 = 0;
    if (mode != 0)
    {
        if (mode == 0x80)
        {
            Pl_shot_buster(pl);
        }
        else
        {
            Pl_shot_special(pl);
        }
    }
    if (!((sint8)pl->flags_bd & 0x80))
    {
        Pl_r1_check_tbl[pl->routine_1](pl);
        Pl_r1_tbl[pl->routine_1](pl);
    }
    hit = Scr_collision_check(pl);
    if (pl->value74 == 1 && (hit & 0x2000) && (uint32)(pl->routine_1 - 7) < 2 && pl->routine_2 < 4)
    {
        pl_anim_set(pl, 0x15, 0, 1);
        pl->routine_2 = 4;
        pl->value46   = 0;
        pl->value4e   = 0;
    }
    Solid_mover_set(pl);
}

void Pl_r1_stand(PL_WORK *pl)
{
    if (pl->routine_2 == 0)
    {
        pl->routine_2 = 1;
        pl->value44   = 0;
        pl->value48   = 0;
        pl->shot_status_flag &= 0xFC;
        if (!(pl->shot_status_flag & 0xC0) && pl->value177 == 0)
        {
            Pl00_shot_enable_on(pl);
            pl_anim_set(pl, 0, 0, 0);
        }
        else if (pl->shot_status_flag & 0x40)
        {
            pl_anim_set(pl, 0x40, pl->value100, 0);
        }
        else
        {
            pl_anim_set(pl, 0x60, pl->value100, 0);
        }
    }
    else
    {
        pl->value44d = 0;
        pl->value_b4 = 0;
        if (!(pl->shot_status_flag & 0xC0) && pl->value177 == 0)
        {
            pl_anim_set(pl, 0, 0, 0);
        }
    }
}

void Pl_r1_walk(PL_WORK *pl)
{
    uint8 flags;

    if (pl->routine_2 == 0)
    {
        pl->routine_2 = 1;
        pl->value48   = 0;
        if (pl->routine_3 == 0)
        {
            pl->value44 = 0x50;
        }
        else
        {
            pl->value44 = -0x50;
        }
        flags                = pl->shot_status_flag | 4;
        pl->shot_status_flag = flags;
        if ((flags & 0xC0) || pl->value177 != 0)
        {
            if (flags & 0x40)
            {
                pl_anim_set(pl, 0x40, pl->value100, 0);
            }
            else
            {
                pl_anim_set(pl, 0x60, pl->value100, 0);
            }
            return;
        }
    }
    if (!(pl->shot_status_flag & 0xC0) && pl->value177 == 0)
    {
        if (pl->routine_3 == 0)
        {
            pl_anim_set(pl, 0xC, 0, 0);
            pl_turn_l1r1(pl, 0x20, 0x20);
        }
        else
        {
            pl_anim_set(pl, 0xD, 0, 0);
            pl_turn_l1r1(pl, 0x40, 0x40);
        }
        if (pl->value_a6 & 0x80)
        {
            Sound_call(0x90, 0, 0);
        }
        Pl_move(pl, pl->value44, 0, 0);
    }
}

void Pl_r1_run(PL_WORK *pl)
{
    sint32 id;

    id = 0;
    switch (pl->routine_2)
    {
        case 0:
            switch ((sint8)pl->value112)
            {
                case 0:
                    pl->value116 = 0;
                    switch ((sint8)pl->value113)
                    {
                        case 0:
                            id = 4;
                            break;
                        case 1:
                            id = 5;
                            Pl00_shot_enable_off(pl);
                            break;
                        case 2:
                            id = 9;
                            Pl00_shot_enable_off(pl);
                            break;
                        case 3:
                            id = 7;
                            Pl00_shot_enable_off(pl);
                            break;
                    }
                    break;
                case 1:
                    pl->value116 = 0x400;
                    switch ((sint8)pl->value113)
                    {
                        case 0:
                            id = 7;
                            break;
                        case 2:
                            id = 5;
                            break;
                        case 3:
                            id = 9;
                            break;
                    }
                    Pl00_shot_enable_off(pl);
                    break;
                case 2:
                    pl->value116 = 0x800;
                    switch ((sint8)pl->value113)
                    {
                        case 0:
                            id = 9;
                            break;
                        case 1:
                            id = 7;
                            break;
                        case 3:
                            id = 5;
                            break;
                    }
                    Pl00_shot_enable_off(pl);
                    break;
                case 3:
                    // Signed store: through the uint16 field GCC loads 0xFC00 with ori.
                    *(sint16 *)&pl->value116 = -0x400;
                    switch ((sint8)pl->value113)
                    {
                        case 0:
                            id = 5;
                            break;
                        case 1:
                            id = 9;
                            break;
                        case 2:
                            id = 7;
                            break;
                    }
                    Pl00_shot_enable_off(pl);
                    break;
            }
            pl->value44  = 0x200;
            pl->value113 = 0;
            pl->routine_2 += 2;
            pl->shot_status_flag |= 4;
            pl_anim_set(pl, id, 0, 1);
            pl_run_move(pl, (sint8)pl->value112);
            break;
        case 1:
            if (pl->value_a4 == 1)
            {
                pl->routine_2++;
            }
            break;
        case 2:
            if (pl->value_a7 == -1)
            {
                pl->value44d = 1;
                pl->routine_2 += 2;
                Pl00_shot_enable_on(pl);
                if ((sint8)pl->value112 != 1)
                {
                    pl_anim_set(pl, 1, 0, 0);
                }
                else
                {
                    pl_anim_set(pl, 1, 9, 0);
                }
            }
            pl_run_move(pl, (sint8)pl->value112);
            break;
        case 3:
            if (pl->value_a4 == 5)
            {
                pl->value44d = 1;
                pl->routine_2++;
            }
            // fall through
        case 4:
            pl_turn_l1r1(pl, 0x28, 0x28);
            pl_run_move(pl, (sint8)pl->value112);
            if (pl->value_a6 & 0x80)
            {
                Sound_call(0x91, 0, 0);
            }
            break;
        case 5:
            switch ((sint8)pl->value112)
            {
                case 0:
                    pl->routine_1             = 0;
                    *(uint16 *)&pl->routine_2 = 0;
                    break;
                case 1:
                    Pl00_shot_enable_off(pl);
                    pl_anim_set(pl, 8, 0, 0);
                    break;
                case 2:
                    Pl00_shot_enable_off(pl);
                    pl_anim_set(pl, 0xA, 0, 0);
                    break;
                case 3:
                    Pl00_shot_enable_off(pl);
                    pl_anim_set(pl, 6, 0, 0);
                    break;
            }
            *(sint16 *)&pl->reserved4A[2] = 0x30;
            pl->routine_2++;
            pl_run_move(pl, (sint8)pl->value112);
            break;
        case 6:
            if (pl->value_a6 == 1)
            {
                pl->routine_1             = 0;
                *(uint16 *)&pl->routine_2 = 0;
                pl->value116              = 0;
                pl->value112              = 0;
                pl->value44               = 0;
                pl->value_b4              = 0;
                Pl00_shot_enable_on(pl);
                pl_anim_set(pl, 0, 0, 0);
            }
            break;
    }
}

void pl_run_move(PL_WORK *pl, sint32 index)
{
    uint16 pad;
    uint32 hit;
    uint32 mask;
    uint32 dir;

    pad  = pl->pad_cur;
    mask = pl->pad_tbl[(sint16)index].mask_b;
    dir  = pl->pad_tbl[(sint16)index].dir_b;
    hit  = (pad & 0xFFFF) & mask;
    if (hit != 0 && hit != mask)
    {
        if ((pad & 0xFFFF) & dir)
        {
            pl->value48 = -0x140;
            if ((pad & (pl->key_turn_l | pl->key_turn_r)) == 0)
            {
                pl->value_b4 = 0x40;
            }
        }
        else
        {
            pl->value48 = 0x140;
            if ((pad & (pl->key_turn_l | pl->key_turn_r)) == 0)
            {
                pl->value_b4 = -0x40;
            }
        }
    }
    else
    {
        pl->value48 = 0;
    }
    Pl_move(pl, pl->value44, 0, pl->value48);
}

void Pl_r1_super_dash(PL_WORK *pl)
{
    uint16 pad;
    uint16 trig;
    sint32 mode;
    uint16 t;
    PL_EFFECT *fx;

    pad  = pl->pad_cur;
    trig = pl->pad_trg;
    switch (pl->routine_2)
    {
        case 0:
            pl->shot_status_flag |= 4;
            pl_anim_set(pl, 0x38, 0, 0);
            pl->value44a++;
            pl->routine_2 += 2;
            break;
        case 1:
            if (pl->key_turn_l & pad)
            {
                fx = (PL_EFFECT *)Open_EFFECT_WORK();
                if (fx != NULL)
                {
                    fx->routine_0 = 1;
                    fx->value02   = 0x89;
                    fx->value03   = 0;
                }
            }
            if (pl->key_turn_r & pad)
            {
                fx = (PL_EFFECT *)Open_EFFECT_WORK();
                if (fx != NULL)
                {
                    fx->routine_0 = 1;
                    fx->value02   = 0x89;
                    fx->value03   = 1;
                }
            }
            pl->routine_2++;
            // fall through
        case 2:
            if (pl->value_a6 == 0)
            {
                pl->value44a++;
                break;
            }
            pl->routine_2 = 3;
            pl->value44   = 0x340;
            // fall through
        case 3:
            if (pl->value_a7 != -1)
            {
                Pl_move(pl, pl->value44, 0, 0);
                break;
            }
            Sound_call(0x8C, 0, 0);
            pl->routine_2++;
            // fall through
        case 4:
            if ((pl->key_forward | pl->key_back) & pad)
            {
                if (pad & pl->key_forward)
                {
                    mode        = pl_turn_l1r1(pl, 0x10, 0x10);
                    pl->value44 = 0x400;
                    pl->value48 = 0xC0;
                }
                else
                {
                    mode        = pl_turn_l1r1(pl, 0x20, 0x20);
                    pl->value44 = 0x280;
                    pl->value48 = 0x140;
                    fx          = (PL_EFFECT *)Open_EFFECT_WORK();
                    if (fx != NULL)
                    {
                        pl_effect_pos_set(pl, fx, 1);
                    }
                    fx = (PL_EFFECT *)Open_EFFECT_WORK();
                    if (fx != NULL)
                    {
                        pl_effect_pos_set(pl, fx, 2);
                    }
                }
            }
            else
            {
                mode        = pl_turn_l1r1(pl, 0x18, 0x18);
                pl->value44 = 0x340;
                pl->value48 = 0x100;
            }
            t = (pl->key_run_l | pl->key_run_r) & pad;
            if (t != 0 && t != (pl->key_run_l | pl->key_run_r))
            {
                if (pad & pl->key_run_r)
                {
                    if (mode == 0)
                    {
                        mode = 1;
                    }
                    pl->value48 = -pl->value48;
                    if (!((pl->key_turn_l | pl->key_turn_r) & pad))
                    {
                        pl->value_b4 = 0x40;
                    }
                }
                else
                {
                    if (mode == 0)
                    {
                        mode = 2;
                    }
                    if (!((pl->key_turn_l | pl->key_turn_r) & pad))
                    {
                        pl->value_b4 = -0x40;
                    }
                }
            }
            else
            {
                pl->value48 = 0;
            }
            switch (mode)
            {
                case 0:
                    pl_anim_set(pl, 0x39, 0, 0);
                    Sound_call(0x8F, 0, 0);
                    break;
                case 1:
                    pl_anim_set(pl, 0x3C, 0, 0);
                    if (pl->key_turn_l & pad)
                    {
                        Sound_call(0x8F, 0, 0);
                    }
                    break;
                case 2:
                    pl_anim_set(pl, 0x3D, 0, 0);
                    if (pl->key_turn_r & pad)
                    {
                        Sound_call(0x8F, 0, 0);
                    }
                    break;
            }
            Pl_move(pl, pl->value44, 0, pl->value48);
            pl->value44d = 1;
            if (!(pl->key_dash & pad) || !(pl->flags16b & 2))
            {
                Sound_call(0x8E, 0, 0);
                Sound_call(0x8F, 0, 0);
                pl_anim_set(pl, 0x3A, 0, 0);
                pl->routine_2++;
            }
            break;
        case 5:
            pl->value44 -= 0x28;
            pl_turn_l1r1(pl, 0x20, 0x20);
            Pl_move(pl, pl->value44, 0, 0);
            if (pl->value_a7 == -1)
            {
                pl->routine_1             = 0;
                *(uint16 *)&pl->routine_2 = 0;
                pl->value44               = 0;
                pl->value44a              = 0;
            }
            break;
    }
    if (pl->routine_2 == 3 || pl->routine_2 == 4)
    {
        if (pl->key_turn_r & trig)
        {
            fx = (PL_EFFECT *)Open_EFFECT_WORK();
            if (fx != NULL)
            {
                fx->routine_0 = 1;
                fx->value02   = 0x89;
                fx->value03   = 1;
            }
        }
        if (pl->key_turn_l & trig)
        {
            fx = (PL_EFFECT *)Open_EFFECT_WORK();
            if (fx != NULL)
            {
                fx->routine_0 = 1;
                fx->value02   = 0x89;
                fx->value03   = 0;
            }
        }
    }
}

void Pl_r1_turn(PL_WORK *pl)
{
    uint32 pad;
    sint32 arg1;
    sint32 arg2;
    sint32 mask;

    pad = pl->pad_cur;
    if (pl->routine_2 == 0)
    {
        pl->routine_2++;
        pl->value_b6 = 0;
        pl->value44d = 0;
    }
    arg1 = 2;
    if (pl->key_dash & pad)
    {
        arg2 = 0x20;
    }
    else
    {
        arg1 = 4;
        arg2 = 0x48;
    }
    if (!(pl->shot_status_flag & 0xC0))
    {
        mask = pl->key_turn_l | pl->key_turn_r;
        if ((pad & mask) != mask)
        {
            if (pl->key_turn_l & pad) // both arms identical in retail too
            {
                pl_anim_set(pl, 0xD, 0, 0);
            }
            else
            {
                pl_anim_set(pl, 0xD, 0, 0);
            }
        }
        else
        {
            pl_anim_set(pl, 0, 0, 0);
        }
    }
    else if (pl->shot_status_flag & 0x80)
    {
        pl_anim_set(pl, 0x60, 0, 0);
    }
    else
    {
        pl_anim_set(pl, 0x40, 0, 0);
    }
    pl_turn_l1r1(pl, arg1, arg2);
}

void func_8003D7D4(PL_WORK *pl)
{
    uint32 pad;
    sint16 hold;

    switch (pl->routine_2)
    {
        case 0:
            switch ((sint8)pl->value112)
            {
                case 1:
                    pl->value10e = -0x80;
                    pl->value164 = pl->key_run_l;
                    break;
                case 2:
                    pl->value10e = 0x100;
                    pl->value164 = pl->key_back;
                    break;
                case 3:
                    pl->value10e = 0x80;
                    pl->value164 = pl->key_run_r;
                    break;
            }
            pl->routine_2++;
            // fall through
        case 1:
            pad  = pl->pad_cur;
            hold = pl->value164;
            if (!(pad & hold) ||
                (pad & (pl->key_forward | pl->key_back | (pl->key_run_l | pl->key_run_r)) & ~hold))
            {
                pl->value164 = 0;
            }
            pl->rotation.y = (pl->rotation.y + pl->value10e) & 0xFFF;
            pl->value116   = (pl->value116 + pl->value10e) & 0xFFF;
            if (pl->value116 == 0)
            {
                pl->value112  = 0;
                pl->routine_1 = 0;
                if (pl->value164 != 0)
                {
                    *(uint16 *)&pl->routine_2 = 0x100;
                }
                else
                {
                    *(uint16 *)&pl->routine_2 = 0;
                }
                Pl00_shot_enable_on(pl);
            }
            break;
    }
    if (!(pl->shot_status_flag & 0xC0) && pl->value177 == 0)
    {
        pl->value_b4 = 0;
        pl_anim_set(pl, 0, 0, 0);
    }
}

void Pl_r1_side_step(PL_WORK *pl)
{
    uint32 pad;
    uint32 mask;
    uint32 right;

    pad = pl->pad_cur;
    if (pl->routine_2 == 0)
    {
        pl->routine_2 = 1;
        pl->value_b4  = 0;
        pl->shot_status_flag |= 4;
    }
    right = pl->key_step_l;
    mask  = right | pl->key_step_r;
    if ((pad & mask) != mask)
    {
        if (right & pad)
        {
            pl->value48 = 0x20;
            pl_anim_set(pl, 0xF, 0, 0);
        }
        else
        {
            pl->value48 = -0x20;
            pl_anim_set(pl, 0xE, 0, 0);
        }
    }
    else
    {
        pl->value48 = 0;
    }
    Pl_move(pl, 0, 0, pl->value48);
    if (pl->value_a4 == 8)
    {
        Sound_call(0x95, 0, 0);
    }
}

void Pl_r1_jump(PL_WORK *pl)
{
    uint16 pad;

    pad = pl->pad_cur;
    switch (pl->routine_2)
    {
        case 0:
            pl->routine_2++;
            pl->value112 = 0;
            pl->value116 = 0;
            pl->value_b4 = 0;
            if (!(pad & pl->pad_tbl[0].mask_b))
            {
                pl->value48 = 0;
            }
            Pl00_shot_enable_off(pl);
            pl_anim_set(pl, 0x10, 0, 1);
            break;
        case 1:
            if (pl->value_a7 == -1)
            {
                pl->value74 = 1;
                pl->routine_2++;
                pl->value_ad = pl->value_ac;
                if (pl->flags16b & 1)
                {
                    pl->value46 = -0x320;
                }
                else
                {
                    pl->value46 = -0x1C0;
                }
                Pl00_shot_enable_on(pl);
                pl_anim_set(pl, 0x11, 0, 1);
                pl_air_move(pl, 0);
                Sound_call(0x92, 0, 0);
            }
            break;
        case 2:
            pl_air_move(pl, (sint8)pl->value112);
            if (pad & pl->key_jump)
            {
                pl->value46 += 0x30;
                if (pl->value46 >= 0)
                {
                    pl_anim_set(pl, 0x12, 0, 1);
                    pl->routine_2 += 2;
                }
            }
            else
            {
                pl->routine_2++;
            }
            break;
        case 3:
            pl_air_move(pl, (sint8)pl->value112);
            if (-pl->value46 > 0x240)
            {
                pl->value46 += 0x240;
            }
            else
            {
                pl_anim_set(pl, 0x12, 0, 1);
                pl->value46 = 0;
                pl->routine_2 += 2;
            }
            break;
        case 4:
            pl_anim_set(pl, 0x12, 0, 1);
            pl->routine_2++;
            // fall through
        case 5:
            pl_air_move(pl, (sint8)pl->value112);
            pl->value46 += 0x30;
            break;
        case 6:
            break;
    }
}

void Pl_r1_run_jump(PL_WORK *pl)
{
    uint16 pad;

    pad = pl->pad_cur;
    switch (pl->routine_2)
    {
        case 0:
            pl->value74  = 1;
            pl->value44d = 1;
            pl->routine_2 += 2;
            pl->value_ad = pl->value_ac;
            if (pl->flags16b & 1)
            {
                pl->value46 = -0x320;
            }
            else
            {
                pl->value46 = -0x1C0;
            }
            if (!(pad & pl->pad_tbl[(sint8)pl->value112].mask_b))
            {
                pl->value48 = 0;
            }
            Pl00_shot_enable_on(pl);
            pl_anim_set(pl, 0x14, 0, 1);
            pl_air_move(pl, (sint8)pl->value112);
            Sound_call(0x92, 0, 0);
            break;
        case 1:
        case 2:
            pl_air_move(pl, (sint8)pl->value112);
            if (pad & pl->key_jump)
            {
                pl->value46 += 0x30;
                if (pl->value46 >= 0)
                {
                    pl_anim_set(pl, 0x15, 0, 1);
                    pl->routine_2 += 3;
                }
            }
            else
            {
                pl->routine_2++;
            }
            break;
        case 3:
            pl_air_move(pl, (sint8)pl->value112);
            if (-pl->value46 > 0x240)
            {
                pl->value46 += 0x240;
            }
            else
            {
                pl_anim_set(pl, 0x15, 0, 1);
                pl->value46 = 0;
                pl->routine_2 += 2;
            }
            break;
        case 4:
            pl_anim_set(pl, 0x12, 0, 1);
            pl->value44 = 0x200;
            pl->routine_2++;
            // fall through
        case 5:
            pl_air_move(pl, (sint8)pl->value112);
            pl->value46 += 0x30;
            break;
        case 6:
            if (pl->value_a7 != -1)
            {
                break;
            }
            Pl00_shot_enable_off(pl);
            switch ((sint8)pl->value112)
            {
                case 0:
                    if (!(pad & pl->key_dash))
                    {
                        if (pad & pl->key_forward)
                        {
                            goto land;
                        }
                        pl_anim_set(pl, 0x2A, 0, 0);
                    }
                    else if (pl->value44a > 0x10)
                    {
                        pl->routine_1             = 3;
                        *(uint16 *)&pl->routine_2 = 1;
                        pl->value44               = 0x300;
                        pl->value48               = 0;
                        pl_anim_set(pl, 0x3B, 0, 0);
                        return;
                    }
                    else
                    {
                        pl_anim_set(pl, 0x2A, 0, 0);
                    }
                    pl->routine_2++;
                    return;
                case 1:
                    if (!(pad & pl->key_dash) && (pad & pl->key_run_l))
                    {
                        goto land;
                    }
                    pl_anim_set(pl, 0x2B, 0, 0);
                    pl->routine_2++;
                    return;
                case 2:
                    if (!(pad & pl->key_dash) && (pad & pl->key_back))
                    {
                        goto land;
                    }
                    pl_anim_set(pl, 0x2D, 0, 0);
                    pl->routine_2++;
                    return;
                case 3:
                    if (!(pad & pl->key_dash) && (pad & pl->key_run_r))
                    {
                        goto land;
                    }
                    pl_anim_set(pl, 0x2C, 0, 0);
                    pl->routine_2++;
                    return;
            }
        land:
            pl->value44               = 0x200;
            pl->routine_1             = 2;
            pl->value46               = 0;
            *(uint16 *)&pl->routine_2 = 2;
            pl_anim_set(pl, 0x17, 0, 0);
            break;
        case 7:
            if (pl->value_a7 == -1)
            {
                pl->routine_1             = 0;
                *(uint16 *)&pl->routine_2 = 0;
                pl->value44               = 0;
                pl->value48               = 0;
                pl->value44a              = 0;
                Pl00_shot_enable_on(pl);
                pl_anim_set(pl, 0, 0, 0);
                pl->value116 = 0;
                pl->value112 = 0;
                pl->value_b4 = 0;
            }
            break;
    }
}

void pl_air_move(PL_WORK *pl, sint32 index)
{
    uint16 pad;
    sint32 mask;
    sint32 dir;
    sint32 hit;

    mask = (&pl->pad_tbl[(sint16)index])->mask_a;
    pad  = pl->pad_cur;
    dir  = (&pl->pad_tbl[(sint16)index])->dir_a;
    hit  = pad & mask;
    if (hit != 0 && hit != mask)
    {
        if (pad & dir)
        {
            pl->value44 += 0x28;
            if (pl->value44 > 0x280)
            {
                pl->value44 = 0x280;
            }
        }
        else
        {
            pl->value44 -= 0x28;
            if (pl->value44 < -0x280)
            {
                pl->value44 = -0x280;
            }
        }
    }
    mask = pl->pad_tbl[(sint16)index].mask_b;
    dir  = pl->pad_tbl[(sint16)index].dir_b;
    hit  = pad & mask;
    if (hit != 0)
    {
        if (hit != mask)
        {
            if (pad & dir)
            {
                pl->value48 -= 0x20;
                if (pl->value48 < -0x100)
                {
                    pl->value48 = -0x100;
                }
            }
            else
            {
                pl->value48 += 0x20;
                if (pl->value48 > 0x100)
                {
                    pl->value48 = 0x100;
                }
            }
        }
        else
        {
            pl->value48 = 0;
        }
    }
    pl_turn_l1r1(pl, 4, 0x20);
    Pl_move(pl, pl->value44, pl->value46, pl->value48);
    if (pl->value74 == 0)
    {
        if (pl->routine_1 == 7)
        {
            pl_anim_set(pl, 0x13, 0, 1);
        }
        else
        {
            pl_anim_set(pl, 0x16, 0, 1);
        }
        Sound_call(0x93, 0, 0);
        pl->routine_1 = 8;
        pl->routine_2 = 6;
    }
}

void Pl_r1_side_jump(PL_WORK *pl)
{
    switch (pl->routine_2)
    {
        case 0:
            pl->value110 = -1;
            if (pl->value_a7 != -1)
            {
                return;
            }
            pl->value74 = 1;
            pl->value44 = 0;
            pl->value46 = 0;
            pl->routine_2++;
            if (pl->routine_3 != 0)
            {
                pl->value48 = -0x200;
                pl_anim_set(pl, 0x18, 0, 1);
            }
            else
            {
                pl->value48 = 0x200;
                pl_anim_set(pl, 0x19, 0, 1);
            }
            Sound_call(0x92, 0, 0);
            return;
        case 1:
            if (pl->value_a7 == -1)
            {
                pl->value44 = 0x200;
                pl->value46 = -0x80;
                pl->value48 = 0;
                pl->routine_2++;
                if (pl->routine_3 != 0)
                {
                    pl->value112 = 3;
                    // Signed store: through the uint16 field GCC loads 0xFC00 with ori.
                    *(sint16 *)&pl->value116 = -0x400;
                }
                else
                {
                    pl->value112 = 1;
                    pl->value116 = 0x400;
                }
                pl_anim_set(pl, 0x1A, 0, 1);
            }
            pl->value74 = 1;
            break;
        case 2:
            pl->value46 += 0x30;
            if (pl->value_a7 == -1)
            {
                pl->value110 = 0;
                if (pl->value74 != 0)
                {
                    pl->routine_2++;
                    pl_anim_set(pl, 0x1C, 0, 1);
                }
                else
                {
                    pl->value46 = 0;
                    pl->routine_2 += 2;
                    pl_anim_set(pl, 0x1B, 0, 1);
                }
            }
            else
            {
                pl->value74 = 1;
            }
            break;
        case 3:
            pl->value46 += 0x30;
            if (pl->value74 == 0)
            {
                pl->value46 = 0;
                pl->routine_2++;
                pl_anim_set(pl, 0x1B, pl->value_a4, 1);
            }
            else if (pl->value_a7 == -1)
            {
                pl_anim_set(pl, 0x12, 0, 1);
                pl->routine_1 = 7;
                pl->routine_2 = 5;
            }
            break;
        case 4:
            if (pl->value_a6 == 1)
            {
                if (pl->routine_3 != 0)
                {
                    pl->value48 = 0xC0;
                }
                else
                {
                    pl->value48 = -0xC0;
                }
                Sound_call(0x93, 0, 0);
                pl->routine_1 = 8;
                pl->routine_2 = 6;
                return;
            }
            break;
    }
    Pl_move(pl, pl->value44, pl->value46, pl->value48);
}

void func_8003E4A8(PL_WORK *pl)
{
    PL_EFFECT *fx;

    switch (pl->routine_2)
    {
        case 0:
            switch (pl->routine_3)
            {
                case 1:
                    pl->position_fixed.z += -0x180000;
                    pl->position_fixed.y += 0x200000;
                    break;
                case 2:
                    pl->position_fixed.z += 0x180000;
                    pl->position_fixed.y += 0x200000;
                    break;
                case 3:
                    pl->position_fixed.x += 0x180000;
                    pl->position_fixed.y += 0x200000;
                    break;
                case 4:
                    pl->position_fixed.x += -0x180000;
                    pl->position_fixed.y += 0x200000;
                    break;
                case 5:
                    pl->position_fixed.x += 0xC0000;
                    pl->position_fixed.z -= 0xC0000;
                    pl->position_fixed.y += 0x200000;
                    break;
                case 6:
                    pl->position_fixed.x += -0xC0000;
                    pl->position_fixed.z += 0xC0000;
                    pl->position_fixed.y += 0x200000;
                    break;
                case 7:
                    pl->position_fixed.x += -0xC0000;
                    pl->position_fixed.z += -0xC0000;
                    pl->position_fixed.y += 0x200000;
                    break;
                case 8:
                    pl->position_fixed.x += 0xC0000;
                    pl->position_fixed.z += 0xC0000;
                    pl->position_fixed.y += 0x200000;
                    break;
            }
            fx = (PL_EFFECT *)Open_EFFECT_WORK();
            if (fx != NULL)
            {
                pl_effect_pos_set(pl, fx, 0);
            }
            Obj_pos_adj_round(&pl->position_fixed, &pl->position);
            pl_anim_set(pl, 0x12, 0, 0);
            pl->value48 = 0;
            pl->value46 = 0;
            pl->value44 = 0;
            break;
        case 1:
            pl_anim_set(pl, 0x13, 0, 1);
            pl->routine_2++;
            break;
        case 2:
            if (pl->value_a7 == -1)
            {
                pl->routine_1 = 8;
                pl->routine_2 = 6;
            }
            break;
    }
}

void Pl_r1_damage(PL_WORK *pl)
{
    switch (pl->routine_2)
    {
        case 0:
            if (pl->routine_3 == 0)
            {
                pl_anim_set(pl, 0x20, 0, 1);
                pl->value44 = -0x200;
            }
            else
            {
                pl_anim_set(pl, 0x21, 0, 1);
                pl->value44 = 0x200;
            }
            pl->value46  = 0;
            pl->value48  = 0;
            pl->value168 = 1;
            pl->value169 = 1;
            pl->value44a = 0;
            Pl00_shot_enable_off(pl);
            Pl_move(pl, pl->value44, pl->value46, pl->value48);
            Sound_key_off_voice(0x16);
            Sound_call(0x97, 0, 0);
            func_800395E8();
            pl->value108 = 0;
            pl->routine_2++;
            return;
        case 1:
            if (pl->value_a6 == 1)
            {
                pl->routine_2++;
            }
            // fall through
        case 2:
            if (pl->routine_3 == 0)
            {
                pl->value44 += 0x40;
            }
            else
            {
                pl->value44 -= 0x40;
            }
            if (pl->value44 != 0)
            {
                Pl_move(pl, pl->value44, pl->value46, pl->value48);
                return;
            }
            pl->value44 = 0;
            pl->routine_2++;
            return;
        case 3:
            if (pl->value_a7 == -1)
            {
                pl->routine_1 = 8;
                pl->routine_2 = 6;
                pl->value71   = 0;
                pl->value168  = 0;
                pl->value169  = 0;
            }
            return;
    }
}

void Pl_r1_down(PL_WORK *pl)
{
    switch (pl->routine_2)
    {
        case 0:
        case 1:
            pl_anim_set(pl, 0x22, pl->routine_2 != 0 ? 6 : 0, 1);
            pl->value44 = -0x200;
            Pl00_shot_enable_off(pl);
            pl->value46  = -0x160;
            pl->value48  = 0;
            pl->value74  = 1;
            pl->value110 = -1;
            pl->value168 = 1;
            pl->value169 = 1;
            pl->value44a = 0;
            Pl_move(pl, pl->value44, pl->value46, pl->value48);
            Sound_key_off_voice(0x16);
            Sound_call(0x98, 0, 0);
            func_800395E8();
            pl->value108 = 0;
            pl->routine_2++;
            return;
        case 2:
            if (pl->value74 == 0)
            {
                pl_anim_set(pl, 0x23, 0, 1);
                Sound_call(0x99, 0, 0);
                pl->routine_2++;
                return;
            }
            if (pl->value_a6 == 1)
            {
                pl->routine_2 += 2;
            }
            pl->value46 += 0x30;
            Pl_move(pl, pl->value44, pl->value46, pl->value48);
            return;
        case 3:
            if (pl->value_a7 != -1)
            {
                return;
            }
            if (pl->life >= 0)
            {
                pl_anim_set(pl, 0x25, 0, 1);
                pl->rotation.y -= pl->value116;
                pl->value112 = 0;
                pl->value116 = 0;
                Camera_cmd_set(D_80097D9C);
                Gauge_hide();
                pl->routine_1 = 8;
                pl->routine_2 = 6;
                pl->value110  = 1;
                pl->value71   = 0;
                pl->value168  = 0;
                pl->value169  = 0;
                pl->value111  = 0x30;
            }
            else
            {
                pl->kind      = 2;
                pl->routine_1 = 0;
            }
            return;
        case 4:
            if (pl->value74 == 0)
            {
                pl_anim_set(pl, 0x24, 0, 1);
                if (func_8005D8C4(8) && (sint8)pl->reserved16a == 0)
                {
                    pl->life -= 8;
                }
                Swing_Camera2(0x1E, 0x400, 0x2800, &pl->position);
                Sound_call(0x99, 0, 0);
                pl->routine_2++;
            }
            else
            {
                pl->value46 += 0x30;
                Pl_move(pl, pl->value44, pl->value46, pl->value48);
            }
            return;
        case 5:
            if (pl->value_a7 != -1)
            {
                return;
            }
            if (pl->life >= 0)
            {
                pl_anim_set(pl, 0x29, 0, 1);
                pl->rotation.y -= pl->value116;
                pl->value112 = 0;
                pl->value116 = 0;
                Camera_cmd_set(D_80097D9C);
                Gauge_hide();
                pl->routine_1 = 8;
                pl->routine_2 = 6;
                pl->value110  = 1;
                pl->value71   = 0;
                pl->value168  = 0;
                pl->value169  = 0;
                pl->value111  = 0x30;
            }
            else
            {
                pl->kind      = 2;
                pl->routine_1 = 0;
            }
            return;
    }
}

void Pl_r1_hang(PL_WORK *pl)
{
    PL_SHORT_VECTOR pos;
    uint16 pad;
    sint32 dir;
    sint32 h;

    pad = pl->pad_cur;
    switch (pl->routine_2)
    {
        case 0:
            Pl00_shot_enable_off(pl);
            pl_anim_set(pl, 0x1D, 0, 0);
            Sound_call(0x9F, 0, 0);
            pl->value44a = 0;
            pl->value_b4 = 0;
            pl->value44d = 0;
            pl->routine_2++;
            break;
        case 1:
            if (pl->value_a7 == -1)
            {
                pl_anim_set(pl, 0x1E, 0, 0);
                pl->routine_2++;
            }
            break;
        case 2:
            if (pl->key_back & pad)
            {
                pl->routine_2 = 6;
                break;
            }
            pos = pl->position;
            dir = (pl->rotation.y - (sint16)pl->value116 + 0x200) >> 10;
            if (dir & 1)
            {
                if (dir & 2)
                {
                    pos.x += pl->info78->value2 + 1;
                }
                else
                {
                    pos.x += pl->info78->value0 - 1;
                }
            }
            else if (dir & 2)
            {
                pos.z += pl->info78->value6 + 1;
            }
            else
            {
                pos.z += pl->info78->value4 - 1;
            }
            pos.y -= 0x98;
            h = Scr_height_calc(&pos, 0, 0, pl->value01) + 0x98;
            if (PL_ABS(pl->position.y - h) < 0x20)
            {
                pl->position.y       = h;
                pl->position_fixed.y = h << 16;
            }
            else
            {
                pl_fall_start(pl);
                break;
            }
            if (pl->key_forward & pad)
            {
                pl->value75 = 2;
                pl->value74 = 2;
                pl->value76 = 0;
                pl->value44 = 0;
                pl->value48 = 0;
                pl->routine_2++;
                pl->value46 = ((pl->position.y - pl->position_prev.y) << 5) - 0x90;
                pl_anim_set(pl, 0x1F, 0, 0);
            }
            break;
        case 3:
            Pl_move(pl, 0, pl->value46, 0);
            if (pl->value_a6 == 1)
            {
                pl->routine_2++;
            }
            break;
        case 4:
            pl->value44 = 0x50;
            pl->value46 = -0x1B0;
            pl->value48 = 0;
            Pl_move(pl, pl->value44, pl->value46, 0);
            pl->routine_2++;
            break;
        case 5:
            pl->value46 += 0x30;
            Pl_move(pl, pl->value44, pl->value46, 0);
            if (pl->value74 == 0)
            {
                pl_anim_set(pl, 0x13, 0, 1);
                Sound_call(0x93, 0, 0);
                pl->routine_1 = 8;
                pl->routine_2 = 6;
            }
            if (pl->value_a7 == -1)
            {
                pl_fall_start(pl);
            }
            break;
        case 6:
            pl->value44 = -0x2A;
            pl->value46 = 0;
            pl->value48 = 0;
            Pl_move(pl, pl->value44, 0, 0);
            pl_fall_start(pl);
            break;
    }
}

void pl_fall_start(PL_WORK *pl)
{
    pl->routine_1             = 7;
    *(uint16 *)&pl->routine_2 = 4;
    pl->value74 = pl->value75 = 1;
    Pl00_shot_enable_on(pl);
    pl_anim_set(pl, 0x12, 0, 1);
}

void Pl_r1_aim(PL_WORK *pl)
{
    PL_WORK *target;
    sint32 dir;
    uint16 angle;

    if (pl->routine_2 == 0)
    {
        pl->routine_2 = 1;
        pl->value44   = 0;
        pl->value46   = 0;
        pl->value48   = 0;
        pl->value108  = 0;
        pl->value10a  = 0;
        pl->value10e  = 0;
    }
    target = (PL_WORK *)Lockon_target;
    if (target == NULL || !(D_80098B18 & 2) || pl_free_aim_check(pl) != 0)
    {
        pl_pitch_pad(pl, 2, 0x40);
#ifdef __wasm32__
        Camera_height_set(pl->value108);
#else
        Camera_height_set(pl->value108, 0x15E);
#endif
        pl->value10e = 0;
        if (!(pl->shot_status_flag & 0xC0))
        {
            pl_turn_pad(pl, 4, 0x48, 1);
        }
        else if (func_80041158(pl) == 0)
        {
            pl_turn_pad(pl, 4, 0x48, 0);
        }
        else
        {
            pl->value_b4 = 0;
        }
        return;
    }
    angle        = func_80031268(&pl->position.x, &target->position.x) & 0xFFF;
    pl->value10a = angle;
    dir          = pl->rotation.y - angle;
    if (dir > 0x800)
    {
        dir -= 0x1000;
    }
    if (dir < -0x800)
    {
        dir += 0x1000;
    }
    if (!(pl->shot_status_flag & 0xC0))
    {
        pl_turn_to(pl, dir, 1);
    }
    else if (pl_shot_button_held(pl) == 0)
    {
        pl_turn_to(pl, dir, 0);
    }
    dir = Calc_elevation(&pl->position, &target->position) - 0xC00;
    if (dir == pl->value108)
    {
        pl->value10e = 0;
    }
    else if (dir - pl->value108 > 0)
    {
        if (pl->value10e < 0)
        {
            pl->value10e = 0;
        }
        else
        {
            pl->value10e += 4;
            if (pl->value10e > 0x40)
            {
                pl->value10e = 0x40;
            }
        }
        if (dir - pl->value108 > pl->value10e)
        {
            pl->value108 += pl->value10e;
        }
        else
        {
            pl->value108 = dir;
        }
        if (pl->value108 > 0x1C0)
        {
            pl->value108 = 0x1C0;
        }
    }
    else
    {
        if (pl->value10e > 0)
        {
            pl->value10e = 0;
        }
        else
        {
            pl->value10e -= 4;
            if (pl->value10e < -0x40)
            {
                pl->value10e = -0x40;
            }
        }
        if (dir - pl->value108 < pl->value10e)
        {
            pl->value108 += pl->value10e;
        }
        else
        {
            pl->value108 = dir;
        }
        if (pl->value108 < -0x1C0)
        {
            pl->value108 = -0x1C0;
        }
    }
#ifdef __wasm32__
    Camera_height_set(pl->value108);
#else
    Camera_height_set(pl->value108, 0x15E);
#endif
}

void pl_turn_to(PL_WORK *pl, sint32 dir, sint32 arg)
{
    sint16 turn;

    if ((dir < 0 ? -dir : dir) < 0x80)
    {
        pl->value_b4   = 0;
        pl->rotation.y = pl->value10a;
        if (arg)
        {
            pl_anim_set(pl, 0, 0, 0);
        }
    }
    else
    {
        turn = -0x80;
        if (dir <= 0)
        {
            turn = 0x80;
        }
        pl->value_b4 = turn;
        if (arg)
        {
            pl_anim_set(pl, 0xD, 0, 0);
        }
        pl->rotation.y += pl->value_b4;
    }
}

sint32 pl_free_aim_check(PL_WORK *pl)
{
    uint16 pad;
    sint32 mode;

    pad = pl->pad_cur;
    if (PL_GAME_FLAG56 == 0)
    {
        mode = Game_work.flags_83;
        if (mode == 0 && (pl->value13e & pad))
        {
            return 1;
        }
        if (mode != 1)
        {
            return 0;
        }
        if (pl->key_aim & pad)
        {
            return 0;
        }
    }
    return 1;
}

void func_8003F288(PL_WORK *pl)
{
    switch (pl->routine_2)
    {
        case 0:
            pl_anim_set(pl, 0x80, 0, 1);
            pl->routine_2++;
            break;
        case 1:
            pl->position_prev       = pl->position;
            pl->position_fixed_prev = pl->position_fixed;
            pl->rotation_prev       = pl->rotation;
            pl->value_ad            = pl->value_ac;
            pl->position            = pl->position_saved;
            pl->position_fixed      = pl->position_saved_fixed;
            pl->rotation            = pl->rotation_saved;
            break;
        case 2:
            pl->routine_1 = 0xC;
            pl->routine_2 = 0;
            break;
    }
}

void Pl_r1_talk(PL_WORK *pl)
{
    switch (pl->routine_2)
    {
        case 0:
            pl_anim_set(pl, 0x30, 0, 1);
            pl->routine_2++;
            break;
        case 1:
            break;
        case 2:
            pl_anim_set(pl, 0x31, 0, 1);
            pl->routine_2++;
            break;
        case 3:
            if (pl->value_a7 == -1)
            {
                pl->routine_1 = 0;
                pl->routine_2 = 0;
            }
            break;
    }
}

void func_8003F498(PL_WORK *pl)
{
    if (pl->routine_2 == 0)
    {
        pl_anim_set(pl, 0x33, 0, 1);
        pl->routine_2++;
    }
}

void func_8003F4E8(PL_WORK *pl)
{
    if (pl->routine_2 == 0)
    {
        pl_anim_set(pl, 0x34, 0, 1);
        pl->routine_2++;
    }
}

void func_8003F538(PL_WORK *pl)
{
    if (pl->routine_2 == 0)
    {
        pl_anim_set(pl, 0x35, 0, 1);
        pl->routine_2++;
    }
}

void func_8003F588(PL_WORK *pl)
{
    if (pl->routine_2 == 0)
    {
        pl_anim_set(pl, 0x36, 0, 1);
        pl->routine_2++;
    }
}

void Pl_r1_stand_check(PL_WORK *pl)
{
    uint32 pad;
    uint32 hold;

    pad  = pl->pad_cur;
    hold = pl->pad_trg;
    if (pl_aim_start(pl) != 0 || pl_investigate_check(pl) != 0)
    {
        pl->value44a = 0;
        return;
    }
    if (hold & (pl->key_forward | pl->key_back | (pl->key_run_l | pl->key_run_r)))
    {
        pl->routine_3 = 0;
    }
    if ((pad & (pl->key_jump | (pl->key_forward | pl->key_back | (pl->key_run_l | pl->key_run_r)) |
                (pl->key_shot_l | pl->key_shot_r) | (pl->key_turn_l | pl->key_turn_r))) ||
        (pl->value176 & 3))
    {
        pl->value44a = 0;
        if (func_800410D4(pl) != 0 ||
            ((pl_shot_start(pl, 0), pl_jump_start(pl) == 0) &&
             (pl_shot_button_held(pl) != 0 || pl_shot_anim_done(pl) == 0 || pl->routine_3 != 0 ||
              (pl_run_start(pl) == 0 && pl_run_left_start(pl, pl->routine_3) == 0 &&
               pl_run_right_start(pl, pl->routine_3) == 0 &&
               pl_run_back_start(pl, pl->routine_3) == 0)) &&
             ((pl->shot_status_flag & 0xC0) || pl->value177 != 0 ||
              (pl_walk_start(pl) == 0 && pl_side_step_start(pl, pl->routine_3) == 0 &&
               pl_walk_back_start(pl, pl->routine_3) == 0))))
        {
            if (func_80041158(pl) == 0)
            {
                pl_turn_start(pl);
            }
            goto reset;
        }
    }
    else
    {
    reset:
        pl_super_dash_start(pl);
    }
}

void Pl_r1_walk_check(PL_WORK *pl)
{
    uint32 pad;

    pad = pl->pad_cur;
    if (pl_investigate_check(pl) == 0)
    {
        if (!(pad & (pl->key_jump |
                     (pl->key_forward | pl->key_back | (pl->key_run_l | pl->key_run_r)) |
                     (pl->key_shot_l | pl->key_shot_r))) ||
            pl_shot_start(pl, 0) != 0)
        {
            goto reset;
        }
        if (pl_jump_start(pl) == 0 && pl_run_start(pl) == 0 && pl_run_back_start(pl, 0) == 0)
        {
            if ((pl->shot_status_flag & 0xC0) || pl->value177 != 0)
            {
                goto reset;
            }
            if (pl_walk_start(pl) == 0 && pl_walk_back_start(pl, 0) == 0)
            {
                if (pl->routine_3 != 0)
                {
                    if (pl->key_forward & pad)
                    {
                        if (!(pl->key_dash & pad))
                        {
                            pl->routine_1             = 2;
                            *(uint16 *)&pl->routine_2 = 0;
                            pl->value112              = 0;
                        }
                    }
                    else
                    {
                        goto reset;
                    }
                }
                else if (pl->key_back & pad)
                {
                    if (!(pl->key_dash & pad))
                    {
                        pl->routine_1             = 2;
                        *(uint16 *)&pl->routine_2 = 0;
                        pl->value112              = 2;
                    }
                }
                else
                {
                    goto reset;
                }
            }
        }
        return;
    reset:
        pl->routine_1             = 0;
        *(uint16 *)&pl->routine_2 = 0;
    }
}

void Pl_r1_run_check(PL_WORK *pl)
{
    uint16 pad;
    uint16 trig;

    pad  = pl->pad_cur;
    trig = pl->pad_trg;
    switch ((sint8)pl->value112)
    {
        case 0:
            if (pad &
                (pl->key_jump | (pl->key_forward | pl->key_back | (pl->key_run_l | pl->key_run_r)) |
                 (pl->key_shot_l | pl->key_shot_r)))
            {
                pl_shot_start(pl, 1);
                if (pl_run_jump_start(pl) || pl_walk_back_start(pl, 0) || pl_run_back_start(pl, 0))
                {
                    return;
                }
                if (pad & pl->key_forward)
                {
                    if (pad & pl->key_dash)
                    {
                        pl->routine_1             = 1;
                        *(uint16 *)&pl->routine_2 = 0;
                    }
                    return;
                }
            }
            pl->routine_1             = 0;
            *(uint16 *)&pl->routine_2 = 0;
            Pl00_shot_enable_off(pl);
            return;
        case 1:
            if (!(pad & (pl->key_jump |
                         (pl->key_forward | pl->key_back | (pl->key_run_l | pl->key_run_r)) |
                         (pl->key_shot_l | pl->key_shot_r))) ||
                (pad & (pl->key_run_l | pl->key_run_r)) == (pl->key_run_l | pl->key_run_r) ||
                pl_shot_button_held(pl))
            {
                break;
            }
            pl_shot_start(pl, 1);
            if (pl_run_jump_start(pl))
            {
                return;
            }
            if (!(pad & pl->key_dash))
            {
                if (pl_run_start(pl) || pl_run_back_start(pl, 0) || pl_run_right_start(pl, 0) ||
                    (pad & pl->key_run_l))
                {
                    return;
                }
                break;
            }
            if (Game_work.flags_83 != 0)
            {
                break;
            }
            if (pad & pl->key_run_l)
            {
                if (pl->routine_2 < 5)
                {
                    pl->routine_1             = 5;
                    *(uint16 *)&pl->routine_2 = 0;
                    Pl00_shot_enable_off(pl);
                }
                return;
            }
            break;
        case 2:
            if (!(pad & (pl->key_jump |
                         (pl->key_forward | pl->key_back | (pl->key_run_l | pl->key_run_r)) |
                         (pl->key_shot_l | pl->key_shot_r))))
            {
                break;
            }
            if (trig & (pl->key_shot_l | pl->key_shot_r))
            {
                if (trig & pl->key_shot_l)
                {
                    pl->value176 = 2;
                }
                else
                {
                    pl->value176 = 1;
                }
                break;
            }
            if (pl_run_jump_start(pl))
            {
                return;
            }
            if (!(pad & pl->key_dash))
            {
                if ((pad & pl->key_back) || pl_run_start(pl) || pl_run_left_start(pl, 0) ||
                    pl_run_right_start(pl, 0))
                {
                    return;
                }
                break;
            }
            if (Game_work.flags_83 != 0)
            {
                break;
            }
            if (pad & pl->key_back)
            {
                if (pl->routine_2 < 5)
                {
                    pl->routine_1             = 5;
                    *(uint16 *)&pl->routine_2 = 0;
                    Pl00_shot_enable_off(pl);
                }
                return;
            }
            break;
        case 3:
            if (!(pad & (pl->key_jump |
                         (pl->key_forward | pl->key_back | (pl->key_run_l | pl->key_run_r)) |
                         (pl->key_shot_l | pl->key_shot_r))) ||
                (pad & (pl->key_run_l | pl->key_run_r)) == (pl->key_run_l | pl->key_run_r) ||
                pl_shot_button_held(pl))
            {
                break;
            }
            pl_shot_start(pl, 1);
            if (pl_run_jump_start(pl))
            {
                return;
            }
            if (pad & pl->key_dash)
            {
                if (Game_work.flags_83 != 0)
                {
                    break;
                }
                if (pad & pl->key_run_r)
                {
                    if (pl->routine_2 < 5)
                    {
                        pl->routine_1             = 5;
                        *(uint16 *)&pl->routine_2 = 0;
                        Pl00_shot_enable_off(pl);
                    }
                    return;
                }
                break;
            }
            if (pl_run_start(pl) || pl_run_left_start(pl, 0) || pl_run_back_start(pl, 0) ||
                (pad & pl->key_run_r))
            {
                return;
            }
            break;
    }
    if (pl->routine_2 < 5)
    {
        pl->routine_2 = 5;
    }
}

void Pl_r1_super_dash_check(PL_WORK *pl)
{
    uint16 pad;

    pad = pl->pad_cur;
    if (pl->routine_2 < 3 && !(pad & pl->key_dash))
    {
        pl->value44a = 0;
        Sound_call(0x8F, 0, 0);
        if (!pl_jump_start(pl))
        {
            pl->routine_1             = 0;
            *(uint16 *)&pl->routine_2 = 0;
        }
    }
}

void Pl_r1_turn_check(PL_WORK *pl)
{
    uint32 pad;

    pad = pl->pad_cur;
    if (func_80041158(pl) != 0 ||
        !(pad & (pl->key_jump | (pl->key_forward | pl->key_back | (pl->key_run_l | pl->key_run_r)) |
                 (pl->key_shot_l | pl->key_shot_r) | (pl->key_turn_l | pl->key_turn_r))))
    {
        goto reset;
    }
    if (pl_shot_start(pl, 0) != 0 || pl_jump_start(pl) != 0)
    {
        return;
    }
    if (pl_shot_button_held(pl) == 0)
    {
        if (pl_run_left_start(pl, 0) != 0 || pl_run_right_start(pl, 0) != 0 ||
            pl_run_start(pl) != 0)
        {
            return;
        }
        if (pl->shot_status_flag & 0xC0)
        {
            goto check;
        }
        if (pl_run_back_start(pl, 0) != 0)
        {
            return;
        }
    }
    if (!(pl->shot_status_flag & 0xC0) && pl->value177 == 0)
    {
        if (pl_walk_start(pl) != 0 || pl_side_step_start(pl, 0) != 0 ||
            pl_walk_back_start(pl, 0) != 0)
        {
            return;
        }
    }
check:
    if (pad & (pl->key_turn_l | pl->key_turn_r))
    {
        return;
    }
reset:
    pl->routine_1             = 0;
    *(uint16 *)&pl->routine_2 = 0;
}

void func_8003FFA8(void)
{
}

void Pl_r1_side_step_check(PL_WORK *pl)
{
    uint32 pad;
    sint32 mask;

    pad = pl->pad_cur;
    if (!(pad & (pl->key_jump | (pl->key_forward | pl->key_back | (pl->key_run_l | pl->key_run_r)) |
                 (pl->key_shot_l | pl->key_shot_r) | (pl->key_step_l | pl->key_step_r))) ||
        pl_shot_start(pl, 0) != 0 ||
        (pl_jump_start(pl) == 0 && pl_walk_start(pl) == 0 && pl_walk_back_start(pl, 0) == 0 &&
         pl_run_back_start(pl, 0) == 0 &&
         ((mask = pl->key_step_l | pl->key_step_r, (pad & mask) == mask) || !(pl->key_dash & pad))))
    {
        pl->routine_1             = 0;
        *(uint16 *)&pl->routine_2 = 0;
    }
}

void Pl_r1_jump_check(PL_WORK *pl)
{
    if (pl_hang_start(pl))
    {
        return;
    }
    if (pl->value74 == 0 && pl->routine_2 == 1)
    {
        if (pl_side_jump_start(pl))
        {
            return;
        }
    }
    if ((sint8)pl->value112 != 2)
    {
        pl_shot_start(pl, 2);
    }
}

void Pl_r1_side_jump_check(void)
{
}

void func_80040138(void)
{
}

void Pl_r1_damage_check(PL_WORK *pl)
{
    if (pl->routine_2 == 2)
    {
        pl_recover_jump_start(pl);
    }
}

void Pl_r1_down_check(void)
{
}

void Pl_r1_hang_check(PL_WORK *pl)
{
    sint32 y;
    sint32 floor;

    if (pl->routine_2 == 1)
    {
        y     = pl->position.y;
        floor = Scr_height_check(pl) - 0x20;
        if (floor - pl->info78->value0a < y)
        {
            pl->routine_1             = 7;
            *(uint16 *)&pl->routine_2 = 4;
            pl->value74 = pl->value75 = 1;
            pl->value44               = -0x2A;
            pl->value46               = 0;
            pl->value48               = 0;
            Pl00_shot_enable_on(pl);
            pl_anim_set(pl, 0x12, 0, 1);
        }
    }
}

void Pl_r1_aim_check(PL_WORK *pl)
{
    uint16 pad;

    pad = pl->pad_cur;
    pl_shot_start(pl, 0);
    if (Game_work.flags_83 != 1 && !(pl->key_aim & pad))
    {
        pl->routine_1             = 0;
        *(uint16 *)&pl->routine_2 = 0;
        func_800394AC();
        pl->value108 = 0;
    }
}

void func_80040294(void)
{
}

void Pl_r1_talk_check(void)
{
}

void func_800402A4(void)
{
}

void func_800402AC(void)
{
}

void func_800402B4(void)
{
}

void func_800402BC(void)
{
}

sint32 pl_walk_start(PL_WORK *pl)
{
    if (pl->pad_cur & pl->key_forward)
    {
        if (pl->pad_cur & pl->key_dash)
        {
            pl->routine_1             = 1;
            *(uint16 *)&pl->routine_2 = 0;
            return 1;
        }
    }
    return 0;
}

sint32 pl_walk_back_start(PL_WORK *pl, sint32 arg)
{
    uint16 pad;

    if (Scene_work != 0 || arg == 0)
    {
        pad = pl->pad_cur;
    }
    else
    {
        pad = pl->pad_trg;
    }
    if (pad & pl->key_back)
    {
        if (pad & pl->key_dash)
        {
            pl->routine_1             = 1;
            *(uint16 *)&pl->routine_2 = 0x100;
            return 1;
        }
    }
    return 0;
}

sint32 pl_run_start(PL_WORK *pl)
{
    uint8 tmp;

    if (pl->pad_cur & pl->key_forward)
    {
        if (!(pl->pad_cur & pl->key_dash))
        {
            tmp                       = pl->value112;
            pl->routine_1             = 2;
            *(uint16 *)&pl->routine_2 = 0;
            pl->value112              = 0;
            pl->value113              = tmp;
            return 1;
        }
    }
    return 0;
}

sint32 pl_run_left_start(PL_WORK *pl, sint32 arg)
{
    uint16 pad;
    uint16 first;
    uint16 second;
    uint16 mask;
    uint8 tmp;

    if (Scene_work != 0 || arg == 0)
    {
        pad = pl->pad_cur;
    }
    else
    {
        pad = pl->pad_trg;
    }
    first  = pl->key_run_l;
    second = pl->key_run_r;
    mask   = first | second;
    if ((pad & mask) == mask)
    {
        return 0;
    }
    if (pad & first)
    {
        if (!(pad & pl->key_dash))
        {
            tmp                       = pl->value112;
            pl->routine_1             = 2;
            *(uint16 *)&pl->routine_2 = 0;
            pl->value112              = 1;
            pl->value113              = tmp;
            return 1;
        }
    }
    return 0;
}

sint32 pl_run_right_start(PL_WORK *pl, sint32 arg)
{
    uint16 pad;
    uint16 first;
    uint16 second;
    uint16 mask;
    uint8 tmp;

    if (Scene_work != 0 || arg == 0)
    {
        pad = pl->pad_cur;
    }
    else
    {
        pad = pl->pad_trg;
    }
    first  = pl->key_run_l;
    second = pl->key_run_r;
    mask   = first | second;
    if ((pad & mask) == mask)
    {
        return 0;
    }
    if (pad & second)
    {
        if (!(pad & pl->key_dash))
        {
            tmp                       = pl->value112;
            pl->routine_1             = 2;
            *(uint16 *)&pl->routine_2 = 0;
            pl->value112              = 3;
            pl->value113              = tmp;
            return 1;
        }
    }
    return 0;
}

sint32 pl_run_back_start(PL_WORK *pl, sint32 arg)
{
    uint16 pad;
    uint8 tmp;

    if (Scene_work != 0 || arg == 0)
    {
        pad = pl->pad_cur;
    }
    else
    {
        pad = pl->pad_trg;
    }
    if (pad & pl->key_back)
    {
        if (!(pad & pl->key_dash))
        {
            tmp                       = pl->value112;
            pl->routine_1             = 2;
            *(uint16 *)&pl->routine_2 = 0;
            pl->value112              = 2;
            pl->value113              = tmp;
            return 1;
        }
    }
    return 0;
}

sint32 pl_super_dash_start(PL_WORK *pl)
{
    uint16 pad;

    pad = pl->pad_cur;
    if ((pad & pl->key_dash) && (pl->flags16b & 2))
    {
        pl->value44a++;
        if (pl->value44a >= 9 && pl->value44b == 0)
        {
            pl->routine_1             = 3;
            *(uint16 *)&pl->routine_2 = 0;
            return 1;
        }
        return 0;
    }
    pl->value44a = 0;
    return 0;
}

sint32 pl_turn_start(PL_WORK *pl)
{
    if (pl->pad_cur & (pl->key_turn_l | pl->key_turn_r))
    {
        pl->routine_1             = 4;
        *(uint16 *)&pl->routine_2 = 0;
        return 1;
    }
    return 0;
}

sint32 pl_side_step_start(PL_WORK *pl, sint32 arg)
{
    uint16 pad;

    if (Scene_work != 0 || arg == 0)
    {
        pad = pl->pad_cur;
    }
    else
    {
        pad = pl->pad_trg;
    }
    if (pad & pl->key_dash)
    {
        if (pad & (pl->key_step_l | pl->key_step_r))
        {
            pl->routine_1             = 6;
            *(uint16 *)&pl->routine_2 = 0;
            return 1;
        }
    }
    return 0;
}

sint32 pl_jump_start(PL_WORK *pl)
{
    if (pl->pad_trg & pl->key_jump)
    {
        pl->routine_1             = 7;
        *(uint16 *)&pl->routine_2 = 0;
        return 1;
    }
    return 0;
}

sint32 pl_run_jump_start(PL_WORK *pl)
{
    if (pl->pad_trg & pl->key_jump)
    {
        pl->routine_1             = 8;
        *(uint16 *)&pl->routine_2 = 0;
        return 1;
    }
    return 0;
}

sint32 pl_side_jump_start(PL_WORK *pl)
{
    uint16 hit;

    hit = pl->pad_cur & (pl->key_turn_l | pl->key_turn_r);
    if (hit == pl->key_turn_l)
    {
        pl->routine_1             = 9;
        *(uint16 *)&pl->routine_2 = 0;
        return 1;
    }
    if (hit == pl->key_turn_r)
    {
        pl->routine_1             = 9;
        *(uint16 *)&pl->routine_2 = 0x100;
        return 1;
    }
    return 0;
}

sint32 pl_recover_jump_start(PL_WORK *pl)
{
    uint16 pad;
    uint16 hit;
    sint32 right_none;
    sint32 hit_none;

    pad = pl->pad_cur;
    if (pl->pad_trg & pl->key_jump)
    {
        right_none = (pad & (pl->key_turn_r | pl->key_run_r)) == 0;
        hit        = pad & (pl->key_turn_l | pl->key_run_l);
        hit_none   = hit == 0;
        if (right_none != hit_none)
        {
            if (hit != 0)
            {
                pl->routine_1             = 9;
                *(uint16 *)&pl->routine_2 = 0;
            }
            else
            {
                pl->routine_1             = 9;
                *(uint16 *)&pl->routine_2 = 0x100;
            }
        }
        else if (rand() & 1)
        {
            pl->routine_1             = 9;
            *(uint16 *)&pl->routine_2 = 0;
        }
        else
        {
            pl->routine_1             = 9;
            *(uint16 *)&pl->routine_2 = 0x100;
        }
        pl->value168 = 0;
        pl->value169 = 0;
        return 1;
    }
    return 0;
}

// The floor height is its own variable; the floor test is the success condition; the height
// test goes through its own variable h; off is reused for the edge distance. Each of the four
// arms computes and stores the stepped coordinate itself (GCC merges the identical tails, and
// the longer chain makes it load info78 before the coordinate).
sint32 pl_hang_start(PL_WORK *pl)
{
    PL_SHORT_VECTOR from;
    PL_SHORT_VECTOR to;
    PL_SHORT_VECTOR step;
    sint32 y;
    sint32 floor;
    sint32 dir;
    sint32 raw;
    sint32 off;
    uint16 x;
    sint32 h;
    uint16 edge;

    to   = pl->position;
    from = to;
    if (pl->value74 == 0 || pl->value46 <= 0)
    {
        return 0;
    }
    y = pl->position.y;
    h = Scr_height_check(pl) - 0x30;
    if (h - pl->info78->value0a < y)
    {
        return 0;
    }
    off = Scr_wall_dist_check(pl);
    raw = (pl->rotation.y - (sint16)pl->value116 + 0x200) >> 10;
    dir = raw & 3;
    if (raw & 1)
    {
        from.z += pl->info78->value4;
        off -= *(sint16 *)0x1F8000C0;
        to.z += pl->info78->value6;
        step.x = off;
        step.z = 0;
        if (raw & 2)
        {
            edge = pl->info78->value2;
            x    = to.x;
            x++;
            x += edge + off;
            from.x = to.x = x;
        }
        else
        {
            edge = pl->info78->value0;
            x    = to.x;
            x--;
            x += edge + off;
            from.x = to.x = x;
        }
    }
    else
    {
        from.x += pl->info78->value0;
        off -= *(sint16 *)0x1F8000C4;
        to.x += pl->info78->value2;
        step.x = 0;
        step.z = off;
        if (raw & 2)
        {
            edge = pl->info78->value6;
            x    = to.z;
            x++;
            x += edge + off;
            from.z = to.z = x;
        }
        else
        {
            edge = pl->info78->value4;
            x    = to.z;
            x--;
            x += edge + off;
            from.z = to.z = x;
        }
    }
    if (PL_ABS(off) > 0x40)
    {
        return 0;
    }
    from.y -= 0x98;
    to.y -= 0x98;
    floor = Scr_height_calc(&from, 0, 0, pl->value01);
    if (floor != Scr_height_calc(&to, 0, 0, pl->value01))
    {
        return 0;
    }
    if (floor < pl->position.y - 0x68 && pl->position.y - 0x98 < floor)
    {
        pl->position.x += step.x;
        pl->position.z += step.z;
        pl->position.y       = floor + 0x98;
        pl->position_fixed.y = (floor + 0x98) << 16;
        pl->rotation.y       = dir << 10;
        pl->value116         = 0;
        pl->value112         = 0;
        Camera_cmd_set(D_80097D9C);
        pl->routine_1             = 0xD;
        *(uint16 *)&pl->routine_2 = dir << 8;
        return 1;
    }
    return 0;
}

sint32 pl_aim_start(PL_WORK *pl)
{
    uint16 pad;

    pad = pl->pad_cur;
    if (Game_work.flags_83 == 1 || (pad & pl->key_aim))
    {
        pl->routine_1             = 0xE;
        *(uint16 *)&pl->routine_2 = 0;
        return 1;
    }
    return 0;
}

sint32 func_80040B34(PL_WORK *pl)
{
    if (pl->pad_cur & pl->value13e)
    {
        pl->routine_1             = 5;
        *(uint16 *)&pl->routine_2 = 0;
        return 1;
    }
    return 0;
}

sint32 pl_shot_start(PL_WORK *pl, sint32 arg)
{
    sint16 dist;
    uint32 pad;
    uint8 reload2;
    uint8 reload1;

    dist    = arg;
    pad     = pl->pad_cur;
    reload2 = D_80089A88[pl->weapon_right_no].reload;
    reload1 = D_80089A88[pl->weapon_left_no].reload;
    if (!(pl->shot_status_flag & 0xC0))
    {
        if ((pl->value177 == 1 && ((pl->key_shot_l & pad) || (pl->value176 & 3) == 2) &&
             (pl->value175 & 5) == 5 && pl_shot_mode_set(pl, 0, dist, reload1) != 0) ||
            (pl->value177 == 2 && ((pl->key_shot_r & pad) || (pl->value176 & 3) == 1) &&
             (pl->value175 & 3) == 3 && Pl_special_shot_check() != 0 &&
             pl_shot_mode_set(pl, 2, dist, reload2) != 0))
        {
            pl->value178 = 1;
            pl->value177 = 0;
            pl->value176 = 0;
            return 1;
        }
        if (((pl->key_shot_l & pad) || (pl->value176 & 3) == 2) &&
            pl->weapon_data[pl->weapon_left_no].bullet_left > 0 && (pl->value175 & 5) == 5)
        {
            pl->value176 = 0;
            return pl_shot_mode_set(pl, 4, dist, reload1);
        }
        if (((pl->key_shot_r & pad) || (pl->value176 & 3) == 1) &&
            pl->weapon_data[pl->weapon_right_no].bullet_left > 0 && (pl->value175 & 3) == 3 &&
            Pl_special_shot_check() != 0)
        {
            pl->value176 = 0;
            return pl_shot_mode_set(pl, 6, dist, reload2);
        }
    }
    pl->value176 = 0;
    return 0;
}

sint32 pl_shot_mode_set(PL_WORK *pl, sint32 a1, sint16 a2, sint16 a3)
{
    switch (a2)
    {
        case 0:
            a1++;
            break;
        case 1:
            if (a3 == 0)
            {
                pl->routine_1             = 0;
                *(uint16 *)&pl->routine_2 = 0;
                return 0;
            }
            break;
        case 2:
            if (a3 == 0)
            {
                return 0;
            }
            break;
        default:
            return 1;
    }
    pl_shot_status_set(pl, a1);
    return 1;
}

void pl_shot_status_set(PL_WORK *pl, sint16 arg)
{
    switch (arg)
    {
        case 0:
            pl->shot_status_flag = -0x7A;
            return;
        case 1:
            pl->shot_status_flag = -0x7E;
            return;
        case 2:
            pl->shot_status_flag = 0x45;
            return;
        case 3:
            pl->shot_status_flag = 0x41;
            return;
        case 4:
            pl->shot_status_flag = -0x7A;
            pl->value104         = Pl_anim_buster(pl);
            break;
        case 5:
            pl->shot_status_flag = -0x7E;
            pl->value104         = Pl_anim_buster(pl);
            break;
        case 6:
            pl->shot_status_flag = 0x45;
            pl->value104         = Pl_anim_special(pl);
            break;
        case 7:
            pl->shot_status_flag = 0x41;
            pl->value104         = Pl_anim_special(pl);
            break;
    }
    pl->value178 = 0;
    pl->value105 = 0;
    pl->value100 = 0;
    if (!(pl->shot_status_flag & 4) && pl->value_ac == pl->value_ad)
    {
        pl_anim_set(pl, pl->value104, 0, 1);
    }
}

sint32 pl_shot_anim_done(PL_WORK *pl)
{
    sint32 type;

    type = pl->shot_status_flag & 0xC0;
    if (type == 0 || (type == 0x40 && pl->value100 > D_80089A84[pl->weapon_right_no * 12]) ||
        (type == 0x80 && pl->value100 > D_80089A84[pl->weapon_left_no * 12]))
    {
        return 1;
    }
    return 0;
}

sint32 pl_shot_button_held(PL_WORK *pl)
{
    uint16 pad;

    pad = pl->pad_cur;
    if (((pad & pl->key_shot_r) && D_80089A88[pl->weapon_right_no].reload == 0) ||
        ((pad & pl->key_shot_l) && D_80089A88[pl->weapon_left_no].reload == 0))
    {
        return 1;
    }
    return 0;
}

sint32 func_800410D4(PL_WORK *pl)
{
    sint32 type;

    type = pl->shot_status_flag & 0xC0;
    if ((type == 0x40 && D_80089A89[pl->weapon_right_no * 12] == 0) ||
        (type == 0x80 && D_80089A89[pl->weapon_left_no * 12] == 0))
    {
        return 1;
    }
    return 0;
}

sint32 func_80041158(PL_WORK *pl)
{
    sint32 type;

    type = pl->shot_status_flag & 0xC0;
    if ((type == 0x40 && D_80089A8A[pl->weapon_right_no * 12] == 0) ||
        (type == 0x80 && D_80089A8A[pl->weapon_left_no * 12] == 0))
    {
        return 1;
    }
    return 0;
}

sint32 pl_investigate_check(PL_WORK *pl)
{
    PL_SHORT_VECTOR pos;
    PL_SHORT_VECTOR rot;
    GAME_WORK_3C0 *p3c0;
    GAME_WORK_5A4 *p5a4;
    GAME_WORK_10C *p10c;
    GAME_WORK_50 *p;
    PL_TARGET50 *p50;
    void *best_wp;
    uint32 best;
    uint32 dist;
    sint32 i;
    uint16 pad;

    best_wp = NULL;
    if (Camera_moving() != 0 || (pl->shot_status_flag & 0xC0) || pl->value177 != 0)
    {
        return 0;
    }
    pad                   = pl->pad_cur;
    rot.z                 = 0;
    rot.y                 = 0;
    rot.x                 = 0;
    best                  = 0x7FFF;
    *(sint16 *)0x1F800100 = rsin(pl->rotation.y);
    *(sint16 *)0x1F800102 = rcos(pl->rotation.y);
    if ((pad & pl->key_check) && pl->value74 == 0)
    {
        for (p3c0 = Em_work_tbl, i = 0; i < 16; i++, p3c0++)
        {
            if ((p3c0->flags & 3) == 3 && ((PL_TARGET *)p3c0)->bounds != NULL)
            {
                dist = pl_front_box_check(pl, &((PL_TARGET *)p3c0)->position,
                                          &((PL_TARGET *)p3c0)->rotation,
                                          ((PL_TARGET *)p3c0)->bounds);
                if (dist != 0 && dist < best)
                {
                    best    = dist;
                    best_wp = p3c0;
                }
            }
        }
        for (p5a4 = Work_5A4_tbl, i = 0; i < 4; i++, p5a4++)
        {
            if ((p5a4->flags & 3) == 3 && ((PL_TARGET *)p5a4)->bounds != NULL)
            {
                dist = pl_front_box_check(pl, &((PL_TARGET *)p5a4)->position,
                                          &((PL_TARGET *)p5a4)->rotation,
                                          ((PL_TARGET *)p5a4)->bounds);
                if (dist != 0 && dist < best)
                {
                    best    = dist;
                    best_wp = p5a4;
                }
            }
        }
        for (p10c = Shl_work_tbl, i = 0; i < 32; i++, p10c++)
        {
            if ((p10c->flags & 3) == 3 && p10c->kind < 0x80 && ((PL_TARGET *)p10c)->bounds != NULL)
            {
                dist = pl_front_box_check(pl, &((PL_TARGET *)p10c)->position,
                                          &((PL_TARGET *)p10c)->rotation,
                                          ((PL_TARGET *)p10c)->bounds);
                if (dist != 0 && dist < best)
                {
                    best    = dist;
                    best_wp = p10c;
                }
            }
        }
        for (p = Work_50_tbl, i = 0, p50 = (PL_TARGET50 *)Work_50_tbl; i < 32; i++, p50++, p++)
        {
            if ((p->flags & 3) == 3 && p50->bounds != NULL)
            {
                if (p->flags & 4)
                {
                    pos.x = p50->x38;
                    pos.y = p50->y3a;
                    pos.z = p50->z3c;
                }
                else
                {
                    pos.x = p50->x1c;
                    pos.y = p50->y1e;
                    pos.z = p50->z20;
                }
                dist = pl_front_box_check(pl, &pos, &rot, p50->bounds);
                if (dist != 0 && dist < best)
                {
                    best    = dist;
                    best_wp = p50;
                }
            }
        }
        if (best < 0xD0)
        {
            pl->table180[0].y            = best;
            *(void **)&pl->table180[0].z = best_wp;
            return 1;
        }
    }
    *(void **)&pl->table180[0].z = NULL;
    return 0;
}

sint32 pl_front_box_check(PL_WORK *pl, PL_SHORT_VECTOR *pos, PL_SHORT_VECTOR *rot, PL_BOUNDS *box)
{
    sint16 probe[3];
    sint16 hit[3];
    sint32 center[3];
    sint32 origin[3];
    PL_MATRIX mtx;
    sint32 i;
    sint32 margin;
    sint32 dist;
    sint32 sx;
    sint32 ex;
    sint32 sz;
    sint32 ez;

    origin[0] = pos->x << 16;
    center[0] = origin[0] + ((box->x0 + box->x1) << 15);
    origin[2] = pos->z << 16;
    center[2] = origin[2] + ((box->z0 + box->z1) << 15);
    center[1] = origin[1] = pos->y << 16;
    sx                    = box->x0 - (box->x0 + box->x1) / 2;
    ex                    = box->x1 - (box->x0 + box->x1) / 2;
    sz                    = box->z0 - (box->z0 + box->z1) / 2;
    ez                    = box->z1 - (box->z0 + box->z1) / 2;
    RotMatrixYXZ(rot, &mtx);
    Calc_rotate_about(&mtx, center, origin);
    for (i = 0, margin = 0xC, dist = 0x50; i < 3; margin += 0xC, i++, dist += 0x30)
    {
        probe[0] = pl->position.x - (*(sint16 *)0x1F800100 * dist) / 0x1000;
        probe[2] = pl->position.z - (*(sint16 *)0x1F800102 * dist) / 0x1000;
        probe[1] = pl->position.y;
        hit[0]   = center[0] >> 16;
        hit[1]   = center[1] >> 16;
        hit[2]   = center[2] >> 16;
        if (hit[0] >= probe[0] - margin - ex && hit[0] <= probe[0] + margin - sx &&
            hit[2] >= probe[2] - margin - ez && hit[2] <= probe[2] + margin - sz &&
            hit[1] >= probe[1] - box->y1 && hit[1] <= probe[1] - box->y0)
        {
            return Calc_distance_xz(&pl->position.x, hit);
        }
    }
    return 0;
}

void pl_damage_check(PL_WORK *pl)
{
    uint32 flags;
    sint32 damage;
    uint32 dir;
    PL_SHORT_VECTOR *from;

    if (Scene_work != 0)
    {
        return;
    }
    flags  = pl->value188;
    damage = flags & 0xFFFFFF;
    from   = (PL_SHORT_VECTOR *)&pl->value190;
    if (damage != 0 || (flags & 0x1000000))
    {
        if (flags & 0xFFFFFF)
        {
            damage -= (damage * (sint8)pl->reserved16c[0]) >> 2;
            if (damage == 0)
            {
                damage = 1;
            }
        }
        if ((sint8)pl->value71 <= 0)
        {
            if ((sint8)pl->value71 == 0)
            {
                if (from->pad != 0)
                {
                    dir = Calc_degree2(from, &pl->position);
                }
                else
                {
                    dir = ratan2(from->x, from->z) + 0x800;
                }
                pl->value71 =
                    (((dir - pl->rotation.y + (sint16)pl->value116 + 0x200) & 0xFFF) >> 10) + 1;
            }
            else if ((pl->value71 & 0x7F) == 1)
            {
                pl->value116 += 0x800;
            }
            switch (pl->value71 & 0x7F)
            {
                case 2:
                    pl->value112 = (pl->value112 + 1) & 3;
                    pl->value116 += 0x400;
                    break;
                case 4:
                    pl->value112 = (pl->value112 - 1) & 3;
                    pl->value116 -= 0x400;
                    break;
            }
        }
        pl->shot_status_flag = 0;
        pl->value177         = 0;
        if (flags & 0x1000000)
        {
            func_8005D8C4(damage);
        }
        if (flags & 0x40000000)
        {
            Gauge_show_hold();
        }
        if (func_8005D8C4(0) != 0 || func_8005D938() != 0)
        {
            if (pl->life > 0)
            {
                pl->life -= damage;
                if (pl->life <= 0)
                {
                    pl->life = 0;
                    Gauge_show_hold();
                }
            }
            else
            {
                pl->life -= damage;
            }
            pl->value73 += damage;
            if ((sint8)pl->value73 > ((sint8)pl->reserved16a + 1) * 16)
            {
                pl->routine_1 = 0xC;
                pl->routine_2 = 1;
            }
            else
            {
                pl_damage_state_set(pl, flags);
            }
        }
        else
        {
            if (pl->life > 0)
            {
                func_8005D8C4(damage);
            }
            else
            {
                Gauge_show_hold();
            }
            pl_damage_state_set(pl, flags);
        }
        pl->rotation.y = (sint16)(pl->rotation.y - 0x80) + (rand() & 0x100);
        if ((pl->value71 & 0x7F) == 1)
        {
            pl->routine_3 = 1;
        }
        else
        {
            pl->routine_3 = 0;
        }
    }
    else if ((uint8)(pl->routine_1 - 0xB) >= 2)
    {
        pl->value73 -= 4;
        if ((sint8)pl->value73 < 0)
        {
            pl->value73 = 0;
        }
    }
}

void pl_damage_state_set(PL_WORK *pl, uint32 flags)
{
    uint8 state;

    if (pl->value74 != 0 || (flags & 0x41000000) || pl->life <= 0)
    {
        state = 0xC;
    }
    else
    {
        if (pl->routine_1 == 0xB && pl->routine_2 < 2)
        {
            return;
        }
        state = 0xB;
    }
    pl->routine_1 = state;
    pl->routine_2 = 0;
}

void Pl_move_dead(PL_WORK *pl)
{
    pl->rotation_prev = pl->rotation;
    pl->value_ad      = pl->value_ac;
    pl->value_b2      = pl->life;
    pl->value75       = pl->value74;
    pl->value77       = pl->value76;
    Scr_collision_check(pl);
    pl->position_prev       = pl->position;
    pl->position_fixed_prev = pl->position_fixed;
    if (pl->routine_1 == 0)
    {
        Game_work.routine_0 = 9;
        Game_work.routine_1 = 0;
        pl->routine_1++;
    }
}

void Pl_move_demo(PL_WORK *pl)
{
    uint8 cur;
    uint32 id;

    switch (pl->routine_1)
    {
        case 0:
            pl->position_saved       = pl->position;
            pl->position_saved_fixed = pl->position_fixed;
            pl->rotation_saved       = pl->rotation;
            pl->value448             = 0xFF;
            // fall through
        case 1:
            Pl00_shot_enable_off(pl);
            pl->flags_bd  = 0x40;
            pl->routine_1 = 2;
            // fall through
        case 2:
            pl->position_prev       = pl->position;
            pl->position_fixed_prev = pl->position_fixed;
            pl->rotation_prev       = pl->rotation;
            pl->value_ad            = pl->value_ac;
            pl->position            = pl->position_saved;
            pl->position_fixed      = pl->position_saved_fixed;
            pl->rotation            = pl->rotation_saved;
            id                      = pl->value448;
            pl->flags_bd &= 0xF7;
            if (id != 0xFF)
            {
                cur = pl->value_ac;
                if (cur != id)
                {
                    pl_anim_set(pl, id, 0, 1);
                    pl->value448 = 0xFF;
                }
                else if (pl->value_a7 == -1)
                {
                    pl_anim_set(pl, cur, 0, 1);
                    pl->value448 = 0xFF;
                }
            }
            if (pl->value_a6 & 0x80)
            {
                Sound_call(0x90, 0, 0);
            }
            break;
        case 3:
            Pl00_shot_enable_on(pl);
            *(uint32 *)&pl->kind = 1;
            pl->flags_bd         = 8;
            break;
    }
    Solid_mover_set(pl);
}

void pl_anim_set(PL_WORK *pl, uint8 id, sint16 arg2, uint8 flag)
{
    if (flag)
    {
        pl->value_ad = 0xFF;
    }
    else if (id == pl->value_ad)
    {
        return;
    }
    switch (id)
    {
        case 0xC:
            if (pl->value_ad == 1)
            {
                arg2 = pl->value_a4 * 2 + 4;
                if (arg2 >= 0x20)
                {
                    arg2 = pl->value_a4 * 2 - 0x1C;
                }
            }
            break;
        case 1:
            if (pl->value_ad == 0xC)
            {
                arg2 = pl->value_a4 - 4;
                if (arg2 < 0)
                {
                    arg2 = pl->value_a4 + 0x1C;
                }
                arg2 = arg2 >> 1;
            }
            break;
    }
    pl->value_ac = id;
    pl->value_a4 = arg2;
}

void Pl_move(PL_WORK *pl, sint32 x, sint32 y, sint32 z)
{
    pl->rotation.y -= pl->value116;
    Obj_pos_set((CODE14_WORK *)pl, x, y, z);
    pl->rotation.y += pl->value116;
}

void pl_pitch_pad(PL_WORK *pl, sint32 a1, sint32 a2)
{
    uint16 first;
    uint16 second;

    first  = pl->key_forward;
    second = pl->key_back;
    if (pl->pad_cur & (first | second))
    {
        pl_pitch_accel(pl, a1, a2, PL_GAME_FLAG55 ? second : first);
    }
    else
    {
        pl->value10c = 0;
    }
}

void pl_pitch_accel(PL_WORK *pl, sint32 a1, sint32 a2, uint16 mask)
{
    if (pl->pad_cur & mask)
    {
        if (pl->value10c < 0)
        {
            pl->value10c = 0;
        }
        else
        {
            pl->value10c += a1;
            if (a2 < pl->value10c)
            {
                pl->value10c = a2;
            }
        }
        pl->value108 += pl->value10c;
        if (pl->value108 > 0x200)
        {
            pl->value108 = 0x200;
        }
    }
    else
    {
        if (pl->value10c > 0)
        {
            pl->value10c = 0;
        }
        else
        {
            pl->value10c -= a1;
            if (pl->value10c < -a2)
            {
                pl->value10c = -a2;
            }
        }
        pl->value108 += pl->value10c;
        if (pl->value108 < -0x200)
        {
            pl->value108 = -0x200;
        }
    }
}

sint32 pl_turn_l1r1(PL_WORK *pl, sint32 a1, sint32 a2)
{
    sint32 result;
    uint32 both;
    uint32 held;

    both   = pl->key_turn_l | pl->key_turn_r;
    held   = pl->pad_cur & both;
    result = 0;
    if (held != 0 && held != both)
    {
        result = pl_turn_accel(pl, a1, a2, pl->key_turn_r);
    }
    else
    {
        pl->value_b4 = 0;
    }
    return result;
}

sint32 pl_turn_pad(PL_WORK *pl, sint32 a1, sint32 a2, sint32 a3)
{
    uint32 pad;
    uint16 m1;
    uint16 m2;
    sint32 result;

    result = 0;
    pad    = pl->pad_cur;
    m1     = pl->key_turn_r | pl->key_run_r;
    m2     = pl->key_turn_l | pl->key_run_l;
    if (!(pad & m1) != !(pad & m2))
    {
        result = pl_turn_accel(pl, a1, a2, m1);
        if (a3 != 0)
        {
            // Both arms make the same call. GCC merges them only after register allocation,
            // so pad still lives across pl_turn_accel (retail's s1 and 0x28-byte frame).
            if (pad & pl->key_turn_r)
            {
                pl_anim_set(pl, 0xD, 0, 0);
            }
            else
            {
                pl_anim_set(pl, 0xD, 0, 0);
            }
        }
    }
    else
    {
        if (a3 != 0)
        {
            pl_anim_set(pl, 0, 0, 0);
        }
        pl->value_b4 = 0;
    }
    return result;
}

sint32 pl_turn_accel(PL_WORK *pl, sint32 a1, sint32 a2, sint32 mask)
{
    sint32 ret;

    if (pl->pad_cur & mask)
    {
        ret = 1;
        if (pl->value_b4 < 0)
        {
            pl->value_b4 = 0;
        }
        else
        {
            pl->value_b4 += a1;
            if (pl->value_b4 > a2)
            {
                pl->value_b4 = a2;
            }
            pl->value_b6 = a1;
        }
    }
    else
    {
        ret = 2;
        if (pl->value_b4 > 0)
        {
            pl->value_b4 = 0;
        }
        else
        {
            pl->value_b4 -= a1;
            if (pl->value_b4 < -a2)
            {
                pl->value_b4 = -a2;
            }
            pl->value_b6 = -a1;
        }
    }
    pl->rotation.y = (pl->rotation.y + pl->value_b4) & 0xFFF;
    return ret;
}

sint32 Player_atari_off(void)
{
    if (Player_work.life >= 0)
    {
        Player_work.value449 = 1;
        return 1;
    }
    return 0;
}

sint32 Player_control_off(PL_WORK *pl)
{
    pl->value449 = -1;
    pl->flags_bd |= 0x40;
    if (pl->routine_0 & 1)
    {
        if (pl->kind == 1)
        {
            switch (pl->routine_1)
            {
                case 14:
                    if (Game_work.flags_83 != 0)
                    {
                        case 0:
                            if (Pl_shot_busy(pl) == 0)
                            {
                                goto reset;
                            }
                    }
                default:
                    if ((*(uint32 *)&pl->kind & 0xFFFF00) == 0x20D00)
                    {
                        pl->routine_2 = 6;
                    }
            }
        }
        if (pl->kind != 3)
        {
            return 0;
        }
    }
reset:
    pl->value_b4 = 0;
    Sound_key_off_voice(0x16);
    Sound_key_off_voice(0x17);
    return 1;
}

void Player_control_on(PL_WORK *pl)
{
    func_8002D004(pl, 0, 0);
    pl->value449 = 0;
    pl->value111 = 0;
    pl->value71  = 0;
    pl->value73  = 0;
    pl->value188 = 0;
    pl->value18c = 0;
    pl->value190 = 0;
    pl->value194 = 0;
    pl->value168 = 0;
    pl->value169 = 0;
    if (pl->routine_0 & 1)
    {
        pl->routine_0 = 3;
        pl->flags06   = 3;
        if (pl->kind == 3)
        {
            pl->routine_1 = 3;
            return;
        }
    }
    pl->flags_bd = 8;
}

void Pl00_shot_enable_off(PL_WORK *pl)
{
    pl->shot_status_flag = 0;
    pl->value177         = 0;
    pl->value175 &= ~1;
    pl->value104 = pl->value_ac;
    pl->value100 = pl->value_a4;
}

void Pl00_shot_enable_on(PL_WORK *pl)
{
    pl->value175 |= 1;
}

sint32 Pl_anim_buster(PL_WORK *pl)
{
    switch (pl->routine_1)
    {
        case 0:
        case 1:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 9:
        case 10:
            break;
        case 2:
        case 8:
            switch ((sint8)pl->value112)
            {
                case 0:
                    return 0x60 + 1;
                case 1:
                    return 0x60 + 3;
                case 3:
                    return 0x60 + 2;
            }
            break;
    }
    return 0x60;
}

sint32 Pl_anim_special(PL_WORK *pl)
{
    switch (pl->routine_1)
    {
        case 0:
        case 1:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 9:
        case 10:
            break;
        case 2:
        case 8:
            switch ((sint8)pl->value112)
            {
                case 0:
                    return 0x40 + 1;
                case 1:
                    return 0x40 + 3;
                case 3:
                    return 0x40 + 2;
            }
            break;
    }
    return 0x40;
}

void func_800424D4(PL_WORK *pl, PL_CMD *cmd)
{
    cmd->value00 = 7;
    cmd->value02 = 0x80;
    cmd->value03 = 2;
    cmd->value38 = pl->table3a8[pl->routine_2 >> 1];
}

void pl_effect_pos_set(PL_WORK *pl, PL_EFFECT *effect, sint32 index)
{
    effect->routine_0 = 3;
    effect->value02   = 0x83;
    effect->value03   = 0;
    switch ((sint16)index)
    {
        case 0:
            effect->position.x = pl->position.x;
            effect->position.y = pl->position.y + 0x10;
            effect->position.z = pl->position.z;
            break;
        case 1:
            effect->position.x = PL_TABLE_VEC(pl, 14)->x;
            effect->position.y = PL_TABLE_VEC(pl, 14)->y + 0x10;
            effect->position.z = PL_TABLE_VEC(pl, 14)->z;
            break;
        case 2:
            effect->position.x = PL_TABLE_VEC(pl, 11)->x;
            effect->position.y = PL_TABLE_VEC(pl, 11)->y + 0x10;
            effect->position.z = PL_TABLE_VEC(pl, 11)->z;
            break;
    }
}
