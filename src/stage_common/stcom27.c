#include "common.h"
#include "stage_common/stcom27.h"

// Shared stage code, linked into 3 stages: r3_st09, r3_st14 and r3_st1a. Each stage compiles
// this file against its own copy of the assembly (TARGET_ASM, set by tools/cc.sh); the names
// are the same in every stage's registry.
#define ASM_DIR TARGET_ASM "/stage_common/stcom27"

// A creature that roams, then charges at the player and is knocked over by hits.
void stcom27_move(STCOM27_WORK *wp)
{
    stcom27_move_tbl[wp->routine_0](wp);
}

// Sets up the creature and starts it standing.
void stcom27_move_init(STCOM27_WORK *wp)
{
    wp->scale.v.x   = 0x200;
    wp->scale.v.y   = 0x200;
    wp->scale.v.z   = 0x200;
    wp->scale.v.pad = 0x4210;
    wp->rotation.x  = 0;
    wp->rotation.z  = 0;
    wp->velocity.z  = 0;
    wp->velocity.y  = 0;
    wp->velocity.x  = 0;
    wp->accel.z     = 0;
    wp->accel.y     = 0;
    wp->accel.x     = 0;
    wp->flags_0 |= 2;
    Model_work_init(wp, NULL);
    Search_Player_Init(wp->ctl.track_0, 4, -0x60);
    wp->data_78 = stcom27_D_000C;
    wp->life_6E = wp->life_max_B0 = Em_param_tbl[wp->kind_2]->life_8 << 4;
    wp->jump_74                   = 0;
    wp->damage_B2                 = 0;
    wp->ctl.sees_10               = 0;
    wp->ctl.flags_14              = 0;
    wp->ctl.hit_20                = 0;
    wp->ctl.value_24              = 0;
    wp->ctl.kind_30               = Em_param_tbl[wp->kind_2]->value_B | 0x1000000;
    wp->ctl.timer_34              = 0x100;
    wp->ctl.timer_36              = 0x40;
    EM_Char_Set(wp, 0, 0);
    wp->routine_0 = 1;
    wp->routine_1 = 0;
    wp->routine_2 = 0;
    wp->routine_3 = 0;
}

// Each frame while alive: takes hits, runs the state (state_tbl), animates and moves.
void stcom27_move_main(STCOM27_WORK *wp)
{
    STCOM27_CTL *ctl;

    wp->position_prev       = wp->position;
    wp->position_fixed_prev = wp->position_fixed;
    wp->rotation_prev       = wp->rotation;
    wp->scale.v.pad         = 0x4210;
    wp->anim_prev_AD        = wp->anim_AC;
    wp->jump_prev_75        = wp->jump_74;
    ctl                     = &wp->ctl;
    stcom27_damage(wp);
    if (wp->routine_0 != 2)
    {
        ctl->sees_10 = Search_Player(ctl->track_0, wp);
        stcom27_state_tbl[wp->routine_1](wp);
        if (wp->anim_AC != wp->anim_prev_AD)
        {
            EM_Char_Set(wp, wp->anim_AC, 0);
        }
        else
        {
            EM_Char_Move(wp);
        }
        stcom27_ground_check(wp);
        stcom27_hit_set(wp);
        if (wp->flags_0 & 2)
        {
            Model_Shadow_Trans(wp, 0x78, 0x200, 0);
        }
    }
}

