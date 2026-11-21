#include "common.h"
#include "cd.h"
#include "rock_neo.exe/moji.h"
#include "rock_neo.exe/player.h"
#include "rock_neo.exe/pad.h"

#ifdef __wasm32__
void Cd_read_comb(sint32 id);
#endif

// Small data reached through $gp
uint32 Moji_flag;
uint32 Moji_flag3;
uint32 D_80098960;
sint32 D_80098B2C;
uint8 D_800988F4;
uint8 Moji_list_top;
uint8 Moji_item;
uint8 D_80098850;
uint8 D_80098851;
uint8 D_800987F8;
sint32 Moji_list_count;
uint8 D_80098930;
uint8 D_80098B6C;
sint32 Moji_amount;
uint8 D_800989D4;
uint32 Moji_time;
sint32 D_800986F0;
uint8 D_80098A64;
uint8 D_800986E0;
uint8 D_800986E4;
uint8 D_800986E8;
uint8 D_800986EC;
uint32 D_800987E0;

void Moji_init(void)
{
    sint16 i;
    MOJI_WORK *wp;

    Cd_read_comb(0x1E);
    Cd_read_sync();
    for (i = 0; i < 5; i++)
    {
        wp                   = &Moji_work[i];
        Moji_work[i].flags_0 = 0;
        wp->pos_6C           = 0;
        Moji_work[i].index_6 = i;
        Moji_work[i].pos_48  = 0;
    }
    D_80098B2C = 0;
    Moji_flag  = 0;
    D_80098960 = 0;
}

sint32 MojiTaskExec(sint32 id, uint8 *script, sint32 index)
{
    MOJI_WORK *wp;
    uint8 idx;

    idx = index;
    if ((Moji_flag & 0x400000) || D_80098824 != 0)
    {
        return 0;
    }
    wp = &Moji_work[id];
    if (wp->flags_0 & 0x40000)
    {
        func_8001D494(0, 1, 0);
    }
    if (id != 4)
    {
        if (Moji_work[4].flags_0 & 0x40000)
        {
            func_8001D494(0, 1, 0);
        }
        Moji_work[4].flags_0 = 0;
        Moji_work[4].val_C2  = 0xFF;
    }
    wp->flags_0 = 0x80000000;
    wp->val_3F  = 1;
    wp->val_3E  = 0;
    wp->val_C2  = idx;
    wp->val_4   = 0;
    wp->val_8   = 0;
    wp->val_A   = 0;
    wp->val_C   = 0;
    wp->val_E   = 0;
    wp->val_3C  = wp->val_3E;
    if (idx != 0xFF)
    {
        wp->pos_6C = wp->pos_48 = script + ((uint16 *)script)[idx];
        wp->base_44             = script;
    }
    else
    {
        wp->pos_6C = wp->pos_48 = script;
        wp->base_44             = 0;
    }
    wp->val_3A     = 2;
    wp->val_3D     = 3;
    wp->val_71     = 0;
    wp->v_72.count = 0;
    wp->val_73     = 0;
    wp->val_7D     = 0;
    wp->val_7E     = 0;
    wp->val_7F     = 0;
    wp->count_3B   = 0;
    wp->val_78     = 0x80;
    wp->val_70     = wp->val_3E;
    wp->depth_C0   = 0;
    wp->depth_BE   = 0;
    wp->val_38     = 0;
    wp->val_BC     = 0;
    wp->pos_14     = wp->pos_6C;
    Moji_flag |= 0x80000000 | (0x08000000 >> id);
    return 1;
}

void MojiTaskExec2(sint32 arg0, uint8 arg1)
{
    MojiTaskExec(arg0, (uint8 *)0x80153000, arg1);
}

void MojiTaskKill(void)
{
    MOJI_WORK *wp;
    uint32 flags;

    flags = 0;
    for (wp = Moji_work; wp < &Moji_work[5]; wp++)
    {
        flags |= wp->flags_0;
        wp->flags_0 = 0;
        wp->val_C2  = 0xFF;
    }
    if (flags & 0x40000)
    {
        func_8001D494(0, 1, 0);
    }
    Moji_flag = 0;
}

void Moji_task_kill(uint8 index)
{
    MOJI_WORK *wp;

    wp = &Moji_work[index];
    if (wp->flags_0 & 0x40000)
    {
        func_8001D494(0, 1, 0);
    }
    wp->val_C2  = 0xFF;
    wp->flags_0 = 0;
    Moji_flag &= 0xBFC1FFFF & ~(0x08000000 >> index);
}

// The text-window renderer. The two area allocations read the packet pointer with a
// post-increment (retail's move to s1); after the clip rect the second no longer matches the
// first one's 0x1F800070 constant in loop.c, so every use rebuilds it with lui/ori as retail
// does. The pad words are Joy1.held/pressed: struct fields keep their loads after the val_B8
// store. The slot test is `flags_0 & 0x80000000`: combine turns it into bgez, but the constant
// stays loaded for the Moji_flag update (dbr puts it in bgez's slot). `wp` is set before the
// Moji_flag mask so its address load splits 0x3043FFFF's lui/ori.
void Moji_update(void)
{
    MOJI_WORK *wp;
    DR_AREA *area;
    SPRT *sprt;
    DR_MODE *mode;
    RECT rect;

    if (!(Moji_flag & 0x400000) && D_80098824 == 0)
    {
        wp = Moji_work;
        Moji_flag &= 0x3043FFFF;
        D_80098B2C = 0;
        for (; wp < &Moji_work[5]; wp++)
        {
            if (wp->flags_0 & 0x80000000)
            {
                Moji_flag |= 0x80000000 | (0x08000000 >> (wp - Moji_work));
                if (!(Moji_flag & 0x40000) && !(Moji_flag & (0x10000 >> wp->index_6)))
                {
                    wp->val_B8 = Joy1.held;
                    wp->val_BA = Joy1.pressed;
                }
                else
                {
                    wp->val_B8 = 0;
                    wp->val_BA = 0;
                }
                if (!(wp->flags_0 & 0x08000000) && (wp->val_B8 & 0x1000) &&
                    (wp->flags_0 & 0x40000) && Sce_cd_reading() != 0)
                {
                    wp->flags_0 &= ~0x40000;
                    func_8001D494(0, 1, 0);
                }
                while (1)
                {
                    if (*wp->pos_6C < 0x84)
                    {
                        if (--wp->val_4 <= 0)
                        {
                            if (!(wp->flags_0 & 0x08000000) && (wp->val_B8 & 0x1000) &&
                                !(wp->flags_0 & 0x40000))
                            {
                                wp->val_4 = wp->val_3F >> 1;
                            }
                            else
                            {
                                wp->val_4 = wp->val_3F;
                            }
                            wp->pos_6C++;
                            if (!(wp->flags_0 & 0x60000000))
                            {
                                if (!(wp->flags_0 & 0x40000))
                                {
                                    Sound_call(0x84, 0, 0x64);
                                }
                                wp->flags_0 |= 0x40000000;
                            }
                        }
                        if (wp->val_4 == 0)
                        {
                            wp->flags_0 |= 0x40000000;
                            continue;
                        }
                    }
                    else if (D_8008A91C[*wp->pos_6C](wp) != 0)
                    {
                        continue;
                    }
                    if (wp->val_3F != 0 || (wp->flags_0 & 0x10000000))
                    {
                        break;
                    }
                }
                area = (*(DR_AREA **)0x1F800070)++;
                SetDrawArea(area, &D_80097F50[*(uint8 *)0x1F800000]);
                addPrim(&D_80098934->ot[(sint8)wp->val_3D], area);
                wp->val_73   = 0;
                wp->val_78   = 0x80;
                wp->pos_14   = wp->pos_48;
                wp->depth_BE = 0;
                wp->pos_10   = wp->val_8;
                wp->pos_12   = wp->val_A + wp->val_38;
                wp->val_3E   = wp->val_3C;
                while (wp->pos_14 != wp->pos_6C)
                {
                    if (*wp->pos_14 < 0x84)
                    {
                        if (!(wp->flags_0 & 0x100000))
                        {
                            sprt                 = *(SPRT **)0x1F800070;
                            *(SPRT **)0x1F800070 = sprt + 1;
                            setSprt(sprt);
                            sprt->r0   = wp->val_78;
                            sprt->g0   = wp->val_78;
                            sprt->b0   = wp->val_78;
                            sprt->x0   = wp->pos_10;
                            sprt->y0   = wp->pos_12;
                            sprt->w    = 12;
                            sprt->h    = 12;
                            sprt->clut = D_80097F30[wp->val_3E];
                            sprt->u0   = (*wp->pos_14 % 21) * 12;
                            sprt->v0   = (*wp->pos_14 / 21) * 12;
                            addPrim(&D_80098934->ot[(sint8)wp->val_3D], sprt);
                            wp->pos_10 += D_8008AE7C[*wp->pos_14];
                        }
                        wp->pos_14++;
                    }
                    else
                    {
                        D_8008AAC4[*wp->pos_14](wp);
                    }
                }
                rect.x = wp->val_8;
                rect.y = wp->val_A + (*(uint8 *)0x1F800000 << 8);
                rect.w = wp->val_7E * 12;
                rect.h = wp->val_7F * 12;
                area   = (*(DR_AREA **)0x1F800070)++;
                SetDrawArea(area, &rect);
                addPrim(&D_80098934->ot[(sint8)wp->val_3D], area);
                if (wp->flags_0 & 0x04000000)
                {
                    D_8008AAE4[wp->val_7C](wp);
                }
            }
            else
            {
                Moji_flag &= ~(0x10000 >> wp->index_6);
            }
            if (wp != &Moji_work[4])
            {
                D_80098B2C |= (sint8)wp->val_71 << ((wp - Moji_work) * 8);
            }
            wp->flags_0 &= 0xAF7FFFFF;
        }
        mode                    = *(DR_MODE **)0x1F800070;
        *(DR_MODE **)0x1F800070 = mode + 1;
        SetDrawMode(mode, 0, 0, GetTPage(0, 0, 0x3C0, 0x100), NULL);
        addPrim(&D_80098934->ot[3], mode);
        D_80098960++;
    }
}

void Moji_text_skip1(MOJI_WORK *wp)
{
    wp->pos_14 += 1;
}

void Moji_text_skip2(MOJI_WORK *wp)
{
    wp->pos_14 += 2;
}

void Moji_text_skip3(MOJI_WORK *wp)
{
    wp->pos_14 += 3;
}

void Moji_text_skip4(MOJI_WORK *wp)
{
    wp->pos_14 += 4;
}

void Moji_text_skip5(MOJI_WORK *wp)
{
    wp->pos_14 += 5;
}

void Moji_text_skip6(MOJI_WORK *wp)
{
    wp->pos_14 += 6;
}

void Moji_text_skip7(MOJI_WORK *wp)
{
    wp->pos_14 += 7;
}

void Moji_text_skip9(MOJI_WORK *wp)
{
    wp->pos_14 += 9;
}

void Moji_text_skip11(MOJI_WORK *wp)
{
    wp->pos_14 += 11;
}

sint32 Moji_cmd_skip1(MOJI_WORK2 *wp)
{
    wp->pos_6C += 1;
    return 1;
}

sint32 Moji_cmd_skip2(MOJI_WORK2 *wp)
{
    wp->pos_6C += 2;
    return 1;
}

sint32 Moji_cmd_skip3(MOJI_WORK2 *wp)
{
    wp->pos_6C += 3;
    return 1;
}

sint32 Moji_cmd_skip5(MOJI_WORK2 *wp)
{
    wp->pos_6C += 5;
    return 1;
}

uint16 get_le16(uint8 *p)
{
    return p[0] | (p[1] << 8);
}

uint32 get_le32(uint8 *p)
{
    return p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24);
}

sint32 Moji_cmd_end_wait(MOJI_WORK *wp)
{
    if (!(wp->flags_0 & 0x1000000))
    {
        wp->flags_0 |= 0x1000000;
        wp->val_4 = get_le16(wp->pos_6C + 1);
    }
    wp->flags_0 |= 0x10000000;
    if (wp->val_4 <= 0)
    {
        wp->flags_0 |= 0x20000;
        if (wp->flags_0 & 0x4000000)
        {
            wp->flags_0 |= 0x100000;
            D_8008AAC0[wp->val_7C](wp);
            return 0;
        }
        wp->flags_0 &= ~0x20000;
        wp->flags_0 &= ~0x81000000;
        wp->pos_6C += 3;
        if (wp->flags_0 & 0x40000)
        {
            func_8001D494(0, 1, 0);
        }
        wp->val_C2 = 0xFF;
        wp->flags_0 &= ~0x40000;
    }
    else
    {
        wp->val_4 -= 1;
    }
    return 0;
}

sint32 Moji_cmd_newline(MOJI_WORK *wp)
{
    wp->count_3B += 1;
    wp->pos_6C += 1;
    return 1;
}

void Moji_text_newline(MOJI_WORK *wp)
{
    wp->pos_10 = wp->val_8;
    wp->pos_12 += 12;
    wp->val_3E = wp->val_70;
    wp->pos_14 += 1;
}

sint32 Moji_cmd_page_wait(MOJI_WORK *wp)
{
    if (!(wp->flags_0 & 0x1000000))
    {
        wp->flags_0 |= 0x1000000;
        wp->val_4 = get_le16(wp->pos_6C + 1);
    }
    wp->flags_0 |= 0x10000000;
    if (wp->val_4 <= 0)
    {
        wp->flags_0 &= ~0x1000000;
        wp->val_4      = 5;
        wp->val_71     = 0;
        wp->v_72.count = 0;
        wp->val_73     = 0;
        wp->pos_6C += 3;
        wp->pos_48 = wp->pos_6C;
        wp->val_3C = wp->val_3E;
        Moji_flag |= 0x200000;
    }
    else
    {
        wp->val_4 -= 1;
    }
    return 0;
}

void func_80054694(MOJI_WORK *wp)
{
    wp->val_3E = wp->pos_14[1];
    wp->pos_14 += 2;
}

sint32 func_800546B0(MOJI_WORK *wp)
{
    wp->val_3F = get_le16(wp->pos_6C + 1);
    wp->val_4  = wp->val_3F;
    wp->pos_6C += 3;
    return 1;
}

sint32 Moji_cmd_wait(MOJI_WORK *wp)
{
    if (!(wp->flags_0 & 0x1000000))
    {
        wp->flags_0 |= 0x1000000;
        wp->val_4 = get_le16(wp->pos_6C + 1);
    }
    wp->flags_0 |= 0x10000000;
    if (wp->val_4 <= 0)
    {
        wp->flags_0 &= ~0x1000000;
        wp->pos_6C += 3;
    }
    else
    {
        wp->val_4 -= 1;
    }
    return 0;
}

sint32 Moji_cmd_window_rect(MOJI_WORK *wp)
{
    wp->val_8  = get_le16(wp->pos_6C + 1);
    wp->val_A  = get_le16(wp->pos_6C + 3);
    wp->val_7E = wp->pos_6C[5];
    wp->val_7F = wp->pos_6C[6];
    wp->pos_6C += 7;
    return 1;
}

sint32 Moji_cmd_sync(MOJI_WORK *wp)
{
    sint32 flags;

    wp->flags_0 |= 0x10000000;
    flags = Moji_work[wp->pos_6C[1]].flags_0;
    if (flags >= 0 || (flags & 0x20000))
    {
        wp->pos_6C += 2;
        return 1;
    }
    return 0;
}

