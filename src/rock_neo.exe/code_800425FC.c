#include "common.h"
#include "rock_neo.exe/code_800425fc.h"

void Pl_key_type_set(PL_KEY_VIEW *wp, sint32 idx)
{
    uint32 i;

    for (i = 0; i < 16; i++)
    {
        wp->key[i] = Pl_key_type_tbl[idx][i];
    }
    wp->pad[0]  = wp->key[0] | wp->key[1];
    wp->pad[1]  = wp->key[0];
    wp->pad[2]  = wp->key[4] | wp->key[5];
    wp->pad[3]  = wp->key[5];
    wp->pad[4]  = wp->key[4] | wp->key[5];
    wp->pad[5]  = wp->key[4];
    wp->pad[6]  = wp->key[0] | wp->key[1];
    wp->pad[7]  = wp->key[0];
    wp->pad[8]  = wp->key[0] | wp->key[1];
    wp->pad[9]  = wp->key[1];
    wp->pad[10] = wp->key[4] | wp->key[5];
    wp->pad[11] = wp->key[4];
    wp->pad[12] = wp->key[4] | wp->key[5];
    wp->pad[13] = wp->key[5];
    wp->pad[14] = wp->key[0] | wp->key[1];
    wp->pad[15] = wp->key[1];
}

void Pl_shot_special(C800425FC_WORK *wp)
{
    Pl_shot_special_tbl[wp->routine_170](wp);
}

void Pl_shot_buster(C800425FC_WORK *wp)
{
    Pl_shot_buster_tbl[wp->routine_171](wp);
}

void Pl_special_std(C800425FC_ACT *ap)
{
    sint32 act;
    uint16 pad;
    uint8 dir;

    act = ap->routine_170;
    pad = ap->h11c;
    switch ((sint8)ap->b178)
    {
        case 0:
            if (ap->b100 == D_80089A84[act * 12])
            {
                ap->b178 += 2;
                ap->b174 |= 0x10;
            }
            break;
        case 1:
            if (!(ap->b174 & 4))
            {
                Pl_Char_Set(ap, ap->b0ac, D_80089A84[act * 12]);
            }
            dir      = Pl_anim_special(ap);
            ap->b104 = dir;
            Pl_Char_Set_Upper(ap, ap->b174 & 3, dir, D_80089A84[act * 12]);
            ap->b178++;
            ap->b174 |= 0x10;
            break;
        case 2:
            ap->b174 &= 0xCF;
            if (D_80089A87[act * 12] != 0 && ap->parts[act].h06 > 0)
            {
                if (ap->b100 != D_80089A85[act * 12] + func_80037664(ap, act))
                {
                    break;
                }
                ap->b178++;
            }
            else
            {
                ap->b178 += 2;
                break;
            }
            // fall through
        case 3:
            if (ap->b100 == D_80089A86[act * 12])
            {
                ap->b178++;
            }
            else if ((ap->h13c & pad) && (ap->b175 & 3) == 3)
            {
                ap->b174 = 0;
                ap->b177 = 2;
            }
            break;
        case 4:
            if (ap->s103 == -1)
            {
                ap->b174 = 0;
                ap->b177 = 0;
                if (act == 0)
                {
                    ap->h44.u = 0;
                }
            }
            break;
    }
    if (act == 0)
    {
        if (Game_work[0x83] == 0)
        {
            if (ap->w198 != 0)
            {
                ap->h44.s = -0x300;
            }
            else
            {
                ap->h44.u += 0x80;
                if (ap->h44.s > 0)
                {
                    ap->h44.u = 0;
                }
            }
            Pl_move(ap, ap->h44.s, 0, 0);
        }
        else
        {
            ap->h44.u = 0;
        }
    }
}

void func_80042A40(C800425FC_ACT *ap)
{
    sint32 act;

    act = ap->routine_170;
    switch ((sint8)ap->b178)
    {
        case 0:
            if (ap->b100 != D_80089A84[act * 12])
            {
                ap->h44.u = 0;
            }
            else
            {
                ap->b178 = ap->b178 + 2;
                ap->b174 |= 0x10;
                if (Game_work[0x83] == 1)
                {
                    ap->h44.u = 0;
                }
                else
                {
                    ap->h44.s = -0x300;
                }
            }
            break;
        case 1:
        case 2:
            ap->b174 &= 0xCF;
            ap->b178++;
            break;
        case 3:
            if (ap->s103 == -1)
            {
                ap->b174 = 0;
                ap->b177 = 0;
            }
            break;
    }
    ap->h44.u += 0xE0;
    if (ap->h44.s < 0)
    {
        Pl_move(ap, ap->h44.s, 0, 0);
        return;
    }
    ap->h44.u = 0;
}

