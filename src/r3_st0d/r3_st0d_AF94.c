#include "common.h"
#include "st0d.h"

void func_8010AF94(STAGE_OBJ *wp)
{
    STAGE_OBJ *owner;

    owner = Obj_id_work(wp->u_C.b.byte_C);
    if (STAGE_GAME.value_50 != 0xD || owner->u_C.b.byte_C != 0)
    {
        D_801124C0[wp->variant_3](wp);
    }
}

void func_8010B010(STAGE_OBJ *wp)
{
    D_801124CC[wp->routine_0](wp);
}

void func_8010B04C(ST0D_PAIRS *wp)
{
    uint8 *pair;
    uint32 i;

    wp->routine_0 = 1;
    wp->routine_1 = 0;
    wp->routine_2 = 0;
    wp->routine_3 = 0;
    wp->value_F   = 0;
    for (i = 0, pair = wp->pair_28[0]; i < 16; i++)
    {
        pair[0] = 0;
        pair[1] = 0;
        pair += 2;
    }
}

void func_8010B08C(ST0D_FLAMES *wp)
{
    STAGE_OBJ *owner;
    ST0D_FLAME *flame;
    uint32 dist;
    uint32 i;

    flame = wp->flame_28;
    owner = Obj_id_work(wp->value_C);
    if (wp->timer_D == 0)
    {
        if (!(owner->flags_0 & 1))
        {
            wp->routine_0++;
            return;
        }
    }
    else if (wp->timer_D == 1)
    {
        if (owner->value_44.half[0] == 0)
        {
            wp->routine_0++;
            return;
        }
    }
    if (wp->count_F < 0x40)
    {
        wp->count_F++;
    }
    wp->x_1C = owner->position.x;
    wp->y_1E = owner->position.y;
    wp->z_20 = owner->position.z;
    dist     = D_801124A8[wp->timer_D];
    wp->x_1C += (rsin((owner->rotation.y + 0x800) & 0xFFF) * dist) >> 12;
    wp->z_20 += (rcos((owner->rotation.y + 0x800) & 0xFFF) * dist) >> 12;
    wp->y_1E -= D_801124B8[wp->timer_D];
    for (i = 0; i < 6; i++)
    {
        if (flame[i].flags_0 & 1)
        {
            func_8010B29C(wp, (owner->rotation.y + 0x800) & 0xFFF, flame[i].frame_1);
            if (++flame[i].frame_1 >= 25)
            {
                flame[i].frame_1 = 0;
            }
        }
        else if (i < (wp->count_F >> 2))
        {
            flame[i].flags_0 |= 1;
        }
    }
}

void func_8010B27C(STAGE_OBJ *wp)
{
    Close_EFFECT_WORK(wp);
}

// len is a sint16: as a sint32 it swaps s1/s2 with frame's extension. The position words are
// read through casts: scalar stack loads may move above the packet's struct stores, as in retail.
// The local copy of frame orders the spill slots (rot at 0x18, frame at 0x20), and the (sint16)
// casts stop fold-const reassociating the constants. The same code as st0b's func_80115780.
void func_8010B29C(ST0D_FLAMES *wp, sint16 angle, sint8 arg)
{
    STCOM_SVECTOR pos;
    STCOM23_PACKET *p;
    sint16 rot;
    sint32 bob;
    sint16 len;
    uint8 r;
    uint8 g;
    uint8 b;
    uint8 u;
    uint8 v;
    sint8 frame;

    frame = arg;
    rot   = (angle + (sint16)(frame * 16 + 0x400)) & 0xFFF;
    b     = 0xF0;
    bob   = (frame << 7) & 0xFFF;
    g     = 0xA0;
    r     = 0xA0;
    len   = frame * 15;
    u     = 0x80;
    pos.x = wp->x_1C + ((rsin(rot) * len) >> 12);
    pos.z = wp->z_20 + ((rcos(rot) * len) >> 12);
    v     = 0x40;
    if (frame <= 16)
    {
        pos.y = wp->y_1E - ((rsin(bob) * 40) >> 12);
    }
    else
    {
        pos.y = wp->y_1E;
    }
    p          = Billboard_alloc();
    p->code_4  = 0x2E000000 | (b << 16) | (g << 8) | r;
    p->uv_C    = 0x3D480000 | (v << 8) | u;
    p->uv_14   = 0xE0000 | (v << 8) | (u + 0x1F);
    p->uv_1C   = ((v + 0x1F) << 8) | u;
    p->xy_8    = *(uint32 *)&pos.x;
    p->z_10    = *(uint32 *)&pos.z;
    p->uv_24   = ((v + 0x1F) << 8) | (u + 0x1F);
    p->size_18 = D_801124B0[wp->timer_D] - frame * 2;
    rot        = ((sint16)(rot - 0x800) - (frame << 5)) & 0xFFF;
    p->pad_20  = 0;
    pos.x      = wp->x_1C + ((rsin(rot) * len) >> 12);
    pos.z      = wp->z_20 + ((rcos(rot) * len) >> 12);
    p          = Billboard_alloc();
    p->code_4  = 0x2E000000 | (b << 16) | (g << 8) | r;
    p->uv_C    = 0x3D480000 | (v << 8) | u;
    p->uv_14   = 0xE0000 | (v << 8) | (u + 0x1F);
    p->uv_1C   = ((v + 0x1F) << 8) | u;
    p->xy_8    = *(uint32 *)&pos.x;
    p->z_10    = *(uint32 *)&pos.z;
    p->uv_24   = ((v + 0x1F) << 8) | (u + 0x1F);
    p->size_18 = D_801124B0[wp->timer_D] - frame * 2;
    p->pad_20  = 0;
}

void func_8010B508(STAGE_OBJ *wp)
{
    D_801124D8[wp->routine_0](wp);
}

