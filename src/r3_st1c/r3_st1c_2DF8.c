#include "common.h"
#include "st1c.h"

// The first donation item the player holds (scene flags 0x54B to 0x552), or 0xFF.
uint8 st1c_donation_find(void)
{
    sint32 i;

    for (i = 0; i < 8; i++)
    {
        if (Sce_flag_test(i + 0x54B))
        {
            return i;
        }
    }
    return 0xFF;
}

// Routine 0: sets the curator's model up.
void st1c_curator_init(ST1C_OBJ *wp)
{
    wp->flags_0 |= 2;
    Model_work_init(wp, NULL);
    wp->value_64               = 0;
    wp->rotation_prev.v.x      = 0;
    wp->rotation.v.x           = 0;
    wp->rotation.words[1]      = 0;
    wp->rotation_prev.words[1] = 0;
    wp->velocity.words[0]      = 0;
    wp->velocity.words[1]      = 0;
    wp->value_64               = 0;
    wp->value_70               = 0;
    wp->value_74               = 0;
    wp->scale.words[0]         = 0x02000200;
    wp->scale.words[1]         = 0x42100200;
    wp->rotation_prev.v        = wp->rotation.v;
    wp->position_prev          = wp->position;
    wp->position_fixed.x = wp->position_fixed_prev.x = wp->position.x << 16;
    wp->position_fixed.z = wp->position_fixed_prev.z = wp->position.z << 16;
    wp->position_fixed.y = wp->position_fixed_prev.y = wp->position.y << 16;
    EM_Char_Set(wp, 0, 0);
    wp->data_78 = NULL;
    wp->data_10 = D_80104460;
    wp->routine_0++;
}

// State 0: waits to be talked to; greets the first time (scene flag 0x40), then takes items.
void st1c_curator_wait(ST1C_OBJ *wp)
{
    if (Player_work.target_184 == wp)
    {
        Player_work.flags_bd |= 0x40;
        if (Sce_flag_test(0x40) == 0)
        {
            wp->routine_1++;
        }
        else
        {
            wp->routine_1 = 2;
        }
    }
    Solid_set(wp, 1, -1, 0);
}

// State 1: camera on the curator, message 1, sets scene flag 0x40.
void st1c_curator_greet(ST1C_OBJ *wp)
{
    STCOM_FIXED_VECTOR pos;
    STCOM_PL_WORK *pl;
    uint8 done;

    pl   = &Player_work;
    done = 0;
    switch (wp->routine_2)
    {
        case 0:
            pos.x = wp->position.x;
            pos.y = wp->position.y - 0x92;
            pos.z = wp->position.z;
            Camera_move_set_pause(&pos, 0xD4, 0, 0);
            pl->flags_0 &= ~2;
            wp->routine_2++;
            break;
        case 1:
            if (Camera_moving() == 0)
            {
                Message_event_set(&wp->position, 1, 0, 1);
                EM_Char_Set(wp, 8, 0);
                done = 1;
                wp->routine_2++;
            }
            break;
        case 2:
            if (Message_event[0] == 0)
            {
                Camera_return_resume();
                pl->flags_0 |= 2;
                wp->routine_2++;
            }
            break;
        case 3:
            if (Camera_moving() == 0)
            {
                Player_work.flags_bd &= ~0x40;
                EM_Char_Set(wp, 0, 0);
                Sce_flag_on(0x40);
                done          = 1;
                wp->routine_2 = 0;
                wp->routine_1--;
            }
            break;
    }
    if (done == 0)
    {
        EM_Char_Move(wp);
    }
}