void Pl_special_hold(C800425FC_ACT *ap)
{
    sint32 act;
    uint16 pad;
    sint8 hit;

    act = ap->routine_170;
    pad = ap->h11c;
    switch ((sint8)ap->b178)
    {
        case 0:
            if (ap->b100 == D_80089A84[act * 12])
            {
                ap->b178 += 2;
                ap->b174 |= 0x10;
            }
            break;
        case 1:
        case 2:
            ap->b174 &= 0xCF;
            if (ap->b100 >= D_80089A86[act * 12])
            {
                // The second read of each table byte goes through the base symbol;
                // the same spelling twice lets GCC keep the whole address live.
                Pl_Char_Set(ap, ap->b0ac, D_80089A85[act * 12]);
                Pl_Char_Set_Upper(ap, ap->b174 & 3, ap->b104, D_80089A84[act * 12 + 1]);
            }
            if (!(pad & ap->h13c) || ap->parts[act].h06 <= 0)
            {
                ap->b110 = 0;
                ap->b178++;
            }
            break;
        case 3:
            Pl_Char_Set(ap, ap->b0ac, D_80089A86[act * 12]);
            Pl_Char_Set_Upper(ap, ap->b174 & 3, ap->b104, D_80089A84[act * 12 + 2]);
            ap->b110 = 0;
            ap->b178++;
            break;
        case 4:
            if (ap->s103 == -1)
            {
                ap->b174 = 0;
                ap->b177 = 0;
                ap->b110 = 0;
            }
            break;
    }
    if (act == 12)
    {
        if (Game_work[0x83] == 0)
        {
            hit = ap->s44c;
            if (hit != 0)
            {
                if (hit & 2)
                {
                    ap->h44.s = -0x280;
                }
                if (ap->s44c & 1)
                {
                    ap->b110 = -1;
                }
                ap->s44c = 0;
            }
            if (ap->b009 == 0 || ap->b009 == 4 || ap->b009 == 14)
            {
                ap->h44.u += 0x80;
                if (ap->h44.s > 0)
                {
                    ap->h44.u = 0;
                    ap->b110  = 0;
                }
            }
            Pl_move(ap, ap->h44.s, 0, 0);
        }
        else
        {
            ap->h44.u = 0;
        }
    }
    if (ap->s074 != 0)
    {
        Pl00_shot_enable_off(ap);
        ap->b110 = 0;
    }
}

void func_80042DF8(C800425FC_ACT *ap)
{
    sint32 act;

    act = ap->routine_170;
    switch ((sint8)ap->b178)
    {
        case 0:
            if (ap->b100 == D_80089A84[act * 12])
            {
                ap->b178 = ap->b178 + 1;
                ap->b174 |= 0x10;
            }
            if (Game_work[0x83] != 1)
            {
                ap->h44.u = 0x90;
            }
            else
            {
                ap->h44.u = 0;
            }
            break;
        case 1:
            ap->b174 &= 0xCF;
            if (ap->b100 == D_80089A85[act * 12])
            {
                ap->h44.u = 0;
                ap->b178++;
            }
            break;
        case 2:
            if (ap->b100 >= D_80089A86[act * 12])
            {
                if (Game_work[0x83] == 1)
                {
                    ap->h44.u = 0;
                }
                else
                {
                    ap->h44.u = 0x60;
                }
                ap->b178++;
            }
            break;
        case 3:
            if (ap->s103 == -1)
            {
                ap->b174  = 0;
                ap->b177  = 0;
                ap->h44.u = 0;
            }
            break;
    }
    Pl_move(ap, ap->h44.s, 0, 0);
}

void Pl_buster_std(C800425FC_ACT *ap)
{
    sint32 act;
    uint16 pad;
    uint8 dir;

    act = ap->routine_171;
    pad = ap->h11c;
    switch ((sint8)ap->b178)
    {
        case 0:
            if (ap->b100 == D_80089A84[act * 12])
            {
                ap->b178 += 2;
                ap->b174 |= 0x20;
            }
            break;
        case 1:
            if (!(ap->b174 & 4))
            {
                Pl_Char_Set(ap, ap->b0ac, D_80089A84[act * 12]);
            }
            dir      = Pl_anim_buster(ap);
            ap->b104 = dir;
            Pl_Char_Set_Upper(ap, ap->b174 & 3, dir, D_80089A84[act * 12]);
            ap->b178++;
            ap->b174 |= 0x20;
            break;
        case 2:
            ap->b174 &= 0xCF;
            if (D_80089A87[act * 12] != 0 && ap->parts[act].h06 > 0)
            {
                if (ap->b100 != D_80089A85[act * 12] + func_80037664(ap, act))
                {
                    break;
                }
                ap->b178++;
            }
            else
            {
                ap->b178 += 2;
                break;
            }
            // fall through
        case 3:
            if (ap->b100 == D_80089A86[act * 12])
            {
                ap->b178++;
            }
            else if ((pad & ap->h13a) && (ap->b175 & 5) == 5)
            {
                ap->b174 = 0;
                ap->b177 = 1;
            }
            break;
        case 4:
            if (ap->s103 == -1)
            {
                ap->b174 = 0;
                ap->b177 = 0;
                if (act == 0)
                {
                    ap->h44.u = 0;
                }
            }
            break;
    }
    if (act == 0)
    {
        if (Game_work[0x83] == 0)
        {
            if (ap->w198 != 0)
            {
                ap->w198  = 0;
                ap->h44.s = -0x300;
            }
            else
            {
                ap->h44.u += 0x80;
                if (ap->h44.s > 0)
                {
                    ap->h44.u = 0;
                }
            }
            Pl_move(ap, ap->h44.s, 0, 0);
        }
        else
        {
            ap->h44.u = 0;
        }
    }
}
