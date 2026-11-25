#include "common.h"
#include "stage_common/stcom28.h"

// Shared stage code, linked into 3 stages: r3_st09, r3_st0e and r3_st13. Each stage compiles
// this file against its own copy of the assembly (TARGET_ASM, set by tools/cc.sh); the names
// are the same in every stage's registry.
#define ASM_DIR TARGET_ASM "/stage_common/stcom28"

// A bobbing creature that roams, turns back at walls, charges at the player and leaps.
void stcom28_move(STCOM28_WORK *wp)
{
    stcom28_move_tbl[wp->routine_0](wp);
}

// Sets up the creature and picks its first move.
void stcom28_move_init(STCOM28_WORK *wp)
{
    wp->rotation.x = 0;
    wp->rotation.z = 0;
    wp->velocity.z = 0;
    wp->velocity.y = 0;
    wp->velocity.x = 0;
    wp->accel.z    = 0;
    wp->accel.y    = 0;
    wp->accel.x    = 0;
    wp->flags_0 |= 6;
    Model_work_init(wp, NULL);
    Search_Player_Init(wp->ctl.track_C, 9, -0x60);
    wp->data_78       = stcom28_D_000C;
    wp->jump_74       = 0;
    wp->ctl.counter_0 = 0;
    wp->ctl.sees_1C   = 0;
    wp->ctl.flags_20  = 0;
    wp->ctl.hit_2C    = 0;
    wp->ctl.value_30  = 0;
    wp->ctl.kind_3C   = 0;
    wp->routine_0     = 1;
    wp->routine_1     = 0;
    wp->routine_2     = 0;
    wp->routine_3     = 0;
    stcom28_choose(wp);
}

// Each frame: takes hits, runs the state (state_tbl), then bobs, moves and registers the hit box.
void stcom28_move_main(STCOM28_WORK *wp)
{
    STCOM_MATRIX *m;
    STCOM_FIXED_VECTOR scale;

    wp->position_prev       = wp->position;
    wp->position_fixed_prev = wp->position_fixed;
    wp->rotation_prev       = wp->rotation;
    wp->color_B4            = 0x808080;
    wp->jump_prev_75        = wp->jump_74;
    stcom28_damage(wp);
    wp->ctl.sees_1C = Search_Player(wp->ctl.track_C, wp);
    stcom28_state_tbl[wp->routine_1](wp);
    if (wp->routine_0 != 2)
    {
        wp->pos_AC = wp->position;
        m          = &wp->matrix;
        wp->pos_AC.y += stcom28_bob_tbl[wp->ctl.counter_0++];
        wp->ctl.counter_0 &= 0x1F;
        RotMatrixYXZ(&wp->rotation, m);
        scale.x = 0x200;
        scale.y = 0x200;
        scale.z = 0x200;
        ScaleMatrix(m, &scale);
        stcom28_ground_check(wp);
        if (wp->routine_1 != 6)
        {
            stcom28_hit_set(wp);
        }
        if (wp->flags_0 & 2)
        {
            Model_Shadow_Trans(wp, 0x28, 0x200, 0);
        }
    }
}

// Picks the next move: mostly roaming for a while, sometimes waiting.
void stcom28_choose(STCOM28_WORK *wp)
{
    if ((rand() & 0xF) < 0xE)
    {
        wp->ctl.turn_1  = rand() & 1;
        wp->velocity.x  = 0x64;
        wp->velocity.y  = 0;
        wp->accel.y     = 0;
        wp->ctl.timer_2 = stcom28_time_tbl[rand() & 7];
        wp->routine_1   = 0;
    }
    else
    {
        wp->ctl.timer_4 = stcom28_time_tbl[rand() & 7];
        wp->routine_1   = 2;
    }
    wp->routine_2 = 0;
    stcom28_move_main(wp);
}