// Takes hits: knocked over when the damage adds up, killed while it is down.
void stcom27_damage(STCOM27_WORK *wp)
{
    STCOM_EFFECT *effect;
    STCOM_SVECTOR *pos;
    uint32 damage;
    sint32 stun;
    uint8 limit;

    if (wp->damage_B2 != 0)
    {
        wp->damage_B2--;
    }
    damage = wp->ctl.hit_20 & 0xFFFFFF;
    if (damage == 0)
    {
        return;
    }
    stun = wp->routine_1 == 6;
    if (wp->routine_1 == 8)
    {
        stun = 1;
    }
    if (stun)
    {
        Sound_call2(0xA3, &wp->position);
        return;
    }
    limit = Em_param_tbl[wp->kind_2]->limit_9;
    if (wp->routine_1 == 7)
    {
        pos = &wp->position;
        wp->life_6E -= damage;
        Sound_call2(0xA0, pos);
        wp->scale.v.pad = 0x7FFF;
        if (wp->life_6E <= 0)
        {
            effect = Open_EFFECT_WORK();
            if (effect != NULL)
            {
                effect->flags_0         = 5;
                effect->kind_2          = 0x80;
                effect->variant_3       = 0;
                effect->sprite.position = wp->position;
            }
#ifdef __wasm32__
            Obj_item_drop(wp, pos);
#else
            Obj_item_drop(wp, pos, effect);
#endif
            wp->routine_0 = 2;
            wp->routine_1 = 0;
            wp->routine_2 = 0;
            return;
        }
        wp->ctl.damage_3E += damage;
        if ((wp->ctl.hit_20 & 0x20000000) || (sint16)wp->ctl.damage_3E >= limit * 16)
        {
            wp->anim_AC = 5;
            stcom27_knock_start(wp);
            wp->routine_1 = 8;
            wp->routine_2 = 0;
        }
        return;
    }
    wp->damage_B2 += damage;
    if ((wp->ctl.hit_20 & 0x20000000) || wp->damage_B2 >= limit * 16)
    {
        wp->anim_AC       = 3;
        wp->damage_B2     = 0;
        wp->ctl.damage_3E = 0;
        wp->ctl.kind_30   = Em_param_tbl[wp->kind_2]->value_B | 0x1000000;
        stcom27_knock_start(wp);
        wp->routine_1 = 6;
        wp->routine_2 = 0;
        return;
    }
    Sound_call2(0xA3, &wp->position);
}

// Knocked into the air, away from the hit.
void stcom27_knock_start(STCOM27_WORK *wp)
{
    STCOM_SVECTOR *pos;
    sint32 angle;

    pos = &wp->position;
    Sound_call2(0x197, pos);
    angle = Calc_hit_side(pos, &wp->ctl.from_28, wp->rotation.y, 0);
    if (angle & 0xFFFF0000)
    {
        wp->rotation.y = (angle + 0x800) & 0xFFF;
    }
    else
    {
        wp->rotation.y = angle;
    }
    wp->velocity.x = 0x80;
    wp->velocity.y = -0x280;
    wp->accel.y    = 0x40;
    stcom27_air_move(wp);
}

// Registers the hit box.
void stcom27_hit_set(STCOM27_WORK *wp)
{
    wp->ctl.hit_20 = 0;
    wp->ctl.pos_18 = wp->position;
    wp->ctl.pos_18.y -= 0x50;
    Set_Atari(&wp->ctl.pos_18, 0x50, 0, 0x8000, wp->index_1, &wp->ctl.hit_20,
              wp->ctl.kind_30 | 0x4080000, 0x800000, 0);
}

// Keeps it on the ground: back to its last position at a ledge.
void stcom27_ground_check(STCOM27_WORK *wp)
{
    STCOM27_CTL *ctl;
    sint32 ground;

    wp->ctl.flags_14 = 0;
    ctl              = &wp->ctl;
    if (Scr_collision_check(wp) & 0xF00)
    {
        wp->ctl.flags_14 |= 1;
    }
    if (wp->jump_74 == 0)
    {
        ground = Scr_height_check(wp);
        if (ground != wp->position_prev.y)
        {
            if (wp->jump_prev_75 == 0)
            {
                wp->position       = wp->position_prev;
                wp->position_fixed = wp->position_fixed_prev;
                wp->ctl.flags_14 |= 4;
            }
        }
        else
        {
            wp->position.y       = ground;
            wp->position_fixed.y = ground << 16;
        }
        ctl->flags_14 |= 2;
    }
}

// Starts chasing once it sees the player.
void stcom27_notice(STCOM27_WORK *wp)
{
    if (wp->ctl.sees_10 != 0)
    {
        wp->anim_AC    = 1;
        wp->velocity.x = 0x20;
        wp->velocity.y = 0;
        wp->accel.y    = 0;
        wp->routine_1  = 3;
        wp->routine_2  = 0;
    }
}

// Stands until the time is up, then walks.
void stcom27_idle(STCOM27_WORK *wp)
{
    if (--wp->ctl.timer_34 == 0)
    {
        stcom27_walk_init(wp);
        return;
    }
    stcom27_notice(wp);
}