sint32 Moji_cmd_se(MOJI_WORK *wp)
{
    Sound_call(get_le16(wp->pos_6C + 1), 0, 0);
    wp->pos_6C += 3;
    return 1;
}

sint32 Moji_cmd_voice(MOJI_WORK *wp)
{
    if (!(wp->flags_0 & 0x1000000))
    {
        wp->flags_0 |= 0x1000000;
        func_8001D58C(get_le32(wp->pos_6C + 1));
    }
    else if (Sce_cd_reading() != 0)
    {
        wp->val_4 = 5;
        Moji_flag |= 0x100000;
        wp->flags_0 = (wp->flags_0 | 0x40000) & ~0x1000000;
        wp->pos_6C += 5;
    }
    wp->flags_0 |= 0x10000000;
    return 0;
}

sint32 Moji_cmd_call(MOJI_WORK *wp)
{
    wp->jump_tbl_4C[wp->depth_C0++] = (uint32)(wp->pos_6C + 2);
    if (wp->base_44 != 0)
    {
        wp->pos_6C = wp->base_44 + ((uint16 *)wp->base_44)[wp->pos_6C[1]];
    }
    else
    {
        wp->pos_6C = D_8008CACC[wp->pos_6C[1]];
    }
    return 1;
}

void Moji_text_call(MOJI_WORK *wp)
{
    wp->jump_tbl_18[wp->depth_BE++] = (uint32)(wp->pos_14 + 2);
    if (wp->base_44 != 0)
    {
        wp->pos_14 = wp->base_44 + ((uint16 *)wp->base_44)[wp->pos_14[1]];
    }
    else
    {
        wp->pos_14 = D_8008CACC[wp->pos_14[1]];
    }
}

sint32 Moji_cmd_return(MOJI_WORK *wp)
{
    wp->depth_C0 -= 1;
    wp->pos_6C = (uint8 *)wp->jump_tbl_4C[wp->depth_C0];
    return 1;
}

void Moji_text_return(MOJI_WORK *wp)
{
    wp->depth_BE -= 1;
    wp->pos_14 = (uint8 *)wp->jump_tbl_18[wp->depth_BE];
}

sint32 Moji_cmd_window(MOJI_WORK *wp)
{
    if (wp->flags_0 & 0x2000000)
    {
        wp->flags_0 &= ~0x2000000;
        wp->val_7D = 0;
    }
    wp->val_7C = wp->pos_6C[1];
    D_8008AAC0[wp->val_7C](wp);
    return 0;
}

void Moji_text_window(MOJI_WORK *wp)
{
    D_8008AB08[wp->val_7C](wp);
}

sint32 func_80054B88(MOJI_WORK *wp)
{
    wp->flags_0 |= MOJI_FLAG_08000000;
    wp->pos_6C += 1;
    return 1;
}

sint32 func_80054BAC(void)
{
    return 0;
}

sint32 Moji_cmd_choice(MOJI_WORK *wp)
{
    wp->flags_0 |= 0x400800;
    wp->v_72.count++;
    wp->pos_6C += 3;
    wp->val_BC = 0;
    return 1;
}

void Moji_text_choice(MOJI_WORK *wp)
{
    sint16 old;

    wp->pos_14[3] = 0xED;
    wp->val_78    = 0;
    if ((sint8)wp->val_71 == (sint8)wp->val_73 && !(wp->flags_0 & 0xC00000))
    {
        old = (sint8)wp->val_71;
        if (!(wp->val_BA & 0x4000))
        {
            if (wp->val_BA & 0x10)
            {
                wp->val_71 = wp->pos_14[2] >> 4;
                wp->val_BC = 0;
            }
            else if ((*(uint32 *)&wp->val_B8 & 0x100010) == 0x10)
            {
                if (wp->val_BC >= 13)
                {
                    wp->val_71 = wp->pos_14[2] >> 4;
                    wp->val_BC = 12;
                }
                else
                {
                    wp->val_BC++;
                }
            }
            else if (wp->val_BA & 0x40)
            {
                wp->val_71 = wp->pos_14[2] & 0xF;
                wp->val_BC = 0;
            }
            else if ((*(uint32 *)&wp->val_B8 & 0x400040) == 0x40)
            {
                if (wp->val_BC >= 13)
                {
                    wp->val_71 = wp->pos_14[2] & 0xF;
                    wp->val_BC = 12;
                }
                else
                {
                    wp->val_BC++;
                }
            }
            else if (wp->val_BA & 0x80)
            {
                wp->val_71 = wp->pos_14[1] >> 4;
                wp->val_BC = 0;
            }
            else if ((*(uint32 *)&wp->val_B8 & 0x800080) == 0x80)
            {
                if (wp->val_BC >= 13)
                {
                    wp->val_71 = wp->pos_14[1] >> 4;
                    wp->val_BC = 12;
                }
                else
                {
                    wp->val_BC++;
                }
            }
            else if (wp->val_BA & 0x20)
            {
                wp->val_71 = wp->pos_14[1] & 0xF;
                wp->val_BC = 0;
            }
            else if ((*(uint32 *)&wp->val_B8 & 0x200020) == 0x20)
            {
                if (wp->val_BC >= 13)
                {
                    wp->val_71 = wp->pos_14[1] & 0xF;
                    wp->val_BC = 12;
                }
                else
                {
                    wp->val_BC++;
                }
            }
        }
        wp->pos_14[3] = 0x60;
        wp->val_78 += 0x30;
        wp->pos_74 = wp->pos_14 + 3;
        wp->flags_0 |= 0x800000;
        if (old != (sint8)wp->val_71)
        {
            Sound_call(0x81, 1, 0);
        }
    }
    wp->val_78 += 0x50;
    wp->val_73++;
    wp->pos_14 += 3;
}

sint32 Moji_cmd_select(MOJI_WORK *wp)
{
    uint8 *p;
    uint8 next;

    if (!(wp->flags_0 & 0x01000000))
    {
        wp->flags_0 = (wp->flags_0 | 0x01000000) & ~0x8000;
        wp->val_4   = 5;
        if (wp->pos_6C[1] & 0x40)
        {
            if (wp->pos_6C[1] & 0x80)
            {
                wp->flags_0 |= 0x8000;
            }
            wp->val_71 = wp->val_79;
        }
        else if (wp->pos_6C[1] & 0x20)
        {
            if (wp->pos_6C[1] & 0x80)
            {
                wp->flags_0 |= 0x8000;
            }
            wp->val_71 = wp->unk_7A[0];
        }
        else if (wp->pos_6C[1] & 0x10)
        {
            if (wp->pos_6C[1] & 0x80)
            {
                wp->flags_0 |= 0x8000;
            }
            wp->val_71 = wp->unk_7A[1];
        }
        else
        {
            if (wp->pos_6C[1] & 0x80)
            {
                wp->flags_0 |= 0x8000;
            }
            wp->val_71 = wp->pos_6C[1] & 0x3F;
        }
    }
    wp->flags_0 = (wp->flags_0 | 0x10000000) & ~0x00400000;
    Moji_flag |= 0x40000;
    if (wp->val_4 <= 0)
    {
        if (!(wp->flags_0 & 0x4000) || (sint8)wp->val_71 == wp->v_72.skip - 1 ||
            (wp->val_BA & 0x1000))
        {
            if ((wp->val_BA & 0x4000) || ((wp->val_BA & 0x1000) && (wp->flags_0 & 0x8000)) ||
                ((wp->val_BA & 8) && ((sint8 *)Game_work)[0] == 4))
            {
                if (((wp->val_BA & 0x1000) && (wp->flags_0 & 0x8000)) ||
                    ((wp->val_BA & 8) && ((sint8 *)Game_work)[0] == 4))
                {
                    wp->val_71 = wp->v_72.count - 1;
                    Sound_call(0x83, 1, 0);
                }
                else
                {
                    Sound_call(0x82, 1, 0);
                }
                Moji_flag &= ~0xFF;
                if (wp->pos_6C[0] == 0xEA && (sint8)wp->val_71 != 5 && (sint8)wp->val_71 != 0)
                {
                    switch ((sint8)wp->val_71)
                    {
                        case 1:
                            Sub_screen_sort_attack();
                            break;
                        case 2:
                            Sub_screen_sort_energy();
                            break;
                        case 3:
                            Sub_screen_sort_range();
                            break;
                        case 4:
                            Sub_screen_sort_rapid();
                            break;
                    }
                }
                else
                {
                    if (wp->pos_6C[0] == 0xE8)
                    {
                        Moji_flag |= 0x40000000 | Moji_list[(sint8)wp->val_71];
                    }
                    else
                    {
                        Moji_flag |= 0x40000000 | wp->val_71;
                    }
                    *wp->pos_74 = 0xED;
                    wp->val_4   = 5;
                    wp->flags_0 = (wp->flags_0 | 0x400000) & 0xEEFF97FF;
                    next        = wp->pos_6C[(sint8)wp->val_71 + 2];
                    if (wp->pos_6C[1] & 0x40)
                    {
                        wp->val_79 = wp->val_71;
                        D_800988F4 = Moji_list_top;
                    }
                    if (wp->pos_6C[1] & 0x20)
                    {
                        wp->unk_7A[0] = wp->val_71;
                    }
                    if (wp->pos_6C[1] & 0x10)
                    {
                        wp->unk_7A[1] = wp->val_71;
                    }
                    if (next == 0xFF)
                    {
                        wp->pos_6C += wp->v_72.skip + 2;
                    }
                    else
                    {
                        wp->val_C2 = next;
                        if (wp->base_44 != NULL)
                        {
                            p = wp->base_44 + ((uint16 *)wp->base_44)[next];
                        }
                        else
                        {
                            p = D_8008CACC[next];
                        }
                        wp->pos_48    = p;
                        wp->pos_6C    = p;
                        wp->val_3E    = 0;
                        wp->val_3C    = 0;
                        wp->v_72.skip = 0;
                    }
                    wp->val_78 = 0x80;
                    return 1;
                }
            }
        }
    }
    else
    {
        wp->val_4--;
    }
    wp->val_78 = 0x80;
    return 0;
}

void Moji_text_select(MOJI_WORK *wp)
{
    wp->pos_14 += wp->v_72.skip + 2;
}

sint32 func_8005531C(MOJI_WORK *wp)
{
    wp->flags_0 &= ~MOJI_FLAG_08000000;
    wp->pos_6C += 1;
    return 1;
}

sint32 Moji_cmd_exec(MOJI_WORK *wp)
{
    uint8 *p;
    uint8 id;
    uint8 index;

    p          = wp->pos_6C;
    id         = p[1];
    index      = p[2];
    wp->pos_6C = p + 3;
    if (wp->base_44 != 0)
    {
        MojiTaskExec((sint8)id, wp->base_44, index);
    }
    else
    {
        MojiTaskExec((sint8)id, D_8008CACC[index], 0xFF);
    }
    return 1;
}

sint32 Moji_cmd_flag_on(MOJI_WORK *wp)
{
    Sce_flag_on(get_le16(wp->pos_6C + 1));
    wp->pos_6C += 3;
    return 1;
}

sint32 Moji_cmd_flag_off(MOJI_WORK *wp)
{
    Sce_flag_off(get_le16(wp->pos_6C + 1));
    wp->pos_6C += 3;
    return 1;
}

sint32 Moji_cmd_flag_branch(MOJI_WORK *wp)
{
    sint32 x;

    if (Sce_flag_test(get_le16(wp->pos_6C + 1)))
    {
        x = wp->pos_6C[3];
    }
    else
    {
        x = wp->pos_6C[4];
    }
    if ((uint8)x == 0xFF)
    {
        wp->pos_6C += 5;
    }
    else
    {
        wp->val_C2 = x;
        wp->pos_6C = wp->base_44 + ((uint16 *)wp->base_44)[(uint8)x];
        wp->pos_48 = wp->pos_6C;
    }
    return 1;
}

sint32 Moji_cmd_scroll(MOJI_WORK *wp)
{
    sint32 step;

    if (!(wp->flags_0 & 0x1000000))
    {
        wp->flags_0 |= 0x1000000;
        wp->val_4 = get_le16(wp->pos_6C + 1);
        wp->count_3B += 1;
    }
    wp->flags_0 |= 0x10000000;
    step = (sint8)wp->count_3B * 12;
    wp->val_4 -= 1;
    if (wp->val_4 < 0)
    {
        if (wp->val_4 * wp->val_3A < -step)
        {
            wp->val_71     = 0;
            wp->v_72.count = 0;
            wp->val_73     = 0;
            wp->count_3B   = 0;
            wp->val_38 += step;
            wp->flags_0 &= ~0x1000000;
            wp->pos_6C += 3;
            wp->pos_48 = wp->pos_6C;
            wp->val_3C = wp->val_3E;
        }
        else
        {
            wp->val_38 -= wp->val_3A;
        }
    }
    return 0;
}

void Moji_text_space(MOJI_WORK *wp)
{
    wp->pos_10 += get_le16(wp->pos_14 + 1);
    wp->pos_14 += 3;
}

sint32 func_8005563C(MOJI_WORK *wp)
{
    wp->flags_0 |= MOJI_FLAG_20000000;
    wp->pos_6C += 1;
    return 1;
}

sint32 func_80055660(MOJI_WORK *wp)
{
    wp->flags_0 &= ~MOJI_FLAG_20000000;
    wp->pos_6C += 1;
    return 1;
}

sint32 Moji_cmd_key_wait(MOJI_WORK *wp)
{
    if (!(wp->flags_0 & 0x1000000))
    {
        wp->flags_0 |= 0x1000000;
        wp->val_4 = 10;
    }
    wp->flags_0 |= 0x10000000;
    if (wp->val_4 <= 0)
    {
        if (!(wp->flags_0 & 0x40000) && !(wp->val_B8 & 0xF000))
        {
            if ((D_80098960 & 0xF) < 8)
            {
                D_800987D4 = 0;
                func_8001FE38(((sint16)wp->val_8 + wp->val_7E * 12 - 4) |
                                  (((sint16)wp->val_A + wp->val_7F * 12 - 4) << 9),
                              D_80097F60);
            }
        }
        else
        {
            Moji_flag |= 0x80000;
            wp->flags_0 &= ~0x1000000;
            wp->pos_6C += 1;
        }
    }
    else
    {
        wp->val_4 -= 1;
    }
    return 0;
}

sint32 Moji_cmd_voice_wait(MOJI_WORK *wp)
{
    if (!(wp->flags_0 & 0x1000000))
    {
        wp->flags_0 |= 0x1000000;
        wp->val_4 = get_le16(wp->pos_6C + 1);
    }
    wp->flags_0 |= 0x10000000;
    if (wp->val_4 <= 0 || !(wp->flags_0 & 0x40000))
    {
        wp->val_4 = 0;
        wp->flags_0 &= ~0x11000000;
        wp->pos_6C += 3;
        return 1;
    }
    wp->val_4 -= 1;
    return 0;
}