// Charges once it sees the player.
void stcom28_notice(STCOM28_WORK *wp)
{
    if (wp->ctl.sees_1C != 0)
    {
        wp->rotation.x  = 0x40;
        wp->velocity.x  = 0xC8;
        wp->ctl.step_3E = 0x20;
        wp->ctl.timer_6 = 0x5A;
        wp->velocity.y  = 0;
        wp->accel.y     = 0;
        wp->routine_1   = 4;
        wp->routine_2   = 0;
    }
}

// Takes a hit: knocked into the air, away from it.
void stcom28_damage(STCOM28_WORK *wp)
{
    STCOM28_CTL *ctl;
    sint32 angle;

    ctl = &wp->ctl;
    if (wp->routine_1 != 6 && (wp->ctl.hit_2C & 0x38000000))
    {
        wp->ctl.kind_3C = 0;
        angle           = Calc_hit_side(&wp->position, &wp->ctl.from_34, wp->rotation.y, 0);
        if (angle & 0xFFFF0000)
        {
            wp->rotation.y = (angle + 0x800) & 0xFFF;
        }
        else
        {
            wp->rotation.y = angle;
        }
        wp->velocity.x = 0x80;
        wp->velocity.y = -0x300;
        wp->accel.y    = 0x80;
        ctl->timer_8   = 8;
        wp->jump_74    = 1;
        Obj_pos_set(wp, wp->velocity.x, wp->velocity.y, wp->velocity.z);
        wp->routine_1 = 6;
        wp->routine_2 = 0;
        wp->velocity.y += wp->accel.y;
    }
}

// Keeps it on the ground: back to its last position at a ledge.
void stcom28_ground_check(STCOM28_WORK *wp)
{
    STCOM28_CTL *ctl;
    sint32 ground;
    sint16 rot;

    rot              = wp->rotation.x;
    wp->rotation.x   = 0;
    wp->ctl.flags_20 = 0;
    ctl              = &wp->ctl;
    if (Scr_collision_check(wp) & 0xF00)
    {
        wp->ctl.flags_20 |= 1;
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
                wp->ctl.flags_20 |= 4;
            }
        }
        else
        {
            wp->position.y            = ground;
            wp->position_fixed_prev.y = ground << 16; // not position_fixed
        }
        ctl->flags_20 |= 2;
    }
    wp->rotation.x = rot;
}

// Registers the hit box.
void stcom28_hit_set(STCOM28_WORK *wp)
{
    wp->ctl.hit_2C = 0;
    wp->ctl.pos_24 = wp->position;
    wp->ctl.pos_24.y -= 0x28;
    Set_Atari(&wp->ctl.pos_24, 0x28, 0, 0x8000, wp->index_1, &wp->ctl.hit_2C,
              wp->ctl.kind_3C | 0x4080000, 0x800000, 0);
}

// routine_1 0: roams, by routine_2 (raise the head, then walk).
void stcom28_roam(STCOM28_WORK *wp)
{
    if (wp->routine_2 == 0)
    {
        stcom28_roam_rise(wp);
    }
    else
    {
        stcom28_roam_walk(wp);
    }
}

// Raises its head before it roams.
void stcom28_roam_rise(STCOM28_WORK *wp)
{
    if (wp->rotation.x == 0xC0)
    {
        wp->routine_2++;
    }
    else
    {
        wp->rotation.x += 0x20;
    }
}

// Roams, turning now and then.
void stcom28_roam_walk(STCOM28_WORK *wp)
{
    if (stcom28_edge_check(wp) != 0)
    {
        return;
    }
    if (--wp->ctl.timer_2 == 0)
    {
        stcom28_choose(wp);
        return;
    }
    if ((rand() & 0xF) >= 0xA)
    {
        if (wp->ctl.turn_1 != 0)
        {
            wp->rotation.y = wp->rotation.y + 0x40;
        }
        else
        {
            wp->rotation.y = wp->rotation.y - 0x40;
        }
        wp->rotation.y &= 0xFFF;
    }
    Obj_pos_set(wp, wp->velocity.x, wp->velocity.y, wp->velocity.z);
    stcom28_notice(wp);
}