// Walks; turns around at a wall or a ledge, stands when the time is up.
void stcom27_walk(STCOM27_WORK *wp)
{
    if (wp->ctl.flags_14 & 5)
    {
        stcom27_turn_init(wp);
        return;
    }
    if (--wp->ctl.timer_36 == 0)
    {
        stcom27_idle_init(wp);
        return;
    }
    Obj_pos_set(wp, wp->velocity.x, wp->velocity.y, wp->velocity.z);
    stcom27_notice(wp);
}

// Turns to ctl.angle_3C, then walks.
void stcom27_turn(STCOM27_WORK *wp)
{
    if (wp->rotation.y == wp->ctl.angle_3C)
    {
        stcom27_walk_init(wp);
        return;
    }
    if (wp->ctl.angle_3C >= wp->rotation.y)
    {
        wp->rotation.y = wp->rotation.y + 0x10;
    }
    else
    {
        wp->rotation.y = wp->rotation.y - 0x10;
    }
    wp->rotation.y &= 0xFFF;
    stcom27_notice(wp);
}

// Turns toward the player and walks at it; charges once it faces the player close by.
void stcom27_chase(STCOM27_WORK *wp)
{
    STCOM_PL_WORK *pl;

    pl = &Player_work;
    if (wp->ctl.flags_14 & 5)
    {
        stcom27_turn_init(wp);
        return;
    }
    wp->rotation.y =
        (wp->rotation.y + Muku(&wp->position, &pl->position, wp->rotation.y, 0x40)) & 0xFFF;
    Obj_pos_set(wp, wp->velocity.x, wp->velocity.y, wp->velocity.z);
    if (Angle_ck(&wp->position_fixed, wp->rotation.y, 0x400, &pl->position_fixed,
                 Player_work.rotation.y, 0x800) != 0 &&
        wp->value_80 <= 1000000)
    {
        wp->ctl.timer_38 = 0x10;
        wp->anim_AC      = 2;
        wp->routine_1    = 4;
        wp->routine_2    = 0;
    }
}

// routine_1 4: the charge, by routine_2 (charge_tbl).
void stcom27_charge(STCOM27_WORK *wp)
{
    stcom27_charge_tbl[wp->routine_2](wp);
}

// Paws the ground facing the player, then charges.
void stcom27_charge_aim(STCOM27_WORK *wp)
{
    STCOM_SVECTOR *pos;
    sint32 step;

    pos = &wp->position;
    wp->rotation.y =
        (wp->rotation.y + Muku(pos, &Player_work.position, wp->rotation.y, 0x40)) & 0xFFF;
    step = wp->frame_A4 == 0;
    if (wp->frame_A4 == 4)
    {
        step = 1;
    }
    if (step)
    {
        Sound_call2(0x196, pos);
    }
    if (--wp->ctl.timer_38 == 0)
    {
        wp->ctl.timer_38 = 0x3C;
        wp->velocity.x   = 0x200;
        wp->velocity.y   = 0;
        wp->accel.y      = 0;
        wp->ctl.kind_30  = Em_param_tbl[wp->kind_2]->value_A | 0x1000000;
        wp->routine_2++;
    }
}

// Runs straight ahead until it hits a wall, reaches a ledge or runs out of time.
void stcom27_charge_run(STCOM27_WORK *wp)
{
    STCOM27_CTL *ctl;
    sint32 stop;
    sint32 step;

    stop = 0;
    ctl  = &wp->ctl;
    if (wp->ctl.flags_14 & 1)
    {
        stop = 1;
        if (wp->flags_6 & 0x80)
        {
            Swing_Camera2(0x1E, 1000, 5000, &wp->position);
        }
    }
    step = wp->frame_A4 == 0;
    if (wp->frame_A4 == 4)
    {
        step = 1;
    }
    if (step)
    {
        Sound_call2(0x196, &wp->position);
    }
    if (ctl->flags_14 & 4)
    {
        stop = 1;
    }
    if (--ctl->timer_38 == 0)
    {
        stop = 1;
    }
    if (stop)
    {
        wp->anim_AC   = 0;
        ctl->kind_30  = Em_param_tbl[wp->kind_2]->value_B | 0x1000000;
        ctl->timer_38 = 0x30;
        wp->routine_2++;
        return;
    }
    Obj_pos_set(wp, wp->velocity.x, wp->velocity.y, wp->velocity.z);
}

// Rests after the charge, then backs off.
void stcom27_charge_wait(STCOM27_WORK *wp)
{
    if (--wp->ctl.timer_38 == 0)
    {
        stcom27_back_off_init(wp);
    }
}