sint32 Moji_cmd_camera(MOJI_WORK *wp)
{
    sint32 vec[3];
    MOJI_POS pos;
    uint8 *state;
    PL_WORK *pl;

    state = D_800987B0;
    pl    = &Player_work;
    if (!(wp->flags_0 & 0x1000000))
    {
        wp->flags_0 |= 0x1000000;
        pos    = *D_800987B8;
        vec[0] = D_800987B8->x;
        vec[1] = D_800987B8->y + (sint16)get_le16(wp->pos_6C + 9);
        vec[2] = D_800987B8->z;
        if (Scene_work == 0)
        {
            func_80039FAC(vec, get_le16(wp->pos_6C + 3),
                          Calc_degree2(&pos, &pl->position) + get_le16(wp->pos_6C + 5),
                          D_800A396C + get_le16(wp->pos_6C + 7));
        }
        else
        {
            func_80039FFC(vec, get_le16(wp->pos_6C + 3),
                          Calc_degree2(&pos, &pl->position) + get_le16(wp->pos_6C + 5),
                          D_800A396C + get_le16(wp->pos_6C + 7));
        }
        wp->flags_0 |= 0x10000;
        state[1] = get_le16(wp->pos_6C + 1);
    }
    wp->flags_0 |= 0x10000000;
    if (func_8003A13C() == 0)
    {
        wp->flags_0 &= 0xEEFFFFFF;
        wp->pos_6C += 0xB;
        return 1;
    }
    return 0;
}

sint32 Moji_cmd_room_branch(MOJI_WORK *wp)
{
    sint32 x;

    if (((sint8 *)Game_work)[0x50] == wp->pos_6C[1] && ((sint8 *)Game_work)[0x51] == wp->pos_6C[2])
    {
        x = wp->pos_6C[3];
    }
    else
    {
        x = wp->pos_6C[4];
    }
    if ((uint8)x == 0xFF)
    {
        wp->pos_6C += 5;
    }
    else
    {
        wp->val_C2 = x;
        wp->pos_6C = wp->base_44 + ((uint16 *)wp->base_44)[(uint8)x];
        wp->pos_48 = wp->pos_6C;
    }
    return 1;
}

sint32 Moji_cmd_logo(MOJI_WORK *wp)
{
    uint32 id;

    if ((uint16)get_le16(wp->pos_6C + 1) != 0)
    {
        Game_logo_kill((sint16)get_le16(wp->pos_6C + 3));
    }
    else
    {
        id = get_le16(wp->pos_6C + 3);
        func_80063FC0(id, get_le32(wp->pos_6C + 5));
    }
    wp->pos_6C += 9;
    return 1;
}

sint32 Moji_cmd_window_close(MOJI_WORK *wp)
{
    if (wp->flags_0 & 0x4000000)
    {
        wp->flags_0 |= 0x100000;
        D_8008AAC0[wp->val_7C](wp);
        return 0;
    }
    wp->pos_6C += 1;
    return 1;
}


sint32 Moji_cmd_camera_end(MOJI_WORK *wp)
{
    if (wp->flags_0 & 0x10000)
    {
        func_80039E18();
        wp->flags_0 &= ~0x10000;
    }
    wp->pos_6C += 1;
    return 1;
}

sint32 Moji_cmd_option_set(MOJI_WORK *wp)
{
    *D_8008D0D4[wp->pos_6C[1]] = wp->pos_6C[2];
    wp->pos_6C += 3;
    return 1;
}

sint32 Moji_cmd_toggle(MOJI_WORK *wp)
{
    wp->flags_0 |= 0x404000;
    wp->v_72.count++;
    wp->pos_6C += 7;
    wp->val_BC = 0;
    return 1;
}

void Moji_text_toggle(MOJI_WORK *wp)
{
    sint16 old;

    wp->pos_14[7] = 0xED;
    wp->val_78    = 0;
    if ((sint8)wp->val_71 == (sint8)wp->val_73 && !(wp->flags_0 & 0xC00000))
    {
        old = (sint8)wp->val_71;
        if (wp->val_BA & 0x10)
        {
            wp->val_71 = wp->pos_14[2] >> 4;
            wp->val_BC = 0;
        }
        else if ((*(uint32 *)&wp->val_B8 & 0x100010) == 0x10)
        {
            if (wp->val_BC >= 13)
            {
                wp->val_71 = wp->pos_14[2] >> 4;
                wp->val_BC = 12;
            }
            else
            {
                wp->val_BC++;
            }
        }
        else if (wp->val_BA & 0x40)
        {
            wp->val_71 = wp->pos_14[2] & 0xF;
            wp->val_BC = 0;
        }
        else if ((*(uint32 *)&wp->val_B8 & 0x400040) == 0x40)
        {
            if (wp->val_BC >= 13)
            {
                wp->val_71 = wp->pos_14[2] & 0xF;
                wp->val_BC = 12;
            }
            else
            {
                wp->val_BC++;
            }
        }
        else if (wp->val_BA & 0x80)
        {
            wp->val_BC = 0;
            if ((sint8)wp->val_71 != wp->v_72.skip - 1)
            {
                if (*D_8008D0D4[wp->pos_14[3]] == 1)
                {
                    Sound_call(0x82, 1, 0);
                }
                *D_8008D0D4[wp->pos_14[3]] = 0;
            }
        }
        else if (wp->val_BA & 0x20)
        {
            wp->val_BC = 0;
            if ((sint8)wp->val_71 != wp->v_72.skip - 1)
            {
                if (*D_8008D0D4[wp->pos_14[3]] == 0)
                {
                    Sound_call(0x82, 1, 0);
                }
                *D_8008D0D4[wp->pos_14[3]] = 1;
            }
        }
        else if (wp->val_BA & 0x4000)
        {
            *D_8008D0D4[wp->pos_14[3]] ^= 1;
            if ((sint8)wp->val_71 != wp->v_72.skip - 1)
            {
                Sound_call(0x82, 1, 0);
                wp->val_BA &= ~0x4000;
            }
        }
        if (wp->base_44 != NULL)
        {
            MojiTaskExec(wp->pos_14[4], wp->base_44, wp->pos_14[*D_8008D0D4[wp->pos_14[3]] + 5]);
        }
        else
        {
            MojiTaskExec(wp->pos_14[4], D_8008CACC[wp->pos_14[*D_8008D0D4[wp->pos_14[3]] + 5]],
                         0xFF);
        }
        wp->pos_14[7] = 0x60;
        wp->pos_74    = wp->pos_14 + 7;
        wp->val_78 += 0; // retail reloads and stores val_78 here
        wp->flags_0 |= 0x800000;
        if (old != (sint8)wp->val_71)
        {
            Sound_call(0x81, 1, 0);
        }
    }
    wp->val_78 += 0x80;
    wp->val_73++;
    wp->pos_14 += 7;
}

void Moji_text_option_bright(MOJI_WORK *wp)
{
    if (*D_8008D0D4[wp->pos_14[1]] == wp->pos_14[2])
    {
        wp->val_78 = 0x80;
    }
    else
    {
        wp->val_78 = 0x40;
    }
    wp->pos_14 += 3;
}

sint32 Moji_cmd_newline_n(MOJI_WORK *wp)
{
    wp->count_3B += 1;
    wp->pos_6C += 2;
    return 1;
}

void Moji_text_newline_n(MOJI_WORK *wp)
{
    wp->pos_10 = wp->val_8;
    wp->pos_12 += wp->pos_14[1];
    wp->val_3E = wp->val_70;
    wp->pos_14 += 2;
}

void Moji_text_bright(MOJI_WORK *wp)
{
    wp->val_78 = wp->pos_14[1];
    wp->pos_14 += 2;
}

sint32 func_8005619C(MOJI_WORK *wp)
{
    if (func_8005DA78(1) != 0)
    {
        wp->pos_6C += 1;
        return 1;
    }
    return 0;
}

sint32 func_800561E8(MOJI_WORK *wp)
{
    if (func_8005DAEC(1) != 0)
    {
        wp->pos_6C += 1;
        return 1;
    }
    return 0;
}

sint32 func_80056234(MOJI_WORK *wp)
{
    if (func_8005DA1C(1) != 0)
    {
        wp->pos_6C += 1;
        return 1;
    }
    return 0;
}

sint32 func_80056280(MOJI_WORK *wp)
{
    uint8 a;
    uint8 b;
    sint32 x;

    a = func_8005DA1C(0);
    b = func_8005DAEC(0);
    if (a == 0)
    {
        if (b == 0)
        {
            x = wp->pos_6C[1];
        }
        else
        {
            x = wp->pos_6C[2];
        }
    }
    else if (b == 0)
    {
        x = wp->pos_6C[3];
    }
    else
    {
        x = wp->pos_6C[4];
    }
    wp->val_4 = 5;
    if ((uint8)x == 0xFF)
    {
        wp->pos_6C += 5;
    }
    else
    {
        wp->val_C2 = x;
        if (wp->base_44 != 0)
        {
            wp->pos_6C = wp->pos_48 = wp->base_44 + ((uint16 *)wp->base_44)[(uint8)x];
        }
        else
        {
            wp->pos_6C = wp->pos_48 = D_8008CACC[(uint8)x];
        }
        wp->val_3E     = 0;
        wp->val_3C     = 0;
        wp->v_72.count = 0;
    }
    return 1;
}

void Moji_text_list_item(MOJI_WORK *wp)
{
    if (D_80098850 == 0)
    {
        Moji_item = Moji_list[Moji_list_top + (sint8)wp->val_71];
    }
    else
    {
        Moji_item = Moji_list[Moji_list_top + (sint8)wp->val_71] + 0x20;
    }
    if (wp->flags_0 & 0x2000)
    {
        if (wp->base_44 != 0)
        {
            MojiTaskExec(wp->pos_14[1], wp->base_44,
                         (uint8)(wp->pos_14[2] + Moji_list[Moji_list_top + (sint8)wp->val_71]));
        }
        else
        {
            MojiTaskExec(wp->pos_14[1],
                         D_8008CACC[wp->pos_14[2] + Moji_list[Moji_list_top + (sint8)wp->val_71]],
                         0xFF);
        }
    }
    wp->pos_14 += 3;
}

void Moji_text_choice_msg(MOJI_WORK *wp)
{
    if (wp->base_44 != 0)
    {
        MojiTaskExec(wp->pos_14[1], wp->base_44, (uint8)(wp->pos_14[2] + wp->val_71));
    }
    else
    {
        MojiTaskExec(wp->pos_14[1], D_8008CACC[wp->pos_14[2] + (sint8)wp->val_71], 0xFF);
    }
    wp->pos_14 += 3;
}

void Moji_text_list_choice_msg(MOJI_WORK *wp)
{
    if (wp->base_44 != 0)
    {
        MojiTaskExec(wp->pos_14[1], wp->base_44,
                     (uint8)(wp->pos_14[2] + Moji_list[(sint8)wp->val_71 + Moji_list_top]));
    }
    else
    {
        MojiTaskExec(
            wp->pos_14[1],
            D_8008CACC[wp->pos_14[2] + ((sint8 *)Moji_list)[(sint8)wp->val_71 + Moji_list_top]],
            0xFF);
    }
    wp->pos_14 += 3;
}

void Moji_text_parts_msg(MOJI_WORK *wp)
{
    if (wp->base_44 != 0)
    {
        MojiTaskExec(wp->pos_14[1], wp->base_44,
                     (uint8)(wp->pos_14[2] +
                             ((uint8 *)&Player_work)[(sint8)wp->val_71 + Moji_list_top + 0x454] - 1));
    }
    else
    {
        MojiTaskExec(wp->pos_14[1],
                     D_8008CAC8[wp->pos_14[2] +
                                ((uint8 *)&Player_work)[(sint8)wp->val_71 + Moji_list_top + 0x454]],
                     0xFF);
    }
    wp->pos_14 += 3;
}

void Moji_text_equip_msg(MOJI_WORK *wp)
{
    if (wp->base_44 != 0)
    {
        MojiTaskExec(
            wp->pos_14[1], wp->base_44,
            (uint8)(wp->pos_14[2] + ((uint8 *)&Player_work)[0x450 + (sint8)wp->val_71] - 1));
    }
    else
    {
        MojiTaskExec(wp->pos_14[1],
                     D_8008CAC8[wp->pos_14[2] + ((uint8 *)&Player_work)[0x450 + (sint8)wp->val_71]],
                     0xFF);
    }
    wp->pos_14 += 3;
}

sint32 Moji_cmd_list_choice(MOJI_WORK *wp)
{
    wp->pos_6C[5] = D_800987F8;
    if (D_80098850 == 0)
    {
        Moji_item = Moji_list[Moji_list_top + wp->v_72.skip];
    }
    else
    {
        Moji_item = Moji_list[Moji_list_top + wp->v_72.skip] + 0x20;
    }
    wp->flags_0 |= 0x402000;
    wp->v_72.count++;
    wp->pos_6C += 3;
    wp->val_BC = 0;
    return 1;
}

void Moji_text_list_choice(MOJI_WORK *wp)
{
    sint16 old;
    sint32 decided;

    wp->pos_14[3] = 0xED;
    decided       = 0;
    wp->val_78    = 0;
    if ((sint8)wp->val_71 == (sint8)wp->val_73)
    {
        if (!(wp->flags_0 & 0xC00000))
        {
            old = (sint8)wp->val_71;
            if (!(wp->val_BA & 0x4000))
            {
                if (wp->val_BA & 0x1000)
                {
                    decided    = 1;
                    wp->val_B8 = 0;
                    wp->val_BA = 0;
                    Sound_call(0x83, 1, 0);
                }
                else if (wp->val_BA & 0x10)
                {
                    wp->val_71 = wp->pos_14[2] >> 4;
                    wp->val_BC = 0;
                    if ((uint32)Moji_list_count >= 6 && old == (sint8)wp->val_71 && Moji_list_top != 0)
                    {
                        Sound_call(0x81, 1, 0);
                        Moji_list_top--;
                    }
                }
                else if ((*(uint32 *)&wp->val_B8 & 0x100010) == 0x10)
                {
                    if (wp->val_BC >= 13)
                    {
                        wp->val_71 = wp->pos_14[2] >> 4;
                        wp->val_BC = 12;
                        if ((uint32)Moji_list_count >= 6 && old == (sint8)wp->val_71 && Moji_list_top != 0)
                        {
                            Sound_call(0x81, 1, 0);
                            Moji_list_top--;
                        }
                    }
                    else
                    {
                        wp->val_BC++;
                    }
                }
                else if (wp->val_BA & 0x40)
                {
                    wp->val_71 = wp->pos_14[2] & 0xF;
                    wp->val_BC = 0;
                    if ((uint32)Moji_list_count >= 6 && old == (sint8)wp->val_71 &&
                        Moji_list_top < (uint32)Moji_list_count - 5)
                    {
                        Sound_call(0x81, 1, 0);
                        Moji_list_top++;
                    }
                }
                else if ((*(uint32 *)&wp->val_B8 & 0x400040) == 0x40)
                {
                    if (wp->val_BC >= 13)
                    {
                        wp->val_71 = wp->pos_14[2] & 0xF;
                        wp->val_BC = 12;
                        if ((uint32)Moji_list_count >= 6 && old == (sint8)wp->val_71 &&
                            Moji_list_top < (uint32)Moji_list_count - 5)
                        {
                            Sound_call(0x81, 1, 0);
                            Moji_list_top++;
                        }
                    }
                    else
                    {
                        wp->val_BC++;
                    }
                }
                else if (wp->val_BA & 0x80)
                {
                    wp->val_71 = wp->pos_14[1] >> 4;
                    wp->val_BC = 0;
                }
                else if ((*(uint32 *)&wp->val_B8 & 0x800080) == 0x80)
                {
                    if (wp->val_BC >= 13)
                    {
                        wp->val_71 = wp->pos_14[1] >> 4;
                        wp->val_BC = 12;
                    }
                    else
                    {
                        wp->val_BC++;
                    }
                }
                else if (wp->val_BA & 0x20)
                {
                    wp->val_71 = wp->pos_14[1] & 0xF;
                    wp->val_BC = 0;
                }
                else if ((*(uint32 *)&wp->val_B8 & 0x200020) == 0x20)
                {
                    if (wp->val_BC >= 13)
                    {
                        wp->val_71 = wp->pos_14[1] & 0xF;
                        wp->val_BC = 12;
                    }
                    else
                    {
                        wp->val_BC++;
                    }
                }
            }
            wp->pos_74 = wp->pos_14 + 3;
            wp->flags_0 |= 0x800000;
            if (old != (sint8)wp->val_71)
            {
                Sound_call(0x81, 1, 0);
            }
            if ((uint32)Moji_list_count >= 6)
            {
                Game_logo_kill(0);
                Game_logo_kill(1);
                if (!decided)
                {
                    if (Moji_list_top != 0 && Moji_list_count - 5 != Moji_list_top)
                    {
                        func_80063FC0(0, 0x20003);
                        func_80063FC0(1, 0x20004);
                    }
                    else if (Moji_list_top != 0)
                    {
                        func_80063FC0(0, 0x20003);
                    }
                    else if (Moji_list_count - 5 != Moji_list_top)
                    {
                        func_80063FC0(0, 0x20004);
                    }
                }
            }
        }
        if ((sint8)wp->val_71 == (sint8)wp->val_73)
        {
            wp->val_78 += 0x30;
            wp->pos_14[3] = 0x60;
        }
    }
    if (D_80098850 == 0)
    {
        Moji_item = Moji_list[Moji_list_top + (sint8)wp->val_73];
    }
    else
    {
        Moji_item = Moji_list[Moji_list_top + (sint8)wp->val_73] + 0x20;
    }
    wp->pos_14[5] = D_800987F8;
    wp->val_78 += 0x50;
    wp->val_73++;
    wp->pos_14 += 3;
    if (decided)
    {
        if (wp->base_44 != NULL)
        {
            MojiTaskExec(D_80098A64, wp->base_44, D_80098A65);
        }
        else
        {
            MojiTaskExec(D_80098A64, D_8008CACC[D_80098A65], 0xFF);
        }
    }
}

