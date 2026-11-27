#include "common.h"
#include "st17.h"

void func_8010F600(STAGE_OBJ *wp, ST17_RIG2 *rig)
{
    STAGE_MATRIX m;
    STCOM_FIXED_VECTOR scale = {0x1000, 0x1000, 0x1000, 0};
    sint8 i;

    STAGE_MATRIX_IDENTITY(m.w);
    RotMatrixYXZ(&wp->rotation, &m.m);
    ScaleMatrix(&m.m, &scale);
    for (i = 0; i < 18; i++)
    {
        ApplyMatrixSV(&m.m, &D_8011BEE8[i], &rig->pos_8[i]);
        rig->pos_8[i].x += wp->position.x;
        rig->pos_8[i].y += wp->position.y;
        rig->pos_8[i].z += wp->position.z;
    }
}

void func_8010F724(STAGE_OBJ *wp)
{
    D_8011BF78[wp->routine_1](wp);
    wp->position_prev       = wp->position;
    wp->position_fixed_prev = wp->position_fixed.v;
    wp->rotation_prev       = wp->rotation;
    EM_Char_Move2(wp);
    EM_World_Pos_Calc_YXZ(wp);
}

void func_8010F7DC(STAGE_OBJ *wp)
{
    STCOM_SVECTOR pos[3] = {{-14000, -9000, 4000, 0}, {0, -8000, 0, 0}, {-4000, -8000, 0, 0}};
    sint32 i;

    wp->flags_0 |= 2;
    i = 0;
    switch (STAGE_SCENE_BYTE(8))
    {
        case 0:
            wp->rotation.y   = -0x30;
            wp->rotation.z   = 0x40;
            wp->u_C.b.byte_C = 0;
            break;
        case 9:
            i = 1;
            break;
        case 10:
            i              = 2;
            wp->rotation.z = 0x30;
            break;
    }
    wp->position.x = pos[i].x;
    wp->position.y = pos[i].y;
    wp->position.z = pos[i].z;
    wp->routine_1  = i + 1;
    Model_work_init2(wp, (sint32)&wp->variant_3);
    wp->scale.words[0] = 0x0C000C00;
    wp->scale.words[1] = 0xC00;
    wp->scale.v.pad    = 0x4210;
    EM_Char_Set(wp, 0, 0);
}

void func_8010F944(STAGE_OBJ *wp)
{
    if (wp->u_C.b.byte_C == 1)
    {
        wp->position.x += 0x6E - ((Scene_work.value_10 - 0x186) / 20) * 0xF;
    }
    else if (wp->u_C.b.byte_C == 2)
    {
        wp->position.x += 8;
        wp->rotation.z -= 1;
    }
}

void func_8010F9C8(STAGE_OBJ *wp)
{
    if (Scene_work.value_10 < 0x30)
    {
        wp->rotation.z += 2;
        wp->position.x += 5;
    }
    else
    {
        wp->position.x += 0x18;
    }
}

void func_8010FA14(STAGE_OBJ *wp)
{
    if (Scene_work.value_10 < 0x64)
    {
        wp->position.x += 0x28;
    }
    else if (Scene_work.value_10 < 0x8C)
    {
        wp->rotation.z -= 2;
        wp->position.x += 0xC;
    }
    else
    {
        wp->position.y += D_800BBCF8[ST17_FRAME & 0x3F] >> 11;
    }
}

void func_8010FAA0(STAGE_OBJ *wp)
{
    if (wp->routine_2 == 0)
    {
        wp->rotation.z = 0;
        wp->position.x = 0;
        wp->position.y = -0xBB8;
        wp->position.z = 0;
        wp->routine_2++;
    }
    wp->position.z -= 0x30;
}