// Backs away facing the player, then walks again.
void stcom27_back_off(STCOM27_WORK *wp)
{
    STCOM_PL_WORK *pl;

    pl = &Player_work;
    if (wp->ctl.flags_14 & 5)
    {
        wp->velocity.x = 0;
    }
    if (--wp->ctl.timer_3A == 0)
    {
        stcom27_walk_init(wp);
        return;
    }
    wp->rotation.y =
        (wp->rotation.y + Muku(&wp->position, &pl->position, wp->rotation.y, 0x80)) & 0xFFF;
    Obj_pos_set(wp, wp->velocity.x, wp->velocity.y, wp->velocity.z);
}

// routine_1 6 and 8: knocked over, by routine_2 (fly, then bounce).
void stcom27_knock_back(STCOM27_WORK *wp)
{
    if (wp->routine_2 == 0)
    {
        stcom27_knock_fly(wp);
        return;
    }
    stcom27_knock_bounce(wp);
}

// Flies until it lands, then bounces once.
void stcom27_knock_fly(STCOM27_WORK *wp)
{
    stcom27_wall_stop(wp);
    if (wp->jump_74 == 0)
    {
        if (wp->routine_1 == 6)
        {
            wp->velocity.x = 0x40;
            wp->velocity.y = -0xE0;
            wp->accel.y    = 0x20;
        }
        else
        {
            wp->velocity.x = 0x40;
            wp->velocity.y = -0xA0;
            wp->accel.y    = 0x20;
        }
        wp->routine_2++;
    }
    stcom27_air_move(wp);
}

// Lands: down after the first knock (routine_1 6), or back on its feet after a knock while
// down (8).
void stcom27_knock_bounce(STCOM27_WORK *wp)
{
    stcom27_wall_stop(wp);
    if (wp->jump_74 == 0)
    {
        if (wp->routine_1 == 6)
        {
            wp->anim_AC   = 4;
            wp->routine_1 = 7;
            wp->routine_2 = 0;
            return;
        }
        stcom27_turn_init(wp);
        return;
    }
    stcom27_air_move(wp);
}

// Stops moving when it hits a wall.
void stcom27_wall_stop(STCOM27_WORK *wp)
{
    if (wp->ctl.flags_14 & 1)
    {
        wp->velocity.x = 0;
        wp->velocity.z = 0;
    }
}

// In the air: moves and falls.
void stcom27_air_move(STCOM27_WORK *wp)
{
    wp->jump_74 = 1;
    Obj_pos_set(wp, wp->velocity.x, wp->velocity.y, wp->velocity.z);
    wp->velocity.y += wp->accel.y;
}

// routine_1 7, down: nothing to do (stcom27_damage takes the hits).
void stcom27_func_0E2C(void)
{
}

// Stands for 0x100 frames (routine_1 0).
void stcom27_idle_init(STCOM27_WORK *wp)
{
    wp->ctl.timer_34 = 0x100;
    wp->anim_AC      = 0;
    wp->routine_1    = 0;
    wp->routine_2    = 0;
}

// Walks slowly for 0x40 frames (routine_1 1).
void stcom27_walk_init(STCOM27_WORK *wp)
{
    wp->anim_AC      = 1;
    wp->velocity.x   = 0x18;
    wp->velocity.y   = 0;
    wp->accel.y      = 0;
    wp->ctl.timer_36 = 0x40;
    wp->routine_1    = 1;
    wp->routine_2    = 0;
}

// Turns around, half a circle (routine_1 2).
void stcom27_turn_init(STCOM27_WORK *wp)
{
    if (wp->anim_AC != 1)
    {
        wp->anim_AC = 1;
    }
    wp->routine_1    = 2;
    wp->routine_2    = 0;
    wp->ctl.angle_3C = (wp->rotation.y + 0x800) & 0xFFF;
}

// Backs away from the player for 12 frames (routine_1 5).
void stcom27_back_off_init(STCOM27_WORK *wp)
{
    wp->anim_AC      = 2;
    wp->velocity.x   = -0x80;
    wp->ctl.timer_3A = 0xC;
    wp->velocity.y   = 0;
    wp->accel.y      = 0;
    wp->routine_1    = 5;
    wp->routine_2    = 0;
}

// Frees the work slot (routine_0 2, after it dies).
void stcom27_erase(STCOM27_WORK *wp)
{
    Close_EM_WORK(wp);
}