// Player_work +0xB0 is signed; the HP table index is 0x85 - n. The list append is written out in
// each branch (no goto), so loop.c keeps the constant 1 unhoisted. Each script arm stores pos_48
// itself: jump2 then cross-jumps that store into a new label, which its "other jump to the same
// label" search never visits, so the two base_44 paths stay separate as in retail. The unused
// 8-byte local gives the 0x40 frame.
sint32 Moji_cmd_shop_list(MOJI_WORK *wp)
{
    uint8 *have;
    uint8 *seen;
    uint32 base;
    sint32 i;
    sint32 n;
    sint32 unused[2];
    uint8 index;
    uint8 *script;

    base       = D_80097F64[D_80098850];
    Moji_list_count = 0;
    Moji_list_top = D_800988F4;
    have       = &Sce_flag[base >> 3];
    seen       = &Sce_flag[(sint32)(base - 0x80) >> 3];
    if (*(sint16 *)((uint8 *)&Player_work + 0xB0) >= 0xA0)
    {
        D_8008FD3C[0x23] = D_8008FE44;
    }
    else
    {
        D_8008FD3C[0x23] =
            D_8008FD3C[0x85 - (0xA0 - *(sint16 *)((uint8 *)&Player_work + 0xB0)) / 16];
    }
    if (MOJI_GAME->value7D >= 0x12)
    {
        D_8008FD3C[0x25] = D_8008FE62;
    }
    else
    {
        D_8008FD3C[0x25] = D_8008FE40[MOJI_GAME->value7D];
    }
    for (i = 0; i < D_80097F68[D_80098850]; i++)
    {
        if (Sce_flag_test(0x2EE) == 0)
        {
            if (!((seen[i >> 3] | ~have[i >> 3]) & (0x80 >> (i & 7))))
            {
                Moji_list[Moji_list_count] = i;
                Moji_list_count++;
            }
            else if (D_80098850 == 1)
            {
                if (i == 3)
                {
                    if (*(sint16 *)((uint8 *)&Player_work + 0xB0) != 0xA0)
                    {
                        Moji_list[Moji_list_count] = i;
                        Moji_list_count++;
                    }
                }
                else if (i == 5 && MOJI_GAME->value7D != 0 && MOJI_GAME->value7D < 0x63)
                {
                    Moji_list[Moji_list_count] = i;
                    Moji_list_count++;
                }
            }
        }
        else if (seen[i >> 3] & have[i >> 3] & (0x80 >> (i & 7)))
        {
            if (D_80098850 == 1)
            {
                Moji_list[Moji_list_count] = i;
                n                      = Moji_list_count++;
                if (i < 0x20 || (uint32)(i - 0x24) < 0x14 || (uint32)(i - 0x4C) < 5)
                {
                    Moji_list_count = n;
                }
            }
            else
            {
                Moji_list[Moji_list_count] = i;
                Moji_list_count++;
            }
        }
    }
    if (Moji_list_count != 0)
    {
        if ((uint32)Moji_list_count < 5)
        {
            index = (wp->pos_6C - Moji_list_count)[7];
            if (index == 0xFF)
            {
                wp->pos_6C += 7;
            }
            else
            {
                wp->val_C2 = index;
                if (wp->base_44 != NULL)
                {
                    script     = wp->base_44 + ((uint16 *)wp->base_44)[index];
                    wp->pos_48 = script;
                }
                else
                {
                    script     = D_8008CACC[index];
                    wp->pos_48 = script;
                }
                wp->pos_6C     = script;
                wp->val_3E     = 0;
                wp->val_3C     = 0;
                wp->v_72.count = 0;
            }
        }
        else
        {
            wp->pos_6C += 7;
        }
    }
    else
    {
        index = wp->pos_6C[2];
        if (index == 0xFF)
        {
            wp->pos_6C += 7;
        }
        else
        {
            wp->val_C2 = index;
            if (wp->base_44 != NULL)
            {
                script     = wp->base_44 + ((uint16 *)wp->base_44)[index];
                wp->pos_48 = script;
            }
            else
            {
                script     = D_8008CACC[index];
                wp->pos_48 = script;
            }
            wp->pos_6C     = script;
            wp->val_3E     = 0;
            wp->val_3C     = 0;
            wp->v_72.count = 0;
        }
    }
    return 1;
}

sint32 Moji_cmd_exec_if(MOJI_WORK *wp)
{
    uint8 *p;
    uint8 id;
    uint8 index;
    uint8 flag;

    p          = wp->pos_6C;
    id         = p[2];
    index      = p[3];
    flag       = p[1];
    wp->pos_6C = p + 4;
    if ((uint8)Moji_flag == flag)
    {
        if (wp->base_44 != 0)
        {
            MojiTaskExec((sint8)id, wp->base_44, index);
        }
        else
        {
            MojiTaskExec((sint8)id, D_8008CACC[index], 0xFF);
        }
    }
    return 1;
}

sint32 func_80057124(MOJI_WORK *wp)
{
    wp->val_40 = Game_work[7];
    wp->pos_6C += 1;
    return 1;
}

void func_80057144(MOJI_WORK *wp)
{
    wp->val_40 = Game_work[7];
    wp->pos_14 += 1;
    num_to_text(0, wp->val_40, wp->pos_14);
}

sint32 Moji_cmd_option_exec(MOJI_WORK *wp)
{
    uint8 *p;
    uint8 a;
    uint8 b;
    uint8 c;
    uint8 d;

    p          = wp->pos_6C;
    a          = p[1];
    b          = p[2];
    c          = p[3];
    d          = p[4];
    wp->pos_6C = p + 5;
    if (*D_8008D0D4[a] == b)
    {
        if (wp->base_44 != 0)
        {
            MojiTaskExec((sint8)c, wp->base_44, d);
        }
        else
        {
            MojiTaskExec((sint8)c, D_8008CACC[d], 0xFF);
        }
    }
    return 1;
}

sint32 Moji_cmd_shop_check(MOJI_WORK *wp)
{
    D_80098930 = Moji_list[Moji_list_top + (sint8)wp->val_71] + wp->pos_6C[1];
    if (Sce_flag_test(0x2EE) == 0 && Money_check(-D_8008FD3C[D_80098930]) != 0)
    {
        D_80098B6C = 1;
    }
    else
    {
        D_80098B6C = 0;
    }
    wp->pos_6C += 2;
    return 1;
}

sint32 Moji_cmd_shop_trade(MOJI_WORK *wp)
{
    uint8 *next;

    if (Sce_flag_test(0x2EE) != 0)
    {
        Money_add(D_8008FD3C[D_80098930] >> 2, 0, 0);
        Sce_flag_off(D_80098930 | 0x500);
        Sub_screen_rb_parts_set();
    }
    else
    {
        Money_add(-D_8008FD3C[D_80098930], 0, 0);
        if (D_80098930 == 0x23)
        {
            // Player_work + 0xB0 (not yet named in PL_WORK)
            Player_work.value6e = ((uint16 *)&Player_work)[0x58] += 0x10;
            func_8005DA78(1);
        }
        else if (D_80098930 == 0x24)
        {
            MOJI_GAME->value7D = MOJI_GAME->value7C = 3;
        }
        else if (D_80098930 == 0x25)
        {
            next               = &MOJI_GAME->value7D;
            MOJI_GAME->value7C = ++*next;
        }
        Sce_flag_on(D_80098930 | 0x500);
    }
    wp->pos_6C += 1;
    return 1;
}

sint32 Moji_cmd_jump(MOJI_WORK *wp)
{
    sint32 x;

    if (wp->pos_6C[0] == 0xD5)
    {
        x = wp->pos_6C[rand() % (sint32)wp->pos_6C[1] + 2];
    }
    else
    {
        x = wp->pos_6C[2];
    }
    if ((uint8)x == 0xFF)
    {
        if (wp->pos_6C[0] == 0xD5)
        {
            wp->pos_6C += wp->pos_6C[1] + 2;
        }
        else
        {
            wp->pos_6C += 3;
        }
    }
    else
    {
        wp->val_C2 = x;
        if (wp->base_44 != 0)
        {
            wp->pos_6C = wp->pos_48 = wp->base_44 + ((uint16 *)wp->base_44)[(uint8)x];
        }
        else
        {
            wp->pos_6C = wp->pos_48 = D_8008CACC[(uint8)x];
        }
        wp->val_3E     = 0;
        wp->val_3C     = 0;
        wp->v_72.count = 0;
    }
    return 1;
}

void Moji_text_jump(MOJI_WORK *wp)
{
    uint8 *p = wp->pos_14;

    if (p[0] == 0xD5)
    {
        wp->pos_14 = p + (p[1] + 2);
    }
    else
    {
        wp->pos_14 = p + 3;
    }
}

void Moji_text_price(MOJI_WORK *wp)
{
    if (Sce_flag_test(0x2EE) != 0)
    {
        if (D_80098850 == 0)
        {
            num_to_text(0, D_8008FD3C[Moji_list[Moji_list_top + (sint8)wp->val_73 - 1]] >> 2,
                          wp->pos_14 + 1);
        }
        else
        {
            num_to_text(0, D_8008FD7C[Moji_list[Moji_list_top + (sint8)wp->val_73 - 1]] >> 2,
                          wp->pos_14 + 1);
        }
    }
    else if (D_80098850 == 0)
    {
        num_to_text(0, D_8008FD3C[Moji_list[Moji_list_top + (sint8)wp->val_73 - 1]],
                      wp->pos_14 + 1);
    }
    else
    {
        num_to_text(0, D_8008FD7C[Moji_list[Moji_list_top + (sint8)wp->val_73 - 1]],
                      wp->pos_14 + 1);
    }
    wp->pos_14 += 1;
}

sint32 Moji_cmd_item_msg(MOJI_WORK *wp)
{
    wp->jump_tbl_4C[wp->depth_C0++] = (uint32)(wp->pos_6C + 1);
    wp->pos_6C                      = D_8008CBA4[Moji_item];
    return 1;
}

void Moji_text_item_msg(MOJI_WORK *wp)
{
    wp->jump_tbl_18[wp->depth_BE++] = (uint32)(wp->pos_14 + 1);
    wp->pos_14                      = D_8008CBA4[Moji_item];
}

sint32 func_80057744(MOJI_WORK *wp)
{
    sint32 x;

    if (MOJI_GAME->value7D == MOJI_GAME->value7C)
    {
        x = wp->pos_6C[1];
    }
    else
    {
        x = wp->pos_6C[2];
    }
    if ((uint8)x == 0xFF)
    {
        wp->pos_6C += 3;
    }
    else
    {
        wp->val_C2 = x;
        if (wp->base_44 != 0)
        {
            wp->pos_6C = wp->pos_48 = wp->base_44 + ((uint16 *)wp->base_44)[(uint8)x];
        }
        else
        {
            wp->pos_6C = wp->pos_48 = D_8008CACC[(uint8)x];
        }
        wp->val_3E     = 0;
        wp->val_3C     = 0;
        wp->v_72.count = 0;
    }
    return 1;
}

sint32 func_800577FC(MOJI_WORK *wp)
{
    sint32 x;

    if (Money_check(-((MOJI_GAME->value7D - MOJI_GAME->value7C) * 10)) == 0)
    {
        x = wp->pos_6C[1];
    }
    else if (MOJI_GAME->value60 >= 0x80 && Money_check(-1) == 0)
    {
        x = wp->pos_6C[3];
    }
    else
    {
        x = wp->pos_6C[2];
    }
    if ((uint8)x == 0xFF)
    {
        wp->pos_6C += 4;
    }
    else
    {
        wp->val_C2 = x;
        if (wp->base_44 != 0)
        {
            wp->pos_6C = wp->pos_48 = wp->base_44 + ((uint16 *)wp->base_44)[(uint8)x];
        }
        else
        {
            wp->pos_6C = wp->pos_48 = D_8008CACC[(uint8)x];
        }
        wp->val_3E     = 0;
        wp->val_3C     = 0;
        wp->v_72.count = 0;
    }
    return 1;
}


sint32 func_80057924(MOJI_WORK *wp)
{
    uint8 *next;

    if (wp->pos_6C[1] == 0)
    {
        next = &MOJI_GAME->value7D;
        Money_add(-((*next - MOJI_GAME->value7C) * 10), 0, 0);
        MOJI_GAME->value7C = *next;
    }
    else
    {
        Money_add(-MOJI_GAME->value1C, 0, 0);
        MOJI_GAME->value7C = MOJI_GAME->value7D;
    }
    wp->pos_6C += 2;
    return 1;
}

void func_800579D8(MOJI_WORK *wp)
{
    num_to_text(1, (MOJI_GAME->value7D - MOJI_GAME->value7C) * 10, ++wp->pos_14);
}


sint32 Moji_cmd_item_get(MOJI_WORK *wp)
{
    wp->jump_tbl_4C[wp->depth_C0++] = (uint32)(wp->pos_6C + 1);
    Sce_flag_on(Moji_item | 0x500);
    wp->pos_6C = D_8008CBA4[Moji_item];
    return 1;
}

void Moji_text_item_get(MOJI_WORK *wp)
{
    wp->jump_tbl_18[wp->depth_BE++] = (uint32)(wp->pos_14 + 1);
    wp->pos_14                      = D_8008CBA4[Moji_item];
}

sint32 Moji_cmd_item_call(MOJI_WORK *wp)
{
    wp->jump_tbl_4C[wp->depth_C0++] = (uint32)(wp->pos_6C + 2);
    wp->pos_6C                      = D_8008CBA4[wp->pos_6C[1]];
    return 1;
}