// Turns back at a wall (1), leaps at a ledge (-1).
sint32 stcom28_edge_check(STCOM28_WORK *wp)
{
    if (wp->ctl.flags_20 & 1)
    {
        stcom28_turn_init(wp);
        return 1;
    }
    if (wp->ctl.flags_20 & 4)
    {
        stcom28_leap_init(wp);
        return -1;
    }
    return 0;
}

// Steps back from the wall and turns around (routine_1 1).
void stcom28_turn_init(STCOM28_WORK *wp)
{
    wp->position       = wp->position_prev;
    wp->position_fixed = wp->position_fixed_prev;
    wp->routine_1      = 1;
    wp->routine_2      = 0;
    wp->ctl.angle_A    = (wp->rotation.y + 0x800) & 0xFFF;
}

// Leaps forward, its hit box armed (routine_1 5).
void stcom28_leap_init(STCOM28_WORK *wp)
{
    wp->velocity.x = 0x180;
    wp->velocity.y = -0x200;
    wp->accel.y    = 0x50;
    wp->rotation.x = 0xE80;
    Sound_call2(0x15C, &wp->position);
    wp->jump_74 = 1;
    Obj_pos_set(wp, wp->velocity.x, wp->velocity.y, wp->velocity.z);
    wp->velocity.y += wp->accel.y;
    wp->ctl.kind_3C = Shl_param_tbl[wp->kind_2]->value_A;
    wp->routine_1   = 5;
    wp->routine_2   = 0;
}

// routine_1 1: turns around to ctl.angle_A, levelling its head, then picks its next move.
void stcom28_turn(STCOM28_WORK *wp)
{
    STCOM28_CTL *ctl;
    sint16 x;

    ctl = &wp->ctl;
    if (wp->rotation.y == wp->ctl.angle_A)
    {
        stcom28_choose(wp);
        return;
    }
    x = wp->rotation.x;
    if (x != 0)
    {
        if (x >= 0x800)
        {
            wp->rotation.x = x + 0x20;
        }
        else
        {
            wp->rotation.x = x - 0x20;
        }
        wp->rotation.x &= 0xFFF;
    }
    if (ctl->angle_A < wp->rotation.y)
    {
        wp->rotation.y = wp->rotation.y - 0x40;
    }
    else
    {
        wp->rotation.y = wp->rotation.y + 0x40;
    }
    wp->rotation.y &= 0xFFF;
}

// routine_1 2 and 3: waits for the time to run out; while waiting (2), not resting (3), it
// charges once it sees the player.
void stcom28_wait(STCOM28_WORK *wp)
{
    STCOM28_CTL *ctl;
    sint16 x;

    x   = wp->rotation.x;
    ctl = &wp->ctl;
    if (x != 0)
    {
        if (x >= 0x800)
        {
            wp->rotation.x = x + 0x20;
        }
        else
        {
            wp->rotation.x = x - 0x20;
        }
        wp->rotation.x &= 0xFFF;
    }
    if (ctl->timer_4 == 0)
    {
        stcom28_choose(wp);
        return;
    }
    ctl->timer_4--;
    if (wp->routine_1 == 2)
    {
        stcom28_notice(wp);
    }
}

// routine_1 4: the charge, by routine_2 (face the player, then run).
void stcom28_charge(STCOM28_WORK *wp)
{
    if (wp->routine_2 == 0)
    {
        stcom28_charge_aim(wp);
    }
    else
    {
        stcom28_charge_run(wp);
    }
}

// Turns to face the player.
void stcom28_charge_aim(STCOM28_WORK *wp)
{
    sint16 step;

    step = Muku(&wp->position, &Player_work.position, wp->rotation.y, 0x80);
    if (step != 0)
    {
        wp->rotation.y = (wp->rotation.y + step) & 0xFFF;
    }
    else
    {
        wp->routine_2++;
    }
}