// State 2: takes the first donation item the player holds (message 3 + item) and puts it on
// show (flag 0x54B + item off, 0x38 + item on); message 2 when there is none.
void st1c_curator_take(ST1C_OBJ *wp)
{
    STCOM_FIXED_VECTOR pos;
    sint32 slot;
    STCOM_PL_WORK *pl;
    uint8 done;

    pl   = &Player_work;
    done = 0;
    switch (wp->routine_2)
    {
        case 0:
            pos.x = wp->position.x;
            pos.y = wp->position.y - 0x92;
            pos.z = wp->position.z;
            Camera_move_set_pause(&pos, 0xD4, 0, 0);
            pl->flags_0 &= ~2;
            wp->routine_2++;
            break;
        case 1:
            if (Camera_moving() == 0)
            {
                if (st1c_donation_find() == 0xFF)
                {
                    wp->timer_C   = 1;
                    wp->routine_2 = 3;
                }
                else
                {
                    EM_Char_Set(wp, 0xB, 0);
                    wp->timer_C = 0x1E;
                    wp->routine_2++;
                }
            }
            break;
        case 2:
            if (--wp->timer_C == 0)
            {
                wp->timer_C = 0xF;
                EM_Char_Set(wp, 0xC, 0);
                wp->routine_2++;
            }
            break;
        case 3:
            if (--wp->timer_C == 0)
            {
                EM_Char_Set(wp, 8, 0);
                slot = st1c_donation_find();
                if (slot == 0xFF)
                {
                    Message_event_set(&wp->position, 1, 0, 2);
                }
                else
                {
                    Message_event_set(&wp->position, 1, 0, slot + 3);
                }
                wp->routine_2++;
            }
            break;
        case 4:
            if (Message_event[0] == 0)
            {
                Camera_return_resume();
                slot = st1c_donation_find();
                if (slot != 0xFF && *(uint8 *)&Moji_flag == 0)
                {
                    Sce_flag_on(slot + 0x38);
                    Sce_flag_off(slot + 0x54B);
                    Sce_light_add(0x19);
                }
                pl->flags_0 |= 2;
                wp->routine_2++;
            }
            break;
        case 5:
            if (Camera_moving() == 0)
            {
                pl->flags_bd &= ~0x40;
                EM_Char_Set(wp, 0, 0);
                Sce_flag_on(0x40);
                done          = 1;
                wp->routine_1 = 0;
                wp->routine_2 = 0;
            }
            break;
    }
    if (done == 0)
    {
        EM_Char_Move(wp);
    }
}

// Routine 1: runs the curator's state (st1c_curator_tbl).
void st1c_curator_main(STAGE_OBJ *wp)
{
    st1c_curator_tbl[wp->routine_1](wp);
}

// Routine 2: closes the curator.
void st1c_curator_erase(STAGE_OBJ *wp)
{
    Close_EM_WORK(wp);
}

// The curator (EM_WORK): runs its routine and casts its shadow.
void st1c_curator_move(STAGE_OBJ *wp)
{
    st1c_curator_move_tbl[wp->routine_0](wp);
    if ((uint8)wp->flags_0 & 2)
    {
        Model_Shadow_Trans(wp, 0x3C, 0x200, 0);
    }
}

// An exhibit (WORK_50 kind 0x22, index_C = item): its model at st1c_exhibit_pos_tbl; shows
// message 0x3C + item when the player looks at it.
void st1c_exhibit_move(ST1C_OBJ50 *wp)
{
    STCOM_FIXED_VECTOR scale;
    STCOM_SVECTOR rot;
    STCOM_PL_WORK *pl;

    pl = &Player_work;
    switch (wp->routine_0)
    {
        case 0:
            wp->flags_0 |= 6;
            wp->variant_3 = st1c_exhibit_model_tbl[wp->index_C];
            Model_work_init(wp, &wp->variant_3);
            if (wp->index_C == 5)
            {
                wp->color_40 = 0x2808080;
            }
            else
            {
                wp->color_40 = 0x808080;
            }
            wp->pos_38.x = st1c_exhibit_pos_tbl[wp->index_C].x;
            wp->pos_38.y = st1c_exhibit_pos_tbl[wp->index_C].y;
            wp->pos_38.z = st1c_exhibit_pos_tbl[wp->index_C].z;
            wp->model_10 = D_801044CC;
            wp->routine_0++;
            // fall through
        case 1:
            rot.x = 0;
            rot.y = st1c_exhibit_pos_tbl[wp->index_C].pad;
            rot.z = 0;
            RotMatrixYXZ(&rot, &wp->matrix);
            scale.x = 0x200;
            scale.y = 0x200;
            scale.z = 0x200;
            ScaleMatrix(&wp->matrix, &scale);
            if (pl->target_184 == wp)
            {
                Message_event_set(&wp->pos_38, 0, 0, wp->index_C + 0x3C);
            }
            break;
    }
}