void Moji_text_item_call(MOJI_WORK *wp)
{
    wp->jump_tbl_18[wp->depth_BE++] = (uint32)(wp->pos_14 + 2);
    wp->pos_14                      = D_8008CBA4[wp->pos_14[1]];
}


sint32 Moji_cmd_money_add(MOJI_WORK *wp)
{
    Money_add(Moji_amount, 0, 0);
    wp->pos_6C += 1;
    return 1;
}


sint32 Moji_cmd_money_sub(MOJI_WORK *wp)
{
    Money_add(-Moji_amount, 0, 0);
    wp->pos_6C += 1;
    return 1;
}

void Moji_text_amount(MOJI_WORK *wp)
{
    wp->pos_14 += 1;
    num_to_text(1, Moji_amount, wp->pos_14);
}

sint32 func_80057C2C(MOJI_WORK *wp)
{
    sint32 count;
    uint32 i;
    sint32 x;
    uint8 *flags;

    flags = D_800BE3DB;
    count = 0;
    for (i = 0; i < 8; i++)
    {
        if (*flags & (0x80 >> i))
        {
            Moji_list[count++] = i;
        }
    }
    Moji_list[count] = 8;
    x                 = wp->pos_6C[count];
    if ((uint8)x == 0xFF)
    {
        wp->pos_6C += 9;
    }
    else
    {
        wp->val_C2 = x;
        if (wp->base_44 != 0)
        {
            wp->pos_6C = wp->pos_48 = wp->base_44 + ((uint16 *)wp->base_44)[(uint8)x];
        }
        else
        {
            wp->pos_6C = wp->pos_48 = D_8008CACC[(uint8)x];
        }
        wp->val_3E     = 0;
        wp->val_3C     = 0;
        wp->v_72.count = 0;
    }
    return 1;
}

sint32 func_80057D00(MOJI_WORK *wp)
{
    wp->jump_tbl_4C[wp->depth_C0++] = (uint32)(wp->pos_6C + 2);
    wp->pos_6C                      = D_8008CE10[Moji_list[wp->pos_6C[1]]];
    return 1;
}

void func_80057D60(MOJI_WORK *wp)
{
    wp->jump_tbl_18[wp->depth_BE++] = (uint32)(wp->pos_14 + 2);
    wp->pos_14                      = D_8008CE10[Moji_list[wp->pos_14[1]]];
}

sint32 func_80057DB8(MOJI_WORK *wp)
{
    uint32 index;

    index = Moji_flag & 0xFF;
    Moji_flag &= ~0xFF;
    Moji_flag |= Moji_list[index];
    wp->pos_6C += 1;
    return 1;
}

sint32 func_80057DF4(MOJI_WORK *wp)
{
    uint32 i;

    for (i = 0; i < 3; i++)
    {
        wp->pos_6C[i + 1] = (MOJI_GAME->value84 + D_800989D4)->value4[i];
    }
    wp->pos_6C += 1;
    return 1;
}

void func_80057E50(MOJI_WORK *wp)
{
    uint8 *p;

    p = wp->pos_14;
    if (p[1] == 0)
    {
        time_to_text(Moji_time, p + 3);
    }
    else if (p[1] == 1)
    {
        time_to_text(MOJI_GAME->value84[p[2]].value0, p + 3);
    }
    else if (p[1] == 2)
    {
        num2_to_text(MOJI_GAME->value84[p[2]].value0, p + 8);
        wp->pos_14[3] = 0xC6;
        wp->pos_14[4] = 0xC6;
        wp->pos_14[5] = 0xC6;
        wp->pos_14[6] = 0xC6;
        wp->pos_14[7] = 0xC6;
    }
    else if (p[1] == 3)
    {
        num2_to_text(Moji_num2, p + 8);
        wp->pos_14[3] = 0xC6;
        wp->pos_14[4] = 0xC6;
        wp->pos_14[5] = 0xC6;
        wp->pos_14[6] = 0xC6;
        wp->pos_14[7] = 0xC6;
    }
    else if (p[1] == 4)
    {
        wp->pos_14[3]  = D_80097FF4[0];
        wp->pos_14[4]  = D_80097FF4[1];
        wp->pos_14[5]  = D_80097FF4[2];
        wp->pos_14[6]  = D_80097FF4[3];
        wp->pos_14[7]  = D_80097FF4[4];
        wp->pos_14[8]  = D_80097FF4[5];
        wp->pos_14[9]  = D_80097FF4[6];
        wp->pos_14[10] = D_80097FF4[7];
    }
    wp->pos_14 += 3;
}

// The name being entered sits in 3-byte records just before the script pointer.
#define NAME_REC(wp, i) ((wp)->pos_6C - ((wp)->v_72.skip - (i)) * 3)
#define NAME_CHAR(wp)   (*(sint16 *)(wp)->unk_86)

// Name entry. Retail's name pointer is the record itself (Game_work+0x84, value4 reached at +4 in
// the store), hence the cast and name[i + 2]. `wp->val_71 = ++cur` (cur set twice, so GCC no
// longer knows it is a zero-extended byte) gives retail's cursor load order, and the left-arrow
// test reads a copy of the cursor shifted into the top byte (retail's sll 24 in the merge block).
sint32 Moji_cmd_name_entry(MOJI_WORK *wp)
{
    uint8 cur;
    sint32 cur_byte;
    uint32 held;
    uint16 done;
    uint16 i;
    uint16 *name;

    if (!(wp->flags_0 & 0x01000000))
    {
        wp->flags_0 |= 0x01000000;
        wp->val_BC    = 0;
        NAME_CHAR(wp) = 0;
        wp->v_72.skip = wp->pos_6C[1];
        wp->val_73    = 0;
        wp->val_71    = 0;
        D_800986E0    = 0x60;
        D_800986E4    = 2;
        for (i = 0; i < wp->v_72.skip; i++)
        {
            NAME_REC(wp, i)[2] = D_8008D0AC[0];
        }
    }
    done     = 0;
    cur      = wp->val_71;
    cur_byte = cur << 24;
    held     = *(uint32 *)&wp->val_B8;
    wp->flags_0 |= 0x10000000;
    if (held & 0x40200000)
    {
        wp->val_71 = ++cur;
        if ((sint8)wp->val_71 >= wp->v_72.skip)
        {
            wp->flags_0 &= ~0x01000000;
            name = (uint16 *)(MOJI_GAME->value84 + D_800989D4);
            for (i = 0; i < wp->v_72.skip; i++)
            {
                name[i + 2] = NAME_REC(wp, i)[2];
            }
            done = 1;
        }
        else
        {
            for (i = 0; i < 0x25; i++)
            {
                if (NAME_REC(wp, (sint8)wp->val_71)[2] == D_8008D0AC[i])
                {
                    break;
                }
            }
            NAME_CHAR(wp) = i;
        }
        Sound_call(0x82, 1, 0);
    }
    else if (held & 0x10800000)
    {
        if (cur_byte != 0)
        {
            wp->val_71 = cur - 1;
        }
        Sound_call(0x83, 1, 0);
        for (i = 0; i < 0x25; i++)
        {
            if (NAME_REC(wp, (sint8)wp->val_71)[2] == D_8008D0AC[i])
            {
                break;
            }
        }
        NAME_CHAR(wp) = i;
    }
    else if (wp->val_BA & 0x10)
    {
        wp->val_BC = 0;
        NAME_CHAR(wp)++;
        Sound_call(0x81, 1, 0);
    }
    else if ((held & 0x100010) == 0x10)
    {
        if (wp->val_BC >= 13)
        {
            wp->val_BC = 12;
            NAME_CHAR(wp)++;
            Sound_call(0x81, 1, 0);
        }
        else
        {
            wp->val_BC++;
        }
    }
    else if (wp->val_BA & 0x40)
    {
        wp->val_BC = 0;
        NAME_CHAR(wp)--;
        Sound_call(0x81, 1, 0);
    }
    else if ((held & 0x400040) == 0x40)
    {
        if (wp->val_BC >= 13)
        {
            wp->val_BC = 12;
            NAME_CHAR(wp)--;
            Sound_call(0x81, 1, 0);
        }
        else
        {
            wp->val_BC++;
        }
    }
    if (NAME_CHAR(wp) >= 0x25)
    {
        NAME_CHAR(wp) = 0;
    }
    if (NAME_CHAR(wp) < 0)
    {
        NAME_CHAR(wp) = 0x24;
    }
    if (done == 0)
    {
        for (i = 0; i < wp->v_72.skip; i++)
        {
            NAME_REC(wp, i)[1] = 0x50;
        }
        NAME_REC(wp, (sint8)wp->val_71)[2] = D_8008D0AC[NAME_CHAR(wp)];
        NAME_REC(wp, (sint8)wp->val_71)[1] = D_800986E0;
        D_800986E0 += D_800986E4;
        if (D_800986E0 == 0x90 || D_800986E0 == 0x60)
        {
            D_800986E4 = -D_800986E4;
        }
    }
    else
    {
        for (i = 0; i < wp->v_72.skip; i++)
        {
            NAME_REC(wp, i)[1] = 0x80;
        }
        wp->pos_6C += 2;
    }
    wp->flags_0 |= 0x800000;
    return done;
}

sint32 func_8005851C(MOJI_WORK *wp)
{
    uint16 *row;
    sint8 *level;
    uint8 *src;
    uint8 index;
    sint32 i;

    index = wp->pos_6C[1];
    row   = D_8008D8EC[D_800989D4][index];
    if (D_800989D4 != 0)
    {
        level = &((sint8(*)[8])((uint8 *)&Player_work + 0x4FC))[D_800989D4][index];
    }
    else
    {
        level = &((sint8 *)&Player_work + 0x4F4)[index];
    }
    if (row[0] == 0)
    {
        src = D_80097FEC;
    }
    else if (row[*level] == 0)
    {
        src = D_80097FE4;
    }
    else
    {
        D_80097FE2 = D_8008D028[*level + 1];
        src        = D_80097FDC;
    }
    for (i = 0; i < 8; i++)
    {
        wp->pos_6C[i + 2] = src[i];
    }
    wp->pos_6C += 2;
    return 1;
}

sint32 func_8005861C(MOJI_WORK *wp)
{
    uint16 *row;
    sint8 *level;
    uint8 *base;
    uint8 index;
    uint8 sel;

    wp->jump_tbl_4C[wp->depth_C0++] = (uint32)(wp->pos_6C + 5);
    index                           = wp->pos_6C[1];
    row                             = D_8008D8EC[D_800989D4][index];
    if (D_800989D4 != 0)
    {
        level = &((sint8(*)[8])((uint8 *)&Player_work + 0x4FC))[D_800989D4][index];
    }
    else
    {
        level = &((sint8 *)&Player_work + 0x4F4)[index];
    }
    if (row[0] == 0)
    {
        sel = wp->pos_6C[2];
        Moji_flag &= ~0x20000;
    }
    else if (row[*level] == 0)
    {
        sel = wp->pos_6C[3];
        Moji_flag &= ~0x20000;
    }
    else
    {
        sel = wp->pos_6C[4];
        Moji_flag |= 0x20000;
    }
    wp->val_C2 = sel;
    base       = wp->base_44;
    wp->pos_6C = base + ((uint16 *)base)[sel];
    return 1;
}

void func_80058740(MOJI_WORK *wp)
{
    uint8 *base;

    wp->jump_tbl_18[wp->depth_BE++] = (uint32)(wp->pos_14 + 5);
    base                            = wp->base_44;
    wp->pos_14                      = base + ((uint16 *)base)[wp->val_C2];
}

sint32 func_80058788(MOJI_WORK *wp)
{
    wp->flags_0 |= 0x400800;
    wp->v_72.count++;
    wp->pos_6C += 5;
    wp->val_BC = 0;
    Moji_flag3 &= ~(0x20000 << (wp - Moji_work));
    return 1;
}

void func_800587F8(MOJI_WORK *wp)
{
    uint32 held;
    uint32 pressed;
    sint16 old;

    wp->pos_14[5] = 0xED;
    wp->val_78    = 0;
    if ((sint8)wp->val_71 == (sint8)wp->val_73 && !(wp->flags_0 & 0xC00000))
    {
        Moji_flag3 &= ~(0x20000 << (wp - Moji_work));
        old = (sint8)wp->val_71;
        if ((sint32)Moji_work[0].flags_0 >= 0 && !((pressed = wp->val_BA) & 0x4000))
        {
            if (pressed & 0x10)
            {
                wp->val_71 = wp->pos_14[1];
                wp->val_BC = 0;
            }
            else if (((held = *(uint32 *)&wp->val_B8) & 0x100010) == 0x10)
            {
                if (wp->val_BC < 13)
                {
                    wp->val_BC++;
                }
                else
                {
                    wp->val_71 = wp->pos_14[1];
                    wp->val_BC = 12;
                }
            }
            else if (pressed & 0x40)
            {
                wp->val_71 = wp->pos_14[2];
                wp->val_BC = 0;
            }
            else if ((held & 0x400040) == 0x40)
            {
                if (wp->val_BC < 13)
                {
                    wp->val_BC++;
                }
                else
                {
                    wp->val_71 = wp->pos_14[2];
                    wp->val_BC = 12;
                }
            }
            else if (pressed & 0x80)
            {
                wp->val_71 = wp->pos_14[3];
                wp->val_BC = 0;
            }
            else if ((held & 0x800080) == 0x80)
            {
                if (wp->val_BC < 13)
                {
                    wp->val_BC++;
                }
                else
                {
                    wp->val_71 = wp->pos_14[3];
                    wp->val_BC = 12;
                }
            }
            else if (pressed & 0x20)
            {
                wp->val_71 = wp->pos_14[4];
                wp->val_BC = 0;
            }
            else if ((held & 0x200020) == 0x20)
            {
                if (wp->val_BC >= 13)
                {
                    wp->val_71 = wp->pos_14[4];
                    wp->val_BC = 12;
                }
                else
                {
                    wp->val_BC++;
                }
            }
        }
        if (!(Moji_flag & 0x20000))
        {
            wp->pos_14[5] = 0x60;
        }
        else
        {
            wp->pos_14[5] = 0xED;
        }
        wp->val_78 += 0x30;
        wp->pos_74 = wp->pos_14 + 5;
        wp->flags_0 |= 0x800000;
        if (old != (sint8)wp->val_71)
        {
            Sound_call(0x81, 1, 0);
            Moji_flag3 |= 0x20000 << (wp - Moji_work);
        }
    }
    wp->val_78 += 0x50;
    wp->val_73++;
    wp->pos_14 += 5;
}

sint32 func_80058AEC(MOJI_WORK *wp)
{

    if (!(wp->flags_0 & 0x1000000))
    {
        wp->flags_0 = (wp->flags_0 | 0x1000000) & ~0x8000;
        wp->val_4   = 5;
    }
    wp->flags_0 &= ~0x400000;
    if (Moji_flag & 0x8000000)
    {
        Moji_flag |= 0x20000;
    }
    else
    {
        Moji_flag &= ~0x20000;
    }
    if (wp->val_4 <= 0)
    {
        if (wp->val_BA & 0x4000)
        {
            Moji_flag &= ~0xFF;
            Moji_flag |= 0x40000000 | wp->val_71;
        }
        else if (wp->val_BA & 0x1000)
        {
            Moji_flag &= ~0xFF;
            Moji_flag |= 0x20000000 | ((wp->v_72.skip - 1) & 0xFF);
        }
    }
    else
    {
        wp->val_4--;
    }
    wp->val_78 = 0x80;
    return 0;
}