// Charges, nodding, and leaps at the player once close in front of it.
void stcom28_charge_run(STCOM28_WORK *wp)
{
    STCOM_PL_WORK *pl;

    pl = &Player_work;
    if (stcom28_edge_check(wp) != 0)
    {
        return;
    }
    wp->rotation.x += wp->ctl.step_3E;
    if (wp->rotation.x == 0x40)
    {
        wp->ctl.step_3E = 0x20;
    }
    if (wp->rotation.x == 0x180)
    {
        wp->ctl.step_3E = -0x20;
    }
    if (wp->ctl.timer_6 != 0)
    {
        wp->ctl.timer_6--;
        Obj_pos_set(wp, wp->velocity.x, wp->velocity.y, wp->velocity.z);
        if (Angle_ck(&wp->position_fixed, wp->rotation.y, 0x400, &pl->position_fixed,
                     Player_work.rotation.y, 0x800) != 0)
        {
            if (wp->value_80 <= 40000)
            {
                stcom28_leap_init(wp);
            }
            return;
        }
        if (wp->value_80 < 250000)
        {
            return;
        }
    }
    wp->ctl.timer_4 = 0x1E;
    wp->routine_1   = 3;
    wp->routine_2   = 0;
}

// routine_1 5: the leap, by routine_2 (in the air, then falling back).
void stcom28_leap(STCOM28_WORK *wp)
{
    if (wp->routine_2 == 0)
    {
        stcom28_leap_air(wp);
    }
    else
    {
        stcom28_leap_back(wp);
    }
}

// In the air after a leap; turns back once the leap hits something, rests once it lands.
void stcom28_leap_air(STCOM28_WORK *wp)
{
    if (wp->ctl.flags_20 & 1)
    {
        wp->velocity.x = 0;
    }
    if (wp->jump_74 == 0)
    {
        wp->ctl.timer_4 = 0x1E;
        wp->ctl.kind_3C = 0;
        wp->routine_1   = 3;
        wp->routine_2   = 0;
        return;
    }
    if (wp->ctl.hit_2C & 0x2000000)
    {
        wp->ctl.kind_3C = 0;
        wp->rotation.y  = (wp->rotation.y + 0x800) & 0xFFF;
        wp->routine_2++;
        return;
    }
    Obj_pos_set(wp, wp->velocity.x, wp->velocity.y, wp->velocity.z);
    wp->velocity.y += wp->accel.y;
}

// Falls back after the leap hit something, then rests.
void stcom28_leap_back(STCOM28_WORK *wp)
{
    if (wp->ctl.flags_20 & 1)
    {
        wp->velocity.x = 0;
    }
    if (wp->jump_74 == 0)
    {
        wp->ctl.timer_4 = 0x1E;
        wp->routine_1   = 3;
        wp->routine_2   = 0;
        return;
    }
    Obj_pos_set(wp, wp->velocity.x, wp->velocity.y, wp->velocity.z);
    wp->velocity.y += wp->accel.y;
}

// routine_1 6: knocked about until it lands, hits a wall or the timer runs out, then bursts.
void stcom28_die(STCOM28_WORK *wp)
{
    STCOM_EFFECT *effect;
    sint32 burst;

    wp->rotation.x = (wp->rotation.x + 0x200) & 0xFFF;
    burst          = --wp->ctl.timer_8 == 0;
    if (wp->ctl.flags_20 & 1)
    {
        burst = 1;
    }
    if (wp->jump_74 == 0)
    {
        burst = 1;
    }
    if (burst)
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
        Obj_item_drop(wp, &wp->position);
#else
        Obj_item_drop(wp, &wp->position, effect);
#endif
        wp->routine_0 = 2;
        wp->routine_1 = 0;
        wp->routine_2 = 0;
        return;
    }
    Obj_pos_set(wp, wp->velocity.x, wp->velocity.y, wp->velocity.z);
    wp->velocity.y += wp->accel.y;
}

// Frees the work slot (routine_0 2, after it bursts).
void stcom28_erase(STCOM28_WORK *wp)
{
    Close_SHL_WORK(wp);
}