void func_8010B544(ST0D_FLAMES *wp)
{
    STAGE_OBJ *owner;
    ST0D_FLAME *flame;
    uint32 i;

    flame = wp->flame_28;
    owner = Obj_id_work(wp->value_C);
    if (!(owner->flags_0 & 1))
    {
        wp->routine_0++;
        return;
    }
    if (wp->count_F < 0x40)
    {
        wp->count_F++;
    }
    wp->x_1C = owner->position.x;
    wp->y_1E = owner->position.y;
    wp->z_20 = owner->position.z;
    wp->x_1C += (rsin(owner->rotation.y & 0xFFF) * 500) >> 12;
    wp->z_20 += (rcos(owner->rotation.y & 0xFFF) * 500) >> 12;
    wp->y_1E -= 0x60;
    for (i = 0; i < 6; i++)
    {
        if (flame[i].flags_0 & 1)
        {
            func_8010B6E8(wp, owner->rotation.y & 0xFFF, flame[i].frame_1);
            if (++flame[i].frame_1 >= 13)
            {
                flame[i].frame_1 = 0;
            }
        }
        else if (i < (wp->count_F >> 1))
        {
            flame[i].flags_0 |= 1;
        }
    }
}

// Scalar reads of the position words let them move above the packet stores, as in retail.
void func_8010B6E8(ST0D_FLAMES *wp, sint32 angle, sint8 frame)
{
    STCOM_SVECTOR pos;
    STCOM23_PACKET *p;
    sint32 v;
    sint32 y;
    sint16 side;

    v     = rsin((sint16)angle) * 30;
    pos.x = wp->x_1C + ((v * frame) >> 12);
    v     = rcos((sint16)angle) * 30;
    side  = (angle + 0x400) & 0xFFF;
    pos.z = wp->z_20 + ((v * frame) >> 12);
    y     = frame * 4;
    pos.y = wp->y_1E + y;
    pos.x += (rsin(side) * 100) >> 12;
    pos.z += (rcos(side) * 100) >> 12;
    p          = Billboard_alloc();
    p->uv_1C   = 0x5F80;
    p->code_4  = 0x2EF0A0A0;
    p->uv_C    = 0x3D484080;
    p->uv_14   = 0xE409F;
    p->uv_24   = 0x5F9F;
    p->size_18 = 0x80 - y;
    p->pad_20  = 0;
    p->xy_8    = *(uint32 *)&pos.x;
    p->z_10    = *(uint32 *)&pos.z;
    v          = rsin((sint16)angle) * 30;
    pos.x      = wp->x_1C + ((v * frame) >> 12);
    v          = rcos((sint16)angle) * 30;
    side       = (side - 0x800) & 0xFFF;
    pos.z      = wp->z_20 + ((v * frame) >> 12);
    pos.x += (rsin(side) * 100) >> 12;
    pos.z += (rcos(side) * 100) >> 12;
    p          = Billboard_alloc();
    p->uv_1C   = 0x5F80;
    p->code_4  = 0x2EF0A0A0;
    p->uv_C    = 0x3D484080;
    p->uv_14   = 0xE409F;
    p->uv_24   = 0x5F9F;
    p->size_18 = 0x80 - y;
    p->pad_20  = 0;
    p->xy_8    = *(uint32 *)&pos.x;
    p->z_10    = *(uint32 *)&pos.z;
}

void func_8010B97C(STAGE_OBJ *wp)
{
    D_801124E4[wp->routine_0](wp);
}

void func_8010B9B8(ST0D_FLAMES *wp)
{
    STAGE_OBJ *owner;
    ST0D_FLAME *flame;
    uint32 i;
    flame = wp->flame_28;
    owner = Obj_id_work(wp->value_C);
    if (owner->value_44.half[0] == 0)
    {
        wp->routine_0++;
        return;
    }
    if (wp->count_F < 0x40)
    {
        wp->count_F++;
    }
    wp->x_1C = owner->position.x;
    wp->y_1E = owner->position.y;
    wp->z_20 = owner->position.z;
    wp->x_1C += (rsin(owner->rotation.y & 0xFFF) * 0x140) >> 12;
    wp->z_20 += (rcos(owner->rotation.y & 0xFFF) * 0x140) >> 12;
    wp->y_1E -= 0x10;
    for (i = 0; i < 4; i++)
    {
        if (flame[i].flags_0 & 1)
        {
            func_8010BB48(wp, owner->rotation.y & 0xFFF, flame[i].frame_1);
            if (++flame[i].frame_1 >= 9)
            {
                flame[i].frame_1 = 0;
            }
        }
        else if (i < (wp->count_F >> 1))
        {
            flame[i].flags_0 |= 1;
        }
    }
}

void func_8010BB48(ST0D_FLAMES *wp, sint16 angle, sint8 frame)
{
    STCOM_SVECTOR_W pos;
    STCOM23_PACKET *p;
    sint32 v;
    uint32 xy;
    uint32 z;

    v          = rsin(angle) * 30;
    pos.v.x    = wp->x_1C + ((v * frame) >> 12);
    v          = rcos(angle) * 30;
    pos.v.z    = wp->z_20 + ((v * frame) >> 12);
    v          = frame * 4;
    pos.v.y    = wp->y_1E + v;
    p          = Billboard_alloc();
    p->code_4  = 0x2EF0A0A0;
    p->uv_C    = 0x3D484080;
    p->uv_14   = 0xE409F;
    p->uv_1C   = 0x5F80;
    p->uv_24   = 0x5F9F;
    xy         = pos.words[0];
    z          = pos.words[1];
    p->size_18 = 0x40 - v;
    p->pad_20  = 0;
    p->xy_8    = xy;
    p->z_10    = z;
}