sint32 func_80058C08(MOJI_WORK *wp)
{
    wp->val_3D = wp->pos_6C[1];
    wp->pos_6C += 2;
    return 1;
}

sint32 Moji_cmd_flag_call(MOJI_WORK *wp)
{
    wp->jump_tbl_4C[wp->depth_C0++] = (uint32)(wp->pos_6C + 3);
    if (Sce_flag_test(get_le16(wp->pos_6C + 1)))
    {
        wp->pos_6C = D_8008BAA4[get_le16(wp->pos_6C + 1)];
    }
    else
    {
        wp->pos_6C = D_8008CE58;
    }
    return 1;
}

void Moji_text_flag_call(MOJI_WORK *wp)
{
    wp->jump_tbl_18[wp->depth_BE++] = (uint32)(wp->pos_14 + 3);
    if (Sce_flag_test(get_le16(wp->pos_14 + 1)))
    {
        wp->pos_14 = D_8008BAA4[get_le16(wp->pos_14 + 1)];
    }
    else
    {
        wp->pos_14 = D_8008CE58;
    }
}

void Moji_text_locate(MOJI_WORK *wp)
{
    wp->pos_10 = get_le16(wp->pos_14 + 1);
    wp->pos_12 = get_le16(wp->pos_14 + 3);
    wp->pos_14 += 5;
}

sint32 Moji_cmd_list_clear(MOJI_WORK *wp)
{
    sint32 i;
    uint8 *p;
    uint8 value;

    value      = 0xFF;
    i          = 0x7F;
    Moji_list_top = 0;
    wp->pos_6C += 1;
    p = &Moji_list[0x7F];
    for (; i >= 0; i--)
    {
        *p-- = value;
    }
    return 1;
}

void Moji_text_list_from_flags(MOJI_WORK *wp)
{
    uint8 *bits;
    sint32 i;

    bits       = D_800BE420;
    Moji_list_count = 0;
    for (i = 0; i < 0x40; i++)
    {
        if (bits[i >> 3] & (0x80 >> (i & 7)))
        {
            Moji_list[Moji_list_count] = i;
            Moji_list_count++;
        }
    }
    for (i = Moji_list_count; i < 0x80; i++)
    {
        Moji_list[i] = 0xFF;
    }
    wp->pos_14++;
}

sint32 func_80058EA0(MOJI_WORK *wp)
{
    wp->flags_0 |= 0x402800;
    wp->v_72.count++;
    wp->pos_6C += 5;
    wp->val_BC = 0;
    D_80098851 = 0xFF;
    Moji_flag3 &= ~(0x20000 << (wp - Moji_work));
    return 1;
}

void func_80058F18(MOJI_WORK *wp)
{
    uint32 bit;
    sint16 old;

    if (D_80098851 == (sint8)wp->val_73 + Moji_list_top && (D_80098960 & 1))
    {
        wp->pos_14[5] = 0x60;
        wp->val_78    = 0x10;
    }
    else
    {
        wp->pos_14[5] = 0xED;
        wp->val_78    = 0;
    }
    if ((sint8)wp->val_71 == (sint8)wp->val_73 && !(wp->flags_0 & 0xC00000))
    {
        bit = 0x20000 << (wp - Moji_work);
        Moji_flag3 &= ~bit;
        old = (sint8)wp->val_71;
        if (!(wp->val_BA & 0x4000))
        {
            if (wp->val_BA & 0x10)
            {
                wp->val_71 = wp->pos_14[1];
                wp->val_BC = 0;
                if (old == (sint8)wp->val_71 && Moji_list_top != 0)
                {
                    Moji_flag3 |= bit;
                    Sound_call(0x81, 1, 0);
                    Moji_list_top -= 2;
                }
            }
            else if ((*(uint32 *)&wp->val_B8 & 1048592) == 0x10)
            {
                if (wp->val_BC >= 13)
                {
                    wp->val_71 = wp->pos_14[1];
                    wp->val_BC = 12;
                    if (old == (sint8)wp->val_71 && Moji_list_top != 0)
                    {
                        Moji_flag3 |= bit;
                        Sound_call(0x81, 1, 0);
                        Moji_list_top -= 2;
                    }
                }
                else
                {
                    wp->val_BC++;
                }
            }
            else if (wp->val_BA & 0x40)
            {
                wp->val_71 = wp->pos_14[2];
                wp->val_BC = 0;
                if (old == (sint8)wp->val_71 && Moji_list_top < D_800989D4)
                {
                    Moji_flag3 |= bit;
                    Sound_call(0x81, 1, 0);
                    Moji_list_top += 2;
                }
            }
            else if ((*(uint32 *)&wp->val_B8 & 4194368) == 0x40)
            {
                if (wp->val_BC >= 13)
                {
                    wp->val_71 = wp->pos_14[2];
                    wp->val_BC = 12;
                    if (old == (sint8)wp->val_71 && Moji_list_top < D_800989D4)
                    {
                        Moji_flag3 |= bit;
                        Sound_call(0x81, 1, 0);
                        Moji_list_top += 2;
                    }
                }
                else
                {
                    wp->val_BC++;
                }
            }
            else if (wp->val_BA & 0x80)
            {
                wp->val_71 = wp->pos_14[3];
                wp->val_BC = 0;
            }
            else if ((*(uint32 *)&wp->val_B8 & 8388736) == 0x80)
            {
                if (wp->val_BC >= 13)
                {
                    wp->val_71 = wp->pos_14[3];
                    wp->val_BC = 12;
                }
                else
                {
                    wp->val_BC++;
                }
            }
            else if (wp->val_BA & 0x20)
            {
                wp->val_71 = wp->pos_14[4];
                wp->val_BC = 0;
            }
            else if ((*(uint32 *)&wp->val_B8 & 2097184) == 0x20)
            {
                if (wp->val_BC >= 13)
                {
                    wp->val_71 = wp->pos_14[4];
                    wp->val_BC = 12;
                }
                else
                {
                    wp->val_BC++;
                }
            }
        }
        wp->pos_14[5] = 0x60;
        wp->val_78    = 0x30;
        wp->pos_74    = wp->pos_14 + 5;
        wp->flags_0 |= 0x800000;
        if (old != (sint8)wp->val_71)
        {
            Sound_call(0x81, 1, 0);
            Moji_flag3 |= 0x20000 << (wp - Moji_work);
        }
    }
    wp->val_78 += 0x50;
    wp->val_73++;
    wp->pos_14 += 5;
}

sint32 func_800592FC(MOJI_WORK *wp)
{
    uint32 slot;
    uint32 value;

    if (!(wp->flags_0 & 0x1000000))
    {
        wp->flags_0 = (wp->flags_0 | 0x1000000) & ~0x8000;
        wp->val_4   = 5;
    }
    wp->flags_0 &= ~0x400000;
    if (wp->val_4 <= 0)
    {
        Game_logo_kill(0);
        Game_logo_kill(1);
        if (wp->val_BA & 0x4000)
        {
            Moji_flag &= ~0xFF;
            slot = Moji_list[Moji_list_top + (sint8)wp->val_71];
            if (slot != 0xFF)
            {
                Moji_flag |= 0x40000000 | (uint8)(slot + 0x40);
            }
            else
            {
                Moji_flag |= 0x400000FF;
            }
        }
        else if (Moji_list_top != 0 && Moji_list_top < D_800989D4)
        {
            func_80063FC0(0, 0x2001A);
            func_80063FC0(1, 0x2001B);
        }
        else if (Moji_list_top != 0)
        {
            func_80063FC0(0, 0x2001A);
        }
        else if (Moji_list_top < D_800989D4)
        {
            func_80063FC0(0, 0x2001B);
        }
    }
    else
    {
        wp->val_4 -= 1;
    }
    value = Moji_list[Moji_list_top + (sint8)wp->val_71];
    if (value != 0xFF)
    {
        Moji_flag3 = (uint8)(value + 0x40);
    }
    else
    {
        Moji_flag3 = value;
    }
    wp->val_78 = 0x80;
    return 0;
}

sint32 Moji_cmd_list_msg(MOJI_WORK *wp)
{
    wp->jump_tbl_4C[wp->depth_C0++] = (uint32)(wp->pos_6C + 1);
    wp->pos_6C = D_8008CCA4[((sint8 *)Moji_list)[Moji_list_top + wp->v_72.skip - 1]];
    return 1;
}

void Moji_text_list_msg(MOJI_WORK *wp)
{
    wp->jump_tbl_18[wp->depth_BE++] = (uint32)(wp->pos_14 + 1);
    wp->pos_14 = D_8008CCA4[((sint8 *)Moji_list)[Moji_list_top + (sint8)wp->val_73 - 1]];
}

sint32 Moji_cmd_part_name(MOJI_WORK *wp)
{
    wp->jump_tbl_4C[wp->depth_C0++] = (uint32)(wp->pos_6C + 3);
    if (wp->pos_6C[1] != 0)
    {
        wp->pos_6C = D_8008CBA0[((sint8 *)&Player_work)[wp->pos_6C[2] + Moji_list_top + 0x454]];
    }
    else
    {
        wp->pos_6C[-1] = 0;
        if ((Moji_flag & 0x20000) && wp->pos_6C[2] == (sint8)wp->val_71)
        {
            wp->pos_6C[-1] = 3;
        }
        wp->pos_6C = D_8008CBA0[((sint8 *)&Player_work)[wp->pos_6C[2] + 0x450]];
    }
    return 1;
}

void Moji_text_part_name(MOJI_WORK *wp)
{
    wp->jump_tbl_18[wp->depth_BE++] = (uint32)(wp->pos_14 + 3);
    if (wp->pos_14[1] != 0)
    {
        wp->pos_14 = D_8008CBA0[((sint8 *)&Player_work)[wp->pos_14[2] + Moji_list_top + 0x454]];
    }
    else
    {
        wp->pos_14[-1] = 0;
        if ((Moji_flag & 0x20000) && wp->pos_14[3] == (sint8)wp->val_71)
        {
            wp->pos_14[-1] = 3;
        }
        wp->pos_14 = D_8008CBA0[((sint8 *)&Player_work)[wp->pos_14[2] + 0x450]];
    }
}

sint32 Moji_cmd_buster_slot(MOJI_WORK *wp)
{
    if (!(wp->flags_0 & 0x1000000))
    {
        wp->flags_0 = (wp->flags_0 | 0x1000000) & ~0x8000;
        wp->val_4   = 5;
        wp->val_71  = wp->val_79;
    }
    wp->val_78 = 0x80;
    wp->flags_0 &= ~0x400000;
    Moji_flag3 = ((uint8 *)&Player_work)[(sint8)Moji_work[1].val_71 + 0x450];
    Moji_flag3 = Moji_flag3 |
                 (((uint8 *)&Player_work)[Moji_list_top + Moji_work[2].val_79 + 0x454] << 8) | 0x10000;
    if (wp->val_4 <= 0)
    {
        if (*(uint32 *)&wp->val_B8 & 0x10080000)
        {
            Sound_call(0x83, 1, 0);
            wp->flags_0 &= ~0x1000000;
            wp->val_79 = wp->val_71;
            MojiTaskExec(wp->pos_6C[3], wp->base_44, wp->pos_6C[4]);
            return 0;
        }
        if (wp->val_BA & 0x4000)
        {
            Sound_call(0x82, 1, 0);
            wp->flags_0 &= ~0x1000000;
            MojiTaskExec(wp->pos_6C[1], wp->base_44, wp->pos_6C[2]);
            Moji_flag |= 0x28000;
            wp->val_79 = wp->val_71;
        }
    }
    else
    {
        wp->val_4 -= 1;
    }
    return 0;
}

// The script hand-off is written out in both confirm branches; GCC cross-jumps the copies.
sint32 Moji_cmd_buster_equip(MOJI_WORK *wp)
{
    uint8 *parts;
    uint8 *script;
    uint8 temp;
    uint8 next;
    uint8 index;

    if (!(wp->flags_0 & 0x1000000))
    {
        wp->flags_0 = (wp->flags_0 | 0x1000000) & ~0x8000;
        wp->val_4   = 5;
        wp->val_71  = wp->val_79;
    }
    wp->flags_0 &= ~0x400000;
    Moji_flag3 = ((uint8 *)&Player_work)[(sint8)Moji_work[1].val_71 + 0x450];
    parts      = (uint8 *)&Player_work + 0x454;
    Moji_flag3 = Moji_flag3 | (parts[Moji_list_top + (sint8)Moji_work[2].val_71] << 8);
    if (wp->val_4 <= 0)
    {
        Game_logo_kill(0);
        Game_logo_kill(1);
        if (*(uint32 *)&wp->val_B8 & 0x10080000)
        {
            Sound_call(0x83, 1, 0);
            script     = wp->base_44;
            wp->val_79 = wp->val_71;
            wp->flags_0 &= ~0x1000000;
            Moji_flag &= ~0x28000;
            index      = wp->pos_6C[4];
            next       = wp->pos_6C[3];
            wp->val_C2 = index;
            MojiTaskExec(wp->pos_6C[1], script, wp->pos_6C[2]);
            MojiTaskExec(next, script, index);
        }
        else if (wp->val_BA & 0x4000)
        {
            Sound_call(0x82, 1, 0);
            temp                                  = parts[(sint8)wp->val_71 + Moji_list_top];
            parts[(sint8)wp->val_71 + Moji_list_top] = ((uint8 *)&Player_work)[D_80098B2D + 0x450];
            ((uint8 *)&Player_work)[D_80098B2D + 0x450] = temp;
            Sub_screen_rb_parts_set();
            script     = wp->base_44;
            wp->val_79 = wp->val_71;
            wp->flags_0 &= ~0x1000000;
            Moji_flag &= ~0x28000;
            index      = wp->pos_6C[4];
            next       = wp->pos_6C[3];
            wp->val_C2 = index;
            MojiTaskExec(wp->pos_6C[1], script, wp->pos_6C[2]);
            MojiTaskExec(next, script, index);
        }
        else if (Moji_list_top != 0 && Moji_list_top < D_800989D4)
        {
            func_80063FC0(0, 0x20018);
            func_80063FC0(1, 0x20019);
        }
        else if (Moji_list_top != 0)
        {
            func_80063FC0(0, 0x20018);
        }
        else if (Moji_list_top < D_800989D4)
        {
            func_80063FC0(0, 0x20019);
        }
    }
    else
    {
        wp->val_4 -= 1;
    }
    return 0;
}

sint32 func_80059B44(MOJI_WORK *wp)
{
    sint32 count;
    sint32 value;

    count = 0;
    if (Sce_flag_test(0x23D) != 0 && MOJI_GAME->value28 != 0x1F4)
    {
        Moji_list[count++] = 0;
    }
    if (Sce_flag_test(0x23E) != 0 && MOJI_GAME->value2C != 0xC8)
    {
        Moji_list[count++] = 1;
    }
    if (Sce_flag_test(0x23F) != 0 && MOJI_GAME->value30 != 0xFA)
    {
        Moji_list[count++] = 2;
    }
    if (Sce_flag_test(0x240) != 0 && MOJI_GAME->value34 != 0xC8)
    {
        Moji_list[count++] = 3;
    }
    if (MOJI_GAME->value66 * 0x96 != MOJI_GAME->value38)
    {
        Moji_list[count++] = 5;
    }
    if (MOJI_GAME->value67 * 0xB4 != MOJI_GAME->value3C)
    {
        Moji_list[count++] = 4;
    }
    Moji_list[count] = 6;
    value             = wp->pos_6C[1] + 6;
    value -= count;
    wp->val_C2 = (uint8)value;
    wp->pos_6C = wp->pos_48 = wp->base_44 + ((uint16 *)wp->base_44)[(uint8)value];
    wp->val_3E              = 0;
    wp->val_3C              = 0;
    wp->v_72.count          = 0;
    return 1;
}

sint32 func_80059D20(MOJI_WORK *wp)
{
    sint32 index;
    uint8 *p;
    uint8 slot;
    sint32 limit;

    p     = wp->pos_6C;
    index = p[1];
    slot  = Moji_list[index];
    limit = D_8008AF00[slot];
    if (slot < 4)
    {
        num_to_text(0, limit - ((sint32 *)((uint8 *)Game_work + 0x28))[slot], p + 7);
    }
    else
    {
        num_to_text(0,
                      ((uint8 *)Game_work)[0x6B - slot] * limit -
                          *(sint32 *)((uint8 *)Game_work + 0x4C - slot * 4),
                      p + 7);
    }
    wp->jump_tbl_4C[wp->depth_C0++] = (uint32)(wp->pos_6C + 2);
    wp->pos_6C                      = D_8008CE5C[Moji_list[index]];
    return 1;
}

void func_80059E24(MOJI_WORK *wp)
{
    uint32 idx = wp->pos_14[1];

    wp->jump_tbl_18[wp->depth_BE++] = (uint32)(wp->pos_14 + 2);
    wp->pos_14                      = D_8008CE5C[Moji_list[idx]];
}

// Number entry. As in Moji_cmd_name_entry, a uint8 cursor plus a copy shifted into the top byte
// (cur << 24) for the left-arrow test gives retail's cursor load and sll; done = 1 written last
// in the two confirm branches gives retail's placement.
sint32 func_80059E74(MOJI_WORK *wp)
{
    uint8 cur;
    sint32 cur_byte;
    uint16 done;
    uint16 i;
    uint32 limit;

    if (!(wp->flags_0 & 0x01000000))
    {
        wp->flags_0 |= 0x01000000;
        wp->val_BC    = 0;
        NAME_CHAR(wp) = 0;
        wp->v_72.skip = wp->pos_6C[1];
        wp->val_73    = 0;
        wp->val_71    = 4;
        wp->val_4     = 5;
        D_800986E8    = 0x60;
        D_800986EC    = 2;
        for (i = 0; i < wp->v_72.skip; i++)
        {
            NAME_REC(wp, i)[2] = D_8008D028[0];
        }
    }
    cur      = wp->val_71;
    cur_byte = cur << 24;
    done     = 0;
    wp->flags_0 |= 0x10000000;
    if (wp->val_4 <= 0)
    {
        if (wp->val_BA & 0x4000)
        {
            if (D_800987E0 != 0)
            {
                Sound_call(0x82, 1, 0);
                wp->flags_0 &= ~0x01000000;
                done = 1;
            }
            else
            {
                Sound_call(0x1C6, 1, 0);
            }
        }
        else if (wp->val_BA & 0x1000)
        {
            Sound_call(0x83, 1, 0);
            D_800987E0 = 0;
            wp->flags_0 &= ~0x01000000;
            done = 1;
        }
        else if (wp->val_BA & 0x20)
        {
            wp->val_71 = cur + 1;
            if ((sint8)wp->val_71 < wp->v_72.skip - 1)
            {
                for (i = 0; i < 10; i++)
                {
                    if (NAME_REC(wp, (sint8)wp->val_71)[2] == D_8008D028[i])
                    {
                        break;
                    }
                }
                NAME_CHAR(wp) = i;
                Sound_call(0x83, 1, 0);
            }
            else
            {
                wp->val_71 = wp->v_72.skip - 2;
            }
        }
        else if (wp->val_BA & 0x80)
        {
            if (cur_byte != 0)
            {
                wp->val_71 = cur - 1;
            }
            Sound_call(0x83, 1, 0);
            for (i = 0; i < 10; i++)
            {
                if (NAME_REC(wp, (sint8)wp->val_71)[2] == D_8008D028[i])
                {
                    break;
                }
            }
            NAME_CHAR(wp) = i;
        }
        else if (wp->val_BA & 0x10)
        {
            wp->val_BC = 0;
            NAME_CHAR(wp)++;
            Sound_call(0x81, 1, 0);
        }
        else if ((*(uint32 *)&wp->val_B8 & 0x100010) == 0x10)
        {
            if (wp->val_BC >= 13)
            {
                wp->val_BC = 12;
                NAME_CHAR(wp)++;
                Sound_call(0x81, 1, 0);
            }
            else
            {
                wp->val_BC++;
            }
        }
        else if (wp->val_BA & 0x40)
        {
            wp->val_BC = 0;
            NAME_CHAR(wp)--;
            Sound_call(0x81, 1, 0);
        }
        else if ((*(uint32 *)&wp->val_B8 & 0x400040) == 0x40)
        {
            if (wp->val_BC >= 13)
            {
                wp->val_BC = 12;
                NAME_CHAR(wp)--;
                Sound_call(0x81, 1, 0);
            }
            else
            {
                wp->val_BC++;
            }
        }
    }
    else
    {
        wp->val_4--;
    }
    if (NAME_CHAR(wp) >= 10)
    {
        NAME_CHAR(wp) = 0;
    }
    if (NAME_CHAR(wp) < 0)
    {
        NAME_CHAR(wp) = 9;
    }
    if (done == 0)
    {
        for (i = 0; i < wp->v_72.skip; i++)
        {
            NAME_REC(wp, i)[1] = 0x50;
        }
        NAME_REC(wp, (sint8)wp->val_71)[2] = D_8008D028[NAME_CHAR(wp)];
        NAME_REC(wp, (sint8)wp->val_71)[1] = D_800986E8;
        D_800986E8 += D_800986EC;
        if (D_800986E8 == 0x90 || D_800986E8 == 0x60)
        {
            D_800986EC = -D_800986EC;
        }
        D_800987E0 = 0;
        for (i = 0; i < wp->v_72.skip - 1; i++)
        {
            D_800987E0 *= 10;
            D_800987E0 += NAME_REC(wp, i)[2] - D_8008D028[0];
        }
        if (Money_check(-Moji_amount) != 0)
        {
            limit = MOJI_GAME->value1C;
        }
        else
        {
            limit = Moji_amount;
        }
        if (limit < D_800987E0)
        {
            num_to_text(2, limit, Moji_list);
            for (i = 0; i < wp->v_72.skip - 1; i++)
            {
                NAME_REC(wp, i)[2] = Moji_list[i + 1];
            }
            D_800987E0    = limit;
            NAME_CHAR(wp) = NAME_REC(wp, (sint8)wp->val_71)[2] - D_8008D028[0];
        }
    }
    else
    {
        num_to_text(2, D_800987E0, Moji_list);
        for (i = 0; i < wp->v_72.skip - 1; i++)
        {
            NAME_REC(wp, i)[2] = Moji_list[i + 1];
        }
        for (i = 0; i < wp->v_72.skip; i++)
        {
            NAME_REC(wp, i)[1] = 0x80;
        }
        wp->pos_6C += 2;
    }
    wp->flags_0 |= 0x800000;
    return done;
}

sint32 func_8005A598(MOJI_WORK *wp)
{
    sint32 x;
    sint16 value;

    value = ((MOJI_WORK_G *)Game_work)->value60;
    if (value >= wp->pos_6C[1] && value <= wp->pos_6C[2])
    {
        x = wp->pos_6C[3];
    }
    else
    {
        x = wp->pos_6C[4];
    }
    if ((uint8)x == 0xFF)
    {
        wp->pos_6C += 5;
    }
    else
    {
        wp->val_C2 = x;
        wp->pos_6C = wp->base_44 + ((uint16 *)wp->base_44)[(uint8)x];
        wp->pos_48 = wp->pos_6C;
    }
    return 1;
}

sint32 Moji_cmd_parts_sort(MOJI_WORK *wp)
{
    uint8 temp;

    if (!(wp->flags_0 & 0x1000000))
    {
        wp->flags_0 = (wp->flags_0 | 0x1000000) & ~0x8000;
        wp->val_4   = 5;
        wp->val_71  = wp->val_79;
        D_80098851  = 0xFF;
    }
    wp->flags_0 &= ~0x400000;
    if (wp->val_4 <= 0)
    {
        Game_logo_kill(0);
        Game_logo_kill(1);
        if (*(uint32 *)&wp->val_B8 & 0x10080000)
        {
            Sound_call(0x83, 1, 0);
            wp->val_79 = wp->val_71;
            wp->flags_0 &= ~0x1000000;
            MojiTaskExec(wp->pos_6C[1], wp->base_44, wp->pos_6C[2]);
            D_80098851 = 0xFF;
        }
        else if (wp->val_BA & 0x4000)
        {
            Sound_call(0x82, 1, 0);
            if (D_80098851 != 0xFF)
            {
                temp = ((uint8 *)&Player_work)[(sint8)wp->val_71 + Moji_list_top + 0x454];
                ((uint8 *)&Player_work)[(sint8)wp->val_71 + Moji_list_top + 0x454] =
                    ((uint8 *)&Player_work)[D_80098851 + 0x454];
                ((uint8 *)&Player_work)[D_80098851 + 0x454] = temp;
                Sub_screen_rb_parts_set();
                wp->val_79 = wp->val_71;
                D_80098851 = 0xFF;
            }
            else
            {
                D_80098851 = wp->val_71 + Moji_list_top;
            }
        }
        else if (Moji_list_top != 0 && Moji_list_top < D_800989D4)
        {
            func_80063FC0(0, 0x20018);
            func_80063FC0(1, 0x20019);
        }
        else if (Moji_list_top != 0)
        {
            func_80063FC0(0, 0x20018);
        }
        else if (Moji_list_top < D_800989D4)
        {
            func_80063FC0(0, 0x20019);
        }
    }
    else
    {
        wp->val_4 -= 1;
    }
    return 0;
}

void Moji_text_char_space(MOJI_WORK *wp)
{
    wp->pos_10 += 12;
    wp->pos_14 += 1;
}

// The edge tests are written with the moving edge on the left; the reset writes the edges in
// field order.
void func_8005A874(MOJI_WORK *wp)
{
    uint32 size;

    switch (wp->val_7D)
    {
        case 0:
            wp->val_85 = 8;
            wp->flags_0 |= 0x14000000;
            wp->val_80 = wp->flags_0 & 0x08000000;
            wp->flags_0 |= 0x08000000;
            size = wp->pos_6C[2];
            wp->val_7D++;
            wp->val_8A = wp->val_7F * 6;
            wp->val_B4 = size;
            wp->val_8E = wp->val_7F * 6;
            wp->val_92 = wp->val_7F * 6;
            wp->val_96 = wp->val_7F * 6;
            wp->val_88 = -wp->val_B4;
            wp->val_B6 = size;
            wp->val_8C = wp->val_B4 + wp->val_7E * 12;
            wp->val_90 = -wp->val_B4;
            wp->val_94 = wp->val_B4 + wp->val_7E * 12;
            break;
        case 1:
            wp->flags_0 |= 0x10000000;
            wp->val_8A -= wp->val_85;
            wp->val_8E -= wp->val_85;
            wp->val_92 += wp->val_85;
            wp->val_96 += wp->val_85;
            if (wp->val_A + wp->val_8A <= wp->val_A - wp->val_B6)
            {
                Moji_flag |= 0x200000;
                wp->val_88 = -wp->val_B4;
                wp->val_8A = -wp->val_B6;
                wp->val_8C = wp->val_B4 + wp->val_7E * 12;
                wp->val_8E = -wp->val_B6;
                wp->val_90 = -wp->val_B4;
                wp->val_92 = wp->val_B6 + wp->val_7F * 12;
                wp->val_94 = wp->val_B4 + wp->val_7E * 12;
                wp->val_96 = wp->val_B6 + wp->val_7F * 12;
                wp->flags_0 |= 0x12000000;
                wp->flags_0 = (wp->flags_0 & ~0x08000000) | wp->val_80;
                wp->val_7D++;
                wp->pos_6C += 3;
            }
            break;
        case 2:
            wp->flags_0 |= 0x10000000;
            wp->val_8A += wp->val_85;
            wp->val_8E += wp->val_85;
            wp->val_92 -= wp->val_85;
            wp->val_96 -= wp->val_85;
            if (wp->val_8A > wp->val_92)
            {
                wp->flags_0 &= ~0x06000000;
            }
            break;
    }
}

// The flags store comes first, so GCC reloads the counter rather than folding the switch value.
void func_8005AB78(MOJI_WORK *wp)
{
    uint8 size;

    switch (wp->val_7D)
    {
        case 0:
            wp->flags_0 |= 0x6000000;
            wp->val_7D += 2;
            size       = wp->pos_6C[2];
            wp->val_B4 = wp->val_B6 = size;
            Moji_flag |= 0x200000;
            wp->pos_6C += 3;
            wp->val_88 = -wp->val_B4;
            wp->val_8A = -wp->val_B6;
            wp->val_8C = wp->val_B4 + wp->val_7E * 12;
            wp->val_8E = -wp->val_B6;
            wp->val_90 = -wp->val_B4;
            wp->val_92 = wp->val_B6 + wp->val_7F * 12;
            wp->val_94 = wp->val_B4 + wp->val_7E * 12;
            wp->val_96 = wp->val_B6 + wp->val_7F * 12;
            break;
        case 2:
            wp->flags_0 = (wp->flags_0 | 0x10000000) & ~0x6000000;
            break;
    }
}

void func_8005ACA8(MOJI_WORK *wp)
{
    DR_MODE *mode;
    DR_MODE *mode2;

    wp->val_8C = wp->val_B4 + wp->val_7E * 12;
    wp->val_94 = wp->val_B4 + wp->val_7E * 12;
    func_8005BCE4(wp, D_8008AF18[wp->val_7C]);
    wp->prim_A0[0]->x0 = wp->val_8 + wp->val_88 - 1;
    wp->prim_A0[0]->y0 = wp->val_A + wp->val_8A - 1;
    wp->prim_A0[0]->x1 = wp->val_8 + wp->val_8C + 1;
    wp->prim_A0[0]->y1 = wp->val_A + wp->val_8E - 1;
    wp->prim_A0[0]->x2 = wp->val_8 + wp->val_88;
    wp->prim_A0[0]->y2 = wp->val_A + wp->val_8A;
    wp->prim_A0[0]->x3 = wp->val_8 + wp->val_8C;
    wp->prim_A0[0]->y3 = wp->val_A + wp->val_8E;
    wp->prim_A0[1]->x0 = wp->val_8 + wp->val_90;
    wp->prim_A0[1]->y0 = wp->val_A + wp->val_92;
    wp->prim_A0[1]->x1 = wp->val_8 + wp->val_94;
    wp->prim_A0[1]->y1 = wp->val_A + wp->val_96;
    wp->prim_A0[1]->x2 = wp->val_8 + wp->val_90 - 1;
    wp->prim_A0[1]->y2 = wp->val_A + wp->val_92 + 1;
    wp->prim_A0[1]->x3 = wp->val_8 + wp->val_94 + 1;
    wp->prim_A0[1]->y3 = wp->val_A + wp->val_96 + 1;
    wp->prim_A0[2]->x0 = wp->val_8 + wp->val_88 - 1;
    wp->prim_A0[2]->y0 = wp->val_A + wp->val_8A - 1;
    wp->prim_A0[2]->x1 = wp->val_8 + wp->val_88;
    wp->prim_A0[2]->y1 = wp->val_A + wp->val_8A;
    wp->prim_A0[2]->x2 = wp->val_8 + wp->val_90 - 1;
    wp->prim_A0[2]->y2 = wp->val_A + wp->val_92 + 1;
    wp->prim_A0[2]->x3 = wp->val_8 + wp->val_90;
    wp->prim_A0[2]->y3 = wp->val_A + wp->val_92;
    wp->prim_A0[3]->x0 = wp->val_8 + wp->val_8C;
    wp->prim_A0[3]->y0 = wp->val_A + wp->val_8E;
    wp->prim_A0[3]->x1 = wp->val_8 + wp->val_8C + 1;
    wp->prim_A0[3]->y1 = wp->val_A + wp->val_8E - 1;
    wp->prim_A0[3]->x2 = wp->val_8 + wp->val_94;
    wp->prim_A0[3]->y2 = wp->val_A + wp->val_96;
    wp->prim_A0[3]->x3 = wp->val_8 + wp->val_94 + 1;
    wp->prim_A0[3]->y3 = wp->val_A + wp->val_96 + 1;
    addPrim(&D_80098934->ot[(sint8)wp->val_3D], wp->prim_A0[0]);
    addPrim(&D_80098934->ot[(sint8)wp->val_3D], wp->prim_A0[1]);
    addPrim(&D_80098934->ot[(sint8)wp->val_3D], wp->prim_A0[2]);
    addPrim(&D_80098934->ot[(sint8)wp->val_3D], wp->prim_A0[3]);
    wp->prim_98->x0 = wp->val_8 + wp->val_88;
    wp->prim_98->y0 = wp->val_A + wp->val_8A;
    wp->prim_98->x1 = wp->val_8 + wp->val_8C;
    wp->prim_98->y1 = wp->val_A + wp->val_8E;
    wp->prim_98->x2 = wp->val_8 + wp->val_90;
    wp->prim_98->y2 = wp->val_A + wp->val_92;
    wp->prim_98->x3 = wp->val_8 + wp->val_94;
    wp->prim_98->y3 = wp->val_A + wp->val_96;
    addPrim(&D_80098934->ot[(sint8)wp->val_3D], wp->prim_98);
    mode                    = *(DR_MODE **)0x1F800070;
    *(DR_MODE **)0x1F800070 = mode + 1;
    SetDrawMode(mode, 0, 0, GetTPage(0, 0, 0x3C0, 0x100), NULL);
    addPrim(&D_80098934->ot[(sint8)wp->val_3D], mode);
    wp->prim_9C->x0 = wp->val_8 + wp->val_88;
    wp->prim_9C->y0 = wp->val_A + wp->val_8A;
    wp->prim_9C->x1 = wp->val_8 + wp->val_8C;
    wp->prim_9C->y1 = wp->val_A + wp->val_8E;
    wp->prim_9C->x2 = wp->val_8 + wp->val_90;
    wp->prim_9C->y2 = wp->val_A + wp->val_92;
    wp->prim_9C->x3 = wp->val_8 + wp->val_94;
    wp->prim_9C->y3 = wp->val_A + wp->val_96;
    addPrim(&D_80098934->ot[(sint8)wp->val_3D], wp->prim_9C);
    mode2                   = *(DR_MODE **)0x1F800070;
    *(DR_MODE **)0x1F800070 = mode2 + 1;
    SetDrawMode(mode2, 0, 0, GetTPage(0, 2, 0x3C0, 0x100), NULL);
    addPrim(&D_80098934->ot[(sint8)wp->val_3D], mode2);
}

void func_8005B454(MOJI_WORK *wp)
{
    switch (wp->val_7D)
    {
        case 0:
            wp->val_8C = 0x140;
            wp->val_92 = 0xF0;
            wp->val_94 = 0x140;
            wp->val_96 = 0xF0;
            wp->val_B0 = 0;
            wp->val_B1 = 0;
            wp->val_B2 = 0;
            wp->val_88 = 0;
            wp->val_8A = 0;
            wp->val_8E = 0;
            wp->val_90 = 0;
            wp->flags_0 |= 0x14000000;
            wp->val_80 = wp->flags_0 & 0x08000000;
            wp->val_7D++;
            wp->flags_0 |= 0x08000000;
            break;
        case 1:
            wp->flags_0 |= 0x10000000;
            wp->val_B0 += 8;
            wp->val_B1 += 8;
            wp->val_B2 += 8;
            if (wp->val_B0 == 0x80)
            {
                wp->flags_0 |= 0x12000000;
                wp->flags_0 = (wp->flags_0 & 0xF7FFFFFF) | wp->val_80;
                wp->val_7D++;
                wp->pos_6C += 3;
            }
            break;
        case 2:
            wp->flags_0 |= 0x10000000;
            wp->val_B0 -= 8;
            wp->val_B1 -= 8;
            wp->val_B2 -= 8;
            if (wp->val_B0 == 0)
            {
                wp->flags_0 &= 0xF9FFFFFF;
            }
            break;
    }
}

void func_8005B5D8(MOJI_WORK *wp)
{
    DR_MODE *mode;
    DR_MODE *mode2;
    POLY_F4 *f4;

    f4                      = *(POLY_F4 **)0x1F800070;
    *(POLY_F4 **)0x1F800070 = f4 + 1;
    wp->prim_98             = f4;
    setPolyF4(wp->prim_98);
    wp->prim_98->code |= 2;
    wp->prim_98->r0         = wp->val_B0;
    wp->prim_98->g0         = wp->val_B1;
    wp->prim_98->b0         = wp->val_B2;
    mode                    = *(DR_MODE **)0x1F800070;
    *(DR_MODE **)0x1F800070 = mode + 1;
    SetDrawMode(mode, 0, 0, GetTPage(0, 0, 0x3C0, 0x100), NULL);
    addPrim(&D_80098934->ot[(sint8)wp->val_3D], mode);
    wp->prim_98->x0 = wp->val_88;
    wp->prim_98->y0 = wp->val_8A;
    wp->prim_98->x1 = wp->val_8C;
    wp->prim_98->y1 = wp->val_8E;
    wp->prim_98->x2 = wp->val_90;
    wp->prim_98->y2 = wp->val_92;
    wp->prim_98->x3 = wp->val_94;
    wp->prim_98->y3 = wp->val_96;
    addPrim(&D_80098934->ot[(sint8)wp->val_3D], wp->prim_98);
    mode2                   = *(DR_MODE **)0x1F800070;
    *(DR_MODE **)0x1F800070 = mode2 + 1;
    SetDrawMode(mode2, 0, 0, GetTPage(0, 2, 0x3C0, 0x100), NULL);
    addPrim(&D_80098934->ot[(sint8)wp->val_3D], mode2);
}

// The flags store comes first, so GCC reloads the counter rather than folding the switch value.
void func_8005B85C(MOJI_WORK *wp)
{
    uint8 size;

    switch (wp->val_7D)
    {
        case 0:
            wp->flags_0 |= 0x6000000;
            wp->val_7D += 2;
            size       = wp->pos_6C[2];
            wp->val_B4 = wp->val_B6 = size;
            Moji_flag |= 0x200000;
            wp->pos_6C += 3;
            wp->val_88 = -wp->val_B4;
            wp->val_8A = -wp->val_B6;
            wp->val_8C = wp->val_B4 + wp->val_7E * 12;
            wp->val_8E = -wp->val_B6;
            wp->val_90 = -wp->val_B4;
            wp->val_92 = wp->val_B6 + wp->val_7F * 12;
            wp->val_94 = wp->val_B4 + wp->val_7E * 12;
            wp->val_96 = wp->val_B6 + wp->val_7F * 12;
            break;
        case 2:
            wp->flags_0 = (wp->flags_0 | 0x10000000) & ~0x6000000;
            break;
    }
}

// The edge tests are written with the moving edge on the left; the reset writes the edges in
// field order.
void func_8005B98C(MOJI_WORK *wp)
{
    uint32 size;

    switch (wp->val_7D)
    {
        case 0:
            wp->val_85 = 8;
            wp->flags_0 |= 0x14000000;
            wp->val_80 = wp->flags_0 & 0x08000000;
            wp->flags_0 |= 0x08000000;
            size = wp->pos_6C[2];
            wp->val_7D++;
            wp->val_8A = wp->val_7F * 6;
            wp->val_B4 = size;
            wp->val_8E = wp->val_7F * 6;
            wp->val_92 = wp->val_7F * 6;
            wp->val_96 = wp->val_7F * 6;
            wp->val_88 = -wp->val_B4;
            wp->val_B6 = size;
            wp->val_8C = wp->val_B4 + wp->val_7E * 12;
            wp->val_90 = -wp->val_B4;
            wp->val_94 = wp->val_B4 + wp->val_7E * 12;
            break;
        case 1:
            wp->flags_0 |= 0x10000000;
            wp->val_8A -= wp->val_85;
            wp->val_8E -= wp->val_85;
            wp->val_92 += wp->val_85;
            wp->val_96 += wp->val_85;
            if (wp->val_A + wp->val_8A <= wp->val_A - wp->val_B6)
            {
                Moji_flag |= 0x200000;
                wp->val_88 = -wp->val_B4;
                wp->val_8A = -wp->val_B6;
                wp->val_8C = wp->val_B4 + wp->val_7E * 12;
                wp->val_8E = -wp->val_B6;
                wp->val_90 = -wp->val_B4;
                wp->val_92 = wp->val_B6 + wp->val_7F * 12;
                wp->val_94 = wp->val_B4 + wp->val_7E * 12;
                wp->val_96 = wp->val_B6 + wp->val_7F * 12;
                wp->flags_0 |= 0x12000000;
                wp->flags_0 = (wp->flags_0 & ~0x08000000) | wp->val_80;
                wp->val_7D++;
                wp->pos_6C += 3;
            }
            break;
        case 2:
            wp->flags_0 |= 0x10000000;
            wp->val_8A += wp->val_85;
            wp->val_8E += wp->val_85;
            wp->val_92 -= wp->val_85;
            wp->val_96 -= wp->val_85;
            if (wp->val_8A > wp->val_92)
            {
                wp->flags_0 &= ~0x06000000;
            }
            break;
    }
}

void func_8005BC90(MOJI_WORK *wp)
{
    if (D_80098912 == 1)
    {
        wp->val_7C = 0;
    }
    else
    {
        wp->val_7C = 4;
    }
    func_8005ACA8(wp);
    wp->val_7C = 7;
}

// The G4 loop indexes the shade tables with (i << 2), which keeps loop.c's count low enough to
// hoist the len/code constants and walk four table pointers, as retail does; the colour table is
// uint8[][6] (a flat index * 6 + k leaves orphan address pseudos that take stack slots).
void func_8005BCE4(MOJI_WORK *wp, sint32 index)
{
    POLY_F4 *f4;
    POLY_G4 *g4;
    sint32 i;

    f4                      = *(POLY_F4 **)0x1F800070;
    *(POLY_F4 **)0x1F800070 = f4 + 1;
    wp->prim_98             = f4;
    *(POLY_F4 **)0x1F800070 = f4 + 2;
    wp->prim_9C             = f4 + 1;
    setPolyF4(wp->prim_98);
    wp->prim_98->code |= 2;
    wp->prim_98->r0 = D_80097F6C[index][0];
    wp->prim_98->g0 = D_80097F6C[index][1];
    wp->prim_98->b0 = D_80097F6C[index][2];
    setPolyF4(wp->prim_9C);
    wp->prim_9C->code |= 2;
    wp->prim_9C->r0 = D_80097F6C[index][3];
    wp->prim_9C->g0 = D_80097F6C[index][4];
    wp->prim_9C->b0 = D_80097F6C[index][5];
    for (i = 0; i < 4; i++)
    {
        g4                      = *(POLY_G4 **)0x1F800070;
        *(POLY_G4 **)0x1F800070 = g4 + 1;
        wp->prim_A0[i]          = g4;
        setPolyG4(wp->prim_A0[i]);
        wp->prim_A0[i]->r0 = D_80097F7B[(i << 2) - 3];
        wp->prim_A0[i]->g0 = D_80097F7B[(i << 2) - 3];
        wp->prim_A0[i]->b0 = D_80097F7B[(i << 2) - 3];
        wp->prim_A0[i]->r1 = D_80097F79[i << 2];
        wp->prim_A0[i]->g1 = D_80097F79[i << 2];
        wp->prim_A0[i]->b1 = D_80097F79[i << 2];
        wp->prim_A0[i]->r2 = D_80097F7A[i << 2];
        wp->prim_A0[i]->g2 = D_80097F7A[i << 2];
        wp->prim_A0[i]->b2 = D_80097F7A[i << 2];
        wp->prim_A0[i]->r3 = D_80097F7B[i << 2];
        wp->prim_A0[i]->g3 = D_80097F7B[i << 2];
        wp->prim_A0[i]->b3 = D_80097F7B[i << 2];
    }
}

void num_to_text(sint32 mode, uint32 value, uint8 *out)
{
    uint32 i;
    uint32 digit;

    for (i = 0; i < 6; i++)
    {
        digit = value / D_8008AF24[i];
        if (D_800986F0 == 1 || digit != 0)
        {
            out[i]     = D_8008D028[digit];
            D_800986F0 = 1;
        }
        else
        {
            out[i] = D_8008AF3C[mode];
        }
        value %= D_8008AF24[i];
    }
    D_800986F0 = 0;
    out[6]     = D_8008D028[0];
}

// Frame count as a time string. One index variable serves every digit, as retail's a0.
void time_to_text(uint32 frames, uint8 *out)
{
    uint32 sec;
    uint32 idx;

    sec    = frames / 30;
    idx    = (frames % 3) * 3;
    out[6] = D_8008D028[idx];
    idx    = (frames % 30) / 3;
    out[5] = D_8008D028[idx];
    idx    = sec % 10;
    out[3] = D_8008D028[idx];
    idx    = (sec % 60) / 10;
    out[2] = D_8008D028[idx];
    idx    = (sec / 60) % 10;
    out[0] = D_8008D028[idx];
}

void num2_to_text(uint32 value, uint8 *out)
{
    out[0] = D_8008D028[value / 10];
    out[1] = D_8008D028[value % 10];
    if (out[0] == D_8008D028[0])
    {
        out[0] = 0x4F;
    }
}

void Moji_param_exec(sint32 index, sint32 mode, sint32 value, sint32 id)
{
    switch ((uint32)mode)
    {
        case 0:
            Moji_item = value;
            MojiTaskExec(id, D_8008CAD0[D_8008AF40[index]], 0xFF);
            break;
        case 1:
            Moji_amount = value;
            MojiTaskExec(id, D_8008CAD4[D_8008AF40[index]], 0xFF);
            break;
        case 2:
            MojiTaskExec(id, D_8008CACC[D_8008AF40[index]], 0xFF);
            break;
    }
}

void Moji_item_set(sint32 value)
{
    Moji_item = value;
}

void Moji_amount_set(sint32 value)
{
    Moji_amount = value;
}

void Moji_time_set(uint32 value)
{
    Moji_time = value;
}

void Moji_num2_set(uint32 value)
{
    Moji_num2 = value;
}

void func_8005C24C(sint32 value)
{
    D_800989D4 = value;
}
