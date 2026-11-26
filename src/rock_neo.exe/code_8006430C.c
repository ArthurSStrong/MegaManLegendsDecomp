#include "common.h"
#include "code_8006430c.h"

// Small data reached through $gp
sint32 D_80098708;
sint32 D_8009870C;
sint32 D_80098710;
sint32 D_80098714;
sint32 D_80098718;
sint32 D_8009871C;
sint32 D_80098720;
sint32 D_80098724;
sint16 D_80098728;
uint16 D_80098910;
void **Em_param_tbl;
void **Work_5A4_param_tbl;
void **Shl_param_tbl;
void **Work_50_param_tbl;
uint8 D_80098A04;
uint8 D_80098A48;

// Status-screen icon sprites. `id` as a sint32 loaded with a (sint16) cast gives retail's early
// sll 16; storing the first packet's 0x02000000 tag before its draw-mode word fixes the
// prologue. The icon position is assigned to x and y inside the xy expression: as separate
// temporaries GCC's fold can't move the 0x22 / 0x3C onto the slot terms, which gives retail's
// x + 0x22 / y + 0x3C adds, the slot / 0x64808080 registers and the load placement.
// FAKE MATCH: the loop condition stores an unused result in `id`, only for the stack frame.
// The loop condition assigns its result to `id` (`id = (sint16)icon->id >= 0`, as written: the
// comparison binds tighter than the assignment). Behaviour is unchanged, since `id` is reloaded
// at the top of the body. GCC expands the stored comparison as `id = ~v; id >>= 31;` and combine
// folds both into the branch, but it never clears the reference count of a register whose
// folded set read its own old value (combine.c, i2dest_in_i2src). `id` is left with references
// and no instructions, so reload gives it an unused 8-byte stack slot. Declared between the
// parameter and `head`, that slot is retail's gap: set 0x10, id 0x18, head 0x20 (reload hands
// out slots in pseudo-number order). Found by decomp-permuter with --stack-diffs.
void func_8006430C(CODE643_ICON_SET *set)
{
    CODE643_ICON *icon;
    CODE643_PRIM *prim;
    sint32 id;
    CODE643_PRIM *head;
    uint32 kind;
    sint32 slot;
    sint32 flag;
    sint32 first;
    sint32 count;
    uint32 value;
    sint32 x;
    sint32 y;

    value          = 0;
    icon           = set->icons;
    prim           = CODE643_PRIM_PTR;
    prim->tag      = 0x02000000;
    prim->code_rgb = 0xE100000C;
    prim->xy       = 0;
    prim->tag      = ((u_long)(prim + 1) & 0xFFFFFF) | 0x02000000;
    head           = prim;
    prim++;
    do
    {
        flag           = icon->flag;
        slot           = icon->id & 0xFF;
        id             = (sint16)icon->id;
        prim->tag      = ((u_long)(prim + 1) & 0xFFFFFF) | 0x04000000;
        prim->code_rgb = 0x64808080;
        prim->xy       = ((slot % 6) * 44 + (x = icon->x + 0x22)) |
                   (((slot / 6) * 40 + (y = icon->y + 0x3C)) << 16);
        prim->wh = icon->wh;
        first    = Sce_flag_test(flag);
        count    = first + Sce_flag_test(flag + 1) + Sce_flag_test(flag + 2);
        kind     = id >> 8;
        switch (kind)
        {
            case 0:
            case 13:
                if (first != 0)
                {
                    prim->uv_clut = icon->uv_clut;
                    prim++;
                }
                else if (kind == 0)
                {
                    prim->uv_clut = 0x3C484080;
                    prim++;
                }
                break;
            case 1:
                if (count != 0)
                {
                    prim->uv_clut = icon->uv_clut;
                }
                else
                {
                    prim->uv_clut = 0x3C484080;
                }
                prim++;
                break;
            case 2:
                if (count != 0)
                {
                    prim->uv_clut = icon->uv_clut;
                    prim++;
                }
                break;
            case 3:
                if (count != 0)
                {
                    prim->uv_clut = icon->uv_clut + (count - 1) * 8;
                    prim++;
                }
                break;
            case 4:
                if (count != 0)
                {
                    prim->uv_clut = icon->uv_clut + (count - 1) * 32;
                }
                else
                {
                    prim->uv_clut = 0x3C484080;
                }
                prim++;
                break;
            case 5:
            case 7:
                if (kind == 5)
                {
                    value = Game_work.flags_7c;
                }
                else
                {
                    value = Game_work.flags_7d;
                }
                if (first != 0 && value >= 10)
                {
                    prim->uv_clut = icon->uv_clut + (value / 10) * 8;
                    prim++;
                }
                break;
            case 6:
            case 8:
                if (kind == 6)
                {
                    value = Game_work.flags_7c;
                }
                else
                {
                    value = Game_work.flags_7d;
                }
                if (first != 0)
                {
                    prim->uv_clut = icon->uv_clut + (value % 10) * 8;
                    prim++;
                }
                break;
            case 9:
            case 10:
            case 11:
            case 12:
                switch (kind)
                {
                    case 9:
                        value = *(sint8 *)&Player_work.reserved16a;
                        break;
                    case 10:
                        value = Player_work.flags16b & 2;
                        break;
                    case 11:
                        value = Player_work.flags16b & 1;
                        break;
                    case 12:
                        value = (sint8)Player_work.reserved16c[0];
                        break;
                }
                if (first != 0 && value != 0)
                {
                    prim->uv_clut = icon->uv_clut;
                    prim++;
                }
                break;
        }
        icon++;
    } while (id = (sint16)icon->id >= 0);
    setaddr(&prim[-1], getaddr(&Disp_env_ptr->layer[set->layer]));
    setaddr(&Disp_env_ptr->layer[set->layer], head);
    CODE643_PRIM_PTR = prim;
}

void Param_tbl_select(void)
{
    if ((sint8)MEM_GWB(0x5B) == 2)
    {
        Em_param_tbl       = D_8008F060;
        Work_5A4_param_tbl = D_8008F200;
        Shl_param_tbl      = D_8008F260;
        Work_50_param_tbl  = D_8008F2E4;
    }
    else
    {
        Em_param_tbl       = D_8008EDC4;
        Work_5A4_param_tbl = D_8008EF64;
        Shl_param_tbl      = D_8008EFC4;
        Work_50_param_tbl  = D_8008F048;
    }
}

void Card_init(void)
{
    func_8007FF70();
    D_80098708 = OpenEvent(SW_CARD, EV_SP_IOE, EV_MD_NOINTR, NULL);
    D_8009870C = OpenEvent(SW_CARD, EV_SP_ERROR, EV_MD_NOINTR, NULL);
    D_80098710 = OpenEvent(SW_CARD, EV_SP_TIMOUT, EV_MD_NOINTR, NULL);
    D_80098714 = OpenEvent(SW_CARD, EV_SP_NEW, EV_MD_NOINTR, NULL);
    D_80098718 = OpenEvent(HW_CARD, EV_SP_IOE, EV_MD_NOINTR, NULL);
    D_8009871C = OpenEvent(HW_CARD, EV_SP_ERROR, EV_MD_NOINTR, NULL);
    D_80098720 = OpenEvent(HW_CARD, EV_SP_TIMOUT, EV_MD_NOINTR, NULL);
    D_80098724 = OpenEvent(HW_CARD, EV_SP_NEW, EV_MD_NOINTR, NULL);
    func_8007FF80();
    EnableEvent(D_80098708);
    EnableEvent(D_8009870C);
    EnableEvent(D_80098710);
    EnableEvent(D_80098714);
    EnableEvent(D_80098718);
    EnableEvent(D_8009871C);
    EnableEvent(D_80098720);
    EnableEvent(D_80098724);
    func_8007FC88(1);
    func_8007FCE0();
    _bu_init();
}

sint32 card_sw_event_wait(void)
{
    for (;;)
    {
        if (TestEvent(D_80098708) != 0)
        {
            return 0;
        }
        if (TestEvent(D_80098710) != 0)
        {
            return 1;
        }
        if (TestEvent(D_80098714) != 0)
        {
            return 2;
        }
        if (TestEvent(D_8009870C) != 0)
        {
            return 3;
        }
    }
}

sint32 card_hw_event_wait(void)
{
    for (;;)
    {
        if (TestEvent(D_80098718) != 0)
        {
            return 0;
        }
        if (TestEvent(D_80098720) != 0)
        {
            return 1;
        }
        if (TestEvent(D_8009871C) != 0)
        {
            return 2;
        }
        if (TestEvent(D_80098724) != 0)
        {
            return 3;
        }
    }
}

void card_sw_event_clear(void)
{
    TestEvent(D_80098708);
    TestEvent(D_80098710);
    TestEvent(D_8009870C);
    TestEvent(D_80098714);
}

void card_hw_event_clear(void)
{
    TestEvent(D_80098718);
    TestEvent(D_80098720);
    TestEvent(D_8009871C);
    TestEvent(D_80098724);
}

sint32 card_check(sint32 port)
{
    sint32 r;

    VSync(0);
retry:
    card_sw_event_clear();
    _card_info(port << 4);
    r = card_sw_event_wait();
    if (r == 3)
    {
        return 3;
    }
    if (r == 1)
    {
        return 1;
    }
    if (r == 2)
    {
        card_hw_event_clear();
        if (_card_clear(port << 4) != 1)
        {
            goto retry;
        }
        if (card_hw_event_wait() != 0)
        {
            return 3;
        }
    }
    card_sw_event_clear();
    _card_load(port << 4);
    VSync(0x48);
    r = card_sw_event_wait();
    if (r == 2)
    {
        return 4;
    }
    return (r != 0) ? 3 : 0;
}

sint32 card_info_get(sint32 port)
{
    sint32 r;

    _card_info(port << 4);
    r = card_sw_event_wait();
    if (r == 1)
    {
        return 0;
    }
    if (r == 2 || r == 0)
    {
        return 1;
    }
    return -1;
}

sint32 card_info_get2(sint32 port)
{
    sint32 r;

    _card_info(port << 4);
    r = card_sw_event_wait();
    if (r == 1)
    {
        return 0;
    }
    if (r == 0)
    {
        return 1;
    }
    if (r == 2)
    {
        return 2;
    }
    return -1;
}

uint32 card_free_blocks(sint32 port)
{
    struct DIRENTRY dir;
    char path[24];
    uint32 free;

    free = 0x1E000;
    if (port == 0)
    {
        strcpy(path, "bu00:*");
    }
    else
    {
        strcpy(path, "bu10:*");
    }
    if (firstfile(path, &dir) == &dir)
    {
        free -= dir.size;
        while (nextfile(&dir) == &dir)
        {
            free -= dir.size;
        }
    }
    return free / 0x2000;
}

void card_header_set(uint8 *buf, uint8 *src)
{
    sint32 i;
    uint8 *dst;
    uint8 *tbl;
    uint8 *dst2;

    buf[0] = 0x53;
    buf[1] = 0x43;
    buf[2] = 0x11;
    buf[3] = 1;
    dst    = buf + 4;
    for (i = 0; i < 0x40; i++)
    {
        *dst++ = *src++;
    }
    dst = buf + 0x44;
    for (i = 0; i < 0x1C; i++)
    {
        *dst++ = 0;
    }
    tbl  = D_8008F9BC;
    dst2 = buf + 0x80;
    for (i = 0; i < 0x80; i++)
    {
        *dst2++ = *tbl++;
    }
    tbl  = D_8008FA3C;
    dst2 = buf + 0x60;
    for (i = 0; i < 0x20; i++)
    {
        *dst2++ = *tbl++;
    }
}

sint32 card_file_write(sint32 port, char *name, void *buf, uint32 size)
{
    char path[0x20];
    sint32 fd;
    sint32 r;
    uint32 free_blocks;
    uint32 blocks;

    r = card_check(port);
    if (r != 0)
    {
        return r;
    }
    blocks      = (size + 0x1FFF) >> 13;
    free_blocks = card_free_blocks(port);
    strcpy(path, D_8008FA5C[port]);
    strcat(path, name);
    fd = open(path, 2);
    if (fd == -1)
    {
        fd = open(path, (blocks << 16) | 0x200);
        if (fd == -1)
        {
            if (free_blocks < blocks)
            {
                return 2;
            }
            return -1;
        }
        if (close(fd) == -1)
        {
            return -1;
        }
        fd = open(path, 2);
        if (fd == -1)
        {
            return -1;
        }
    }
    if (write(fd, buf, size) == -1)
    {
        return -1;
    }
    close(fd);
    return 0;
}

sint32 card_file_read(sint32 port, char *name, void *buf, uint32 size)
{
    char path[0x20];
    sint32 fd;
    sint32 r;
    uint32 free_blocks;
    uint32 blocks;

    r = card_check(port);
    if (r != 0)
    {
        return r;
    }
    blocks      = (size + 0x1FFF) >> 13;
    free_blocks = card_free_blocks(port);
    strcpy(path, D_8008FA5C[port]);
    strcat(path, name);
    fd = open(path, 1);
    if (fd == -1)
    {
        if (free_blocks < blocks)
        {
            return 2;
        }
        return -1;
    }
    if (read(fd, buf, size) == -1)
    {
        return 3;
    }
    close(fd);
    return 0;
}

sint32 card_format(sint32 port)
{
    sint32 r;

    r = card_check(port);
    if (r == 1)
    {
        return -1;
    }
    format(D_8008FA5C[port]);
    return -(card_check(port) == 1);
}

sint32 card_new_clear(sint32 arg)
{
    D_8009AB31    = 0xFF;
    D_8009AB30[0] = 0xFF;
    card_hw_event_clear();
    _new_card();
    if (arg == 0)
    {
        _card_write(0, 0, D_8009AB30);
    }
    else
    {
        _card_write(0x10, 0, D_8009AB30);
    }
    return card_hw_event_wait() == 0;
}

void save_image_make(void)
{
    PL_WORK *pl;
    MEM_GAME_VIEW *gw;
    MEM_IMAGE *save;
    sint8 i;

    pl = &Player_work;
    gw = &Game_work;
    save_title_set();
    save = (MEM_IMAGE *)Cd_ring_buf;
    card_header_set(Cd_ring_buf, D_8008FA64);
    D_800B5EB0 = MEM_GWB(0x50);
    for (i = 0; i < 8; i++)
    {
        save->data_101[i] = (&D_80097FD4)[i];
    }
    Map_obj_state_save();
    save->gw_10       = MEM_W(gw, 0x10);
    save->gw_14       = gw->timer_14;
    save->gw_18       = MEM_H(gw, 0x18);
    save->gw_1a       = MEM_H(gw, 0x1A);
    save->gw_0c       = gw->play_time;
    save->gw_1c[0]    = gw->words_1c[0];
    save->gw_1c[1]    = gw->words_1c[1];
    save->gw_1c[2]    = gw->words_1c[2];
    save->gw_28[0]    = gw->words_1c[3];
    save->gw_28[1]    = gw->words_1c[4];
    save->gw_28[2]    = gw->words_1c[5];
    save->gw_28[3]    = gw->words_1c[6];
    save->gw_28[4]    = gw->words_1c[7];
    save->gw_28[5]    = gw->words_1c[8];
    save->gw_40[0]    = gw->halves_40[0];
    save->gw_40[1]    = gw->halves_40[1];
    save->gw_40[2]    = gw->halves_40[2];
    save->gw_40[3]    = gw->halves_40[3];
    save->gw_40[4]    = gw->halves_40[4];
    save->gw_40[5]    = gw->halves_40[5];
    save->gw_4c       = gw->word_4c;
    save->gw_50[0]    = MEM_B(gw, 0x50);
    save->gw_50[1]    = MEM_B(gw, 0x51);
    save->gw_50[2]    = MEM_B(gw, 0x52);
    save->gw_50[3]    = MEM_B(gw, 0x53);
    save->gw_60       = gw->value_60;
    save->gw_78[0]    = MEM_B(gw, 0x78);
    save->gw_78[1]    = MEM_B(gw, 0x79);
    save->gw_78[2]    = MEM_B(gw, 0x7A);
    save->gw_78[3]    = MEM_B(gw, 0x7B);
    save->gw_7c[0]    = gw->flags_7c;
    save->gw_7c[1]    = gw->flags_7d;
    save->gw_62       = gw->value_62;
    save->gw_64       = gw->word_64;
    save->gw_54[0]    = MEM_B(gw, 0x54);
    save->gw_54[1]    = MEM_B(gw, 0x55);
    save->gw_54[2]    = MEM_B(gw, 0x56);
    save->gw_58[0]    = MEM_B(gw, 0x58);
    save->gw_58[1]    = MEM_B(gw, 0x59);
    save->gw_58[2]    = MEM_B(gw, 0x5A);
    save->gw_58[3]    = MEM_B(gw, 0x5B);
    save->pl_position = *(MEM_BYTES8 *)&pl->position;
    save->pl_56       = MEM_H(pl, 0x56);
    save->pl_6e       = pl->life;
    save->pl_b0       = MEM_H(pl, 0xB0);
    save->pl_16a[0]   = MEM_B(pl, 0x16A);
    save->pl_16a[1]   = MEM_B(pl, 0x16B);
    save->pl_16a[2]   = MEM_B(pl, 0x16C);
    save->pl_170[0]   = MEM_B(pl, 0x170);
    save->pl_170[1]   = MEM_B(pl, 0x171);
    save->pl_170[2]   = MEM_B(pl, 0x172);
    save->pl_170[3]   = MEM_B(pl, 0x173);
    save->pl_44c      = MEM_B(pl, 0x44C);
    save->pl_44e[0]   = MEM_B(pl, 0x44E);
    save->pl_44e[1]   = MEM_B(pl, 0x44F);
    save->pl_450      = MEM_W(pl, 0x450);
    func_800176DC(&MEM_B(pl, 0x454), save->pl_454, 1);
    func_800176DC(&MEM_B(pl, 0x474), save->pl_474, 4);
    func_800176DC(&MEM_B(pl, 0x4F4), save->pl_4f4, 8);
    func_800176DC(Sce_flag, save->sce_flag, 8);
    func_800176DC(Map_obj_state, save->block_480, 0x70);
    func_800176DC(Automap_visited, save->block_1280, 0x50);
    func_800176A4(gw->records_84, save->records, 15);
}

void save_image_sum_set(void)
{
    uint32 sum[3];
    uint32 *save;
    uint32 i;
    uint32 w;

    save        = MEM_SAVE_WORDS;
    save[0x5C]  = (rand() << 16) | rand();
    save[0x6C]  = (rand() << 16) | rand();
    save[0x7C]  = (rand() << 16) | rand();
    save[0xDC]  = (rand() << 16) | rand();
    save[0x74C] = (rand() << 16) | rand();
    save[0x7B8] = (rand() << 16) | rand();
    MEM_SUM_BLOCK(0, 0x5C);
    save[0x5D] = save[0x5C] + sum[0];
    save[0x5E] = (save[0x5C] ^ 0xE1E1E1E1) + sum[1];
    save[0x5F] = (save[0x5C] ^ 0x57575757) + sum[2];
    MEM_SUM_BLOCK(0x60, 0x6C);
    save[0x6D] = ~save[0x6C] + sum[0];
    save[0x6E] = (save[0x6C] ^ 0x33333333) + sum[1];
    save[0x6F] = (save[0x6C] ^ 0xCCCCCCCC) + sum[2];
    MEM_SUM_BLOCK(0x70, 0x7C);
    save[0x7D] = (save[0x7C] ^ 0x69696969) + sum[0];
    save[0x7E] = (save[0x7C] ^ 0x2B2B2B2B) + sum[1];
    save[0x7F] = (save[0x7C] ^ 0xB8B8B8B8) + sum[2];
    MEM_SUM_BLOCK(0x80, 0xDC);
    save[0xDD] = save[0xDC] + sum[0];
    save[0xDE] = (save[0xDC] ^ 0x1E1E1E1E) + sum[1];
    save[0xDF] = (save[0xDC] ^ 0x75757575) + sum[2];
    MEM_SUM_BLOCK(0xE0, 0x74C);
    save[0x74D] = ~save[0x74C] + sum[0];
    save[0x74E] = (save[0x74C] ^ 0x33333333) + sum[1];
    save[0x74F] = (save[0x74C] ^ 0xCCCCCCCC) + sum[2];
    MEM_SUM_BLOCK(0x750, 0x7B8);
    save[0x7B9] = (save[0x7B8] ^ 0x96969696) + sum[0];
    save[0x7BA] = (save[0x7B8] ^ 0xB2B2B2B2) + sum[1];
    save[0x7BB] = (save[0x7B8] ^ 0x8B8B8B8B) + sum[2];
    save[0x7BD] = ((save[0x5D] + save[0x6D]) ^ save[0x5C]) +
                  ((save[0x7D] + save[0xDD]) ^ save[0x7C]) +
                  ((save[0x74D] + save[0x7B9]) ^ save[0x74C]);
    save[0x7BE] = ((save[0x5E] + save[0x6E]) ^ save[0x6C]) +
                  ((save[0x7E] + save[0xDE]) ^ save[0xDC]) +
                  ((save[0x74E] + save[0x7BA]) ^ save[0x7B8]);
    save[0x7BF] = ((save[0x5F] + save[0x6F]) ^ save[0x5C]) +
                  ((save[0x7F] + save[0xDF]) ^ save[0x6C]) +
                  ((save[0x74F] + save[0x7BB]) ^ save[0x7C]);
    save[0x7BC] = ~save[0x7BD] + (save[0x7BE] ^ 0xAAAAAAAA) + (save[0x7BF] ^ 0x55555555);
}

// Offsets via MEM_GWB because the local Game_work view does not name every byte
void Game_work_load(void)
{
    *(uint32 *)((uint8 *)&Game_work + 0x10) = D_800B5ED8;
    *(uint32 *)((uint8 *)&Game_work + 0x14) = D_800B5EDC;
    *(uint16 *)((uint8 *)&Game_work + 0x18) = D_800B5EE0;
    *(uint16 *)((uint8 *)&Game_work + 0x1A) = D_800B5EE2;
    Game_work.play_time                     = D_800B5F30;
    Game_work.words_1c[0]                   = D_800B5F34;
    Game_work.words_1c[1]                   = D_800B5F38;
    Game_work.words_1c[2]                   = D_800B5F3C;
    Game_work.words_1c[3]                   = D_800B7AB0;
    Game_work.words_1c[4]                   = D_800B7AB4;
    Game_work.words_1c[5]                   = D_800B7AB8;
    Game_work.words_1c[6]                   = D_800B7ABC;
    Game_work.words_1c[7]                   = D_800B7AC0;
    Game_work.words_1c[8]                   = D_800B7AC4;
    Game_work.halves_40[0]                  = D_800B7AC8;
    Game_work.halves_40[1]                  = D_800B7ACA;
    Game_work.halves_40[2]                  = D_800B7ACC;
    Game_work.halves_40[3]                  = D_800B7ACE;
    Game_work.halves_40[4]                  = D_800B7AD0;
    Game_work.halves_40[5]                  = D_800B7AD2;
    Game_work.word_4c                       = D_800B5F40;
    D_800C356E                              = D_800B5F44;
    MEM_GWB(0x50)                           = D_800B5F44;
    D_800C356F                              = D_800B5F45;
    MEM_GWB(0x51)                           = D_800B5F45;
    MEM_GWB(0x52)                           = D_800B5F46;
    MEM_GWB(0x53)                           = D_800B5F47;
    Game_work.value_60                      = D_800B5F80;
    MEM_GWB(0x78)                           = D_800B5FB4;
    MEM_GWB(0x79)                           = D_800B5FB5;
    MEM_GWB(0x7A)                           = D_800B5FB6;
    MEM_GWB(0x7B)                           = D_800B5FB7;
    Game_work.flags_7c                      = D_800B5FB0;
    Game_work.flags_7d                      = D_800B5FB1;
    Game_work.value_62                      = D_800B5F82;
    Game_work.word_64                       = D_800B5F90;
    *(uint16 *)((uint8 *)&Game_work + 0x64) = 0;
    if (MEM_GWB(0x66) >= 0x18)
    {
        MEM_GWB(0x66) = 0;
    }
    if (MEM_GWB(0x67) >= 6)
    {
        MEM_GWB(0x67) = 0;
    }
    D_800C3560    = D_800B5F70;
    D_800C3570    = 2;
    D_800C356D    = 0;
    MEM_GWB(0x54) = D_800B5F48;
    MEM_GWB(0x55) = D_800B5F49;
    MEM_GWB(0x56) = D_800B5F4A;
    MEM_GWB(0x58) = D_800B5F4C;
    MEM_GWB(0x59) = D_800B5F4D;
    MEM_GWB(0x5A) = D_800B5F4E;
    MEM_GWB(0x5B) = D_800B5F4F;
    D_800C356A    = D_800B5F78;
}

sint32 save_image_sum_check(void)
{
    uint32 sum[3];
    uint32 *save;
    uint32 i;
    uint32 w;

    save = MEM_SAVE_WORDS;
    MEM_SUM_BLOCK(0, 0x5C);
    if (save[0x5D] != save[0x5C] + sum[0])
    {
        return 3;
    }
    if (save[0x5E] != (save[0x5C] ^ 0xE1E1E1E1) + sum[1])
    {
        return 3;
    }
    if (save[0x5F] != (save[0x5C] ^ 0x57575757) + sum[2])
    {
        return 3;
    }
    MEM_SUM_BLOCK(0x60, 0x6C);
    if (save[0x6D] != ~save[0x6C] + sum[0])
    {
        return 3;
    }
    if (save[0x6E] != (save[0x6C] ^ 0x33333333) + sum[1])
    {
        return 3;
    }
    if (save[0x6F] != (save[0x6C] ^ 0xCCCCCCCC) + sum[2])
    {
        return 3;
    }
    MEM_SUM_BLOCK(0x70, 0x7C);
    if (save[0x7D] != (save[0x7C] ^ 0x69696969) + sum[0])
    {
        return 3;
    }
    if (save[0x7E] != (save[0x7C] ^ 0x2B2B2B2B) + sum[1])
    {
        return 3;
    }
    if (save[0x7F] != (save[0x7C] ^ 0xB8B8B8B8) + sum[2])
    {
        return 3;
    }
    MEM_SUM_BLOCK(0x80, 0xDC);
    if (save[0xDD] != save[0xDC] + sum[0])
    {
        return 3;
    }
    if (save[0xDE] != (save[0xDC] ^ 0x1E1E1E1E) + sum[1])
    {
        return 3;
    }
    if (save[0xDF] != (save[0xDC] ^ 0x75757575) + sum[2])
    {
        return 3;
    }
    MEM_SUM_BLOCK(0xE0, 0x74C);
    if (save[0x74D] != ~save[0x74C] + sum[0])
    {
        return 3;
    }
    if (save[0x74E] != (save[0x74C] ^ 0x33333333) + sum[1])
    {
        return 3;
    }
    if (save[0x74F] != (save[0x74C] ^ 0xCCCCCCCC) + sum[2])
    {
        return 3;
    }
    MEM_SUM_BLOCK(0x750, 0x7B8);
    if (save[0x7B9] != (save[0x7B8] ^ 0x96969696) + sum[0])
    {
        return 3;
    }
    if (save[0x7BA] != (save[0x7B8] ^ 0xB2B2B2B2) + sum[1])
    {
        return 3;
    }
    if (save[0x7BB] != (save[0x7B8] ^ 0x8B8B8B8B) + sum[2])
    {
        return 3;
    }
    if (save[0x7BD] != ((save[0x5D] + save[0x6D]) ^ save[0x5C]) +
                           ((save[0x7D] + save[0xDD]) ^ save[0x7C]) +
                           ((save[0x74D] + save[0x7B9]) ^ save[0x74C]))
    {
        return 3;
    }
    if (save[0x7BE] != ((save[0x5E] + save[0x6E]) ^ save[0x6C]) +
                           ((save[0x7E] + save[0xDE]) ^ save[0xDC]) +
                           ((save[0x74E] + save[0x7BA]) ^ save[0x7B8]))
    {
        return 3;
    }
    if (save[0x7BF] != ((save[0x5F] + save[0x6F]) ^ save[0x5C]) +
                           ((save[0x7F] + save[0xDF]) ^ save[0x6C]) +
                           ((save[0x74F] + save[0x7BB]) ^ save[0x7C]))
    {
        return 3;
    }
    if (save[0x7BC] != ~save[0x7BD] + (save[0x7BE] ^ 0xAAAAAAAA) + (save[0x7BF] ^ 0x55555555))
    {
        return 3;
    }
    return 0;
}

void Game_work_new(void)
{
    sint32 flags;
    MEM_ENTRY *ep;

    flags                  = Game_work.flags_82;
    ep                     = &D_800C3558;
    Game_work.value_60     = 0x80;
    Game_work.flags_79     = -1;
    Game_work.flags_7a     = -1;
    Game_work.flag_5a      = 1;
    Game_work.play_time    = 0;
    Game_work.words_1c[0]  = 0;
    Game_work.words_1c[1]  = 0;
    Game_work.words_1c[2]  = 0;
    Game_work.words_1c[3]  = 0;
    Game_work.words_1c[4]  = 0;
    Game_work.words_1c[5]  = 0;
    Game_work.words_1c[6]  = 0;
    Game_work.words_1c[7]  = 0;
    Game_work.words_1c[8]  = 0;
    Game_work.halves_40[0] = 0;
    Game_work.halves_40[1] = 0;
    Game_work.halves_40[2] = 0;
    Game_work.halves_40[3] = 0;
    Game_work.halves_40[4] = 0;
    Game_work.halves_40[5] = 0;
    Game_work.word_4c      = 0;
    Game_work.flags_78     = 0;
    Game_work.flags_7b     = 0;
    Game_work.flags_7c     = 0;
    Game_work.flags_7d     = 0;
    Game_work.value_62     = 0;
    Game_work.word_64      = 0;
    if (flags & 0x80)
    {
        D_800C3570 = 0xFF;
    }
    else if (Game_work.value_50 != 0)
    {
        D_800C3570 = 2;
    }
    else
    {
        D_800C3570 = 0xFF;
    }
    ep->flag_15 = 1;
}

void Card_screen_init(void)
{
    D_80098910             = 0;
    *(uint32 *)&D_80098914 = 0;
}

// Returns the screen handler's result (1 once the screen is done): the in-game save screen
// (game.c, r3_sup) adds it to its routine.
sint32 Card_screen_exec(sint16 arg)
{
    D_80098912    = arg;
    D_80098918[1] = D_800C0C2A;
    D_80098918[0] = D_800C0C26;
    return Card_screen_tbl[D_80098914]((MEM_WORK *)&D_80098910);
}

sint32 Card_screen_start(MEM_WORK *wp)
{
    sint32 r;

    wp->value_0 = 1;
    if (wp->arg_2 == 0)
    {
        wp->mode_4             = 2;
        wp->sub_5              = 0;
        *(uint16 *)&wp->step_6 = 0;
        card_slots_init();
        r = Card_screen_load(wp);
    }
    else
    {
        wp->mode_4             = 1;
        wp->sub_5              = 0;
        *(uint16 *)&wp->step_6 = 0;
        card_slots_init();
        r = Card_screen_save(wp);
    }
    return r;
}

sint32 Card_screen_save(MEM_WORK *wp)
{
    Card_sub_tbl[wp->sub_5](wp);
    return 0;
}

// Same code as Card_screen_save.
sint32 Card_screen_load(MEM_WORK *wp)
{
    Card_sub_tbl[wp->sub_5](wp);
    return 0;
}

sint32 Card_screen_end(MEM_WORK *wp)
{
    wp->value_0            = 0;
    *(uint32 *)&wp->mode_4 = 0;
    wp->mode_4             = 0;
    return 1;
}

// A sint16 choice gives retail's two copies of Moji_flag & 0xFF (one for the switch, one for
// the D_80098A48 store); uint8 gives one, sint32 none.
sint32 Card_port_select(MEM_WORK *wp)
{
    sint16 choice;

    switch (wp->step_6)
    {
        case 0:
            MojiTaskKill();
            if (wp->mode_4 == 1)
            {
                MojiTaskExec(0, D_8008CAEC, -1);
                MojiTaskExec(1, D_8008CAF0, -1);
                MojiTaskExec(2, D_8008CAF4, -1);
            }
            else
            {
                MojiTaskExec(0, D_8008CB40, -1);
                MojiTaskExec(1, D_8008CB44, -1);
                MojiTaskExec(2, D_8008CB48, -1);
            }
            wp->step_6++;
            // fall through
        case 1:
            if (Moji_flag & 0x40000000)
            {
                choice = Moji_flag & 0xFF;
                switch (choice)
                {
                    case 0:
                    case 1:
                        wp->sub_5              = 1;
                        *(uint16 *)&wp->step_6 = 0;
                        D_80098A48             = choice;
                        break;
                    case 2:
                        wp->mode_4 = 3;
                        break;
                }
            }
            break;
    }
    return 1;
}

sint32 Card_files_read(MEM_WORK *wp)
{
    MEM_SLOT *slot;
    sint16 i;
    sint16 j;
    sint16 ok;
    sint16 blank;

    switch (wp->step_6)
    {
        case 0:
            MojiTaskKill();
            if (wp->mode_4 == 1)
            {
                MojiTaskExec(0, D_8008CAEC, -1);
            }
            else
            {
                MojiTaskExec(0, D_8008CB40, -1);
            }
            MojiTaskExec(1, D_8008CAF8[D_80098A48], -1);
            MojiTaskExec(2, D_8008CB00, -1);
            wp->timer_0c = 0xF;
            wp->step_6++;
            card_slots_init();
            break;
        case 1:
            if (--wp->timer_0c != 0)
            {
                break;
            }
            blank = 0;
            ok    = 0;
            for (i = 0; i < 5; i++)
            {
                slot = &D_800BAF38[i];
                switch ((sint16)card_file_read(D_80098A48, D_800980AC[i], slot, 0x180))
                {
                    case -1:
                        slot->byte_109 = 1;
                        ok++;
                        break;
                    case 0:
                        for (j = 0; j < 13; j++)
                        {
                            D_8008D168[i][j] = D_8008CE94[slot->index_100][j];
                        }
                        for (j = 0; j < 8; j++)
                        {
                            D_8008D184[i][j] = slot->data_101[j];
                        }
                        slot->byte_109 = 0;
                        break;
                    case 1:
                        wp->sub_5              = 2;
                        *(uint16 *)&wp->step_6 = 0;
                        return 0;
                    case 2:
                        if (++blank == 5)
                        {
                            wp->sub_5              = 3;
                            *(uint16 *)&wp->step_6 = 0;
                            return 0;
                        }
                        for (j = 0; j < 13; j++)
                        {
                            D_8008D168[i][j] = D_80097FAC[j];
                        }
                        for (j = 0; j < 8; j++)
                        {
                            D_8008D184[i][j] = slot->data_101[j] = D_80097FFC[j];
                        }
                        slot->byte_109  = 2;
                        slot->index_100 = 0;
                        break;
                    default:
                        for (j = 0; j < 13; j++)
                        {
                            D_8008D168[i][j] = D_80097FC0[j];
                        }
                        for (j = 0; j < 8; j++)
                        {
                            D_8008D184[i][j] = slot->data_101[j] = D_80097FFC[j];
                        }
                        wp->step_7     = 1;
                        slot->byte_109 = 3;
                        break;
                    case 4:
                        wp->sub_5              = 4;
                        *(uint16 *)&wp->step_6 = 0;
                        return 0;
                }
            }
            if (wp->mode_4 == 2 && ok == 5)
            {
                wp->sub_5              = 6;
                *(uint16 *)&wp->step_6 = 0;
                return 0;
            }
            wp->step_6++;
            break;
        case 2:
            if (wp->step_7 != 0)
            {
                MojiTaskExec(2, D_8008CB38, -1);
                wp->step_6++;
                break;
            }
            wp->sub_5              = 5;
            *(uint16 *)&wp->step_6 = 0;
            break;
        case 3:
            if (Moji_flag & 0x80000)
            {
                wp->sub_5              = 5;
                *(uint16 *)&wp->step_6 = 0;
            }
            break;
    }
    return 1;
}

// Card_sub_tbl[2]: no memory card in the port (card_check timeout); shows the message,
// then back to the port select.
sint32 Card_no_card(MEM_WORK *wp)
{
    switch (wp->step_6)
    {
        case 0:
            MojiTaskKill();
            if (wp->mode_4 == 1)
            {
                MojiTaskExec(0, D_8008CAEC, -1);
            }
            else
            {
                MojiTaskExec(0, D_8008CB40, -1);
            }
            MojiTaskExec(1, D_8008CAF8[D_80098A48], -1);
            MojiTaskExec(2, D_8008CB04, -1);
            wp->step_6++;
            break;
        case 1:
            if (Moji_flag & 0x80000)
            {
                wp->sub_5              = 0;
                *(uint16 *)&wp->step_6 = 0;
            }
            break;
    }
    return 1;
}

// Card_sub_tbl[3]: none of the five save files exists and the card has no free blocks
// for one; shows the message, then back to the port select.
sint32 Card_full(MEM_WORK *wp)
{
    switch (wp->step_6)
    {
        case 0:
            MojiTaskKill();
            if (wp->mode_4 == 1)
            {
                MojiTaskExec(0, D_8008CAEC, -1);
                MojiTaskExec(2, D_8008CB08, -1);
            }
            else
            {
                MojiTaskExec(0, D_8008CB40, -1);
                MojiTaskExec(2, D_8008CADC, -1);
            }
            MojiTaskExec(1, D_8008CAF8[D_80098A48], -1);
            wp->step_6++;
            break;
        case 1:
            if (Moji_flag & 0x80000)
            {
                wp->sub_5              = 0;
                *(uint16 *)&wp->step_6 = 0;
            }
            break;
    }
    return 1;
}

sint32 Card_format_ask(MEM_WORK *wp)
{
    switch (wp->step_6)
    {
        case 0:
            MojiTaskKill();
            MojiTaskExec(1, D_8008CAF8[D_80098A48], -1);
            if (wp->mode_4 == 1)
            {
                MojiTaskExec(0, D_8008CAEC, -1);
                MojiTaskExec(2, D_8008CB10, -1);
                wp->step_6++;
            }
            else
            {
                MojiTaskExec(0, D_8008CB40, -1);
                MojiTaskExec(2, D_8008CB5C, -1);
                wp->step_6 = 5;
            }
            wp->timer_10 = 0x1E;
            break;
        case 1:
            if (Moji_flag & 0x40000000)
            {
                D_80098728 = Moji_flag & 0xFF;
                if (D_80098728 != 0)
                {
                    wp->sub_5              = 0;
                    *(uint16 *)&wp->step_6 = 0;
                    break;
                }
                MojiTaskExec(2, D_8008CB14, -1);
                wp->timer_0c = 0xF;
                wp->step_6++;
            }
            else if (wp->timer_10 != 0)
            {
                wp->timer_10--;
            }
            else
            {
                wp->timer_10 = 0x1E;
                if (card_info_get(D_80098A48) == 0)
                {
                    wp->sub_5              = 2;
                    *(uint16 *)&wp->step_6 = 0;
                }
            }
            break;
        case 2:
            if (--wp->timer_0c == 0)
            {
                wp->step_6 += 2 + card_format(D_80098A48);
            }
            break;
        case 3:
            MojiTaskExec(2, D_8008CB1C, -1);
            wp->step_6 += 2;
            break;
        case 4:
            MojiTaskExec(2, D_8008CB18, -1);
            wp->step_6++;
            break;
        case 5:
            if (Moji_flag & 0x80000)
            {
                if (wp->mode_4 == 1)
                {
                    wp->sub_5 = 1;
                }
                else
                {
                    wp->sub_5 = 0;
                }
                *(uint16 *)&wp->step_6 = 0;
            }
            break;
    }
    return 1;
}

// Card_sub_tbl[5]: picks one of the five save files, confirms, then writes it (save) or
// reads it (load); card errors go to the matching message state.
sint32 Card_file_select(MEM_WORK *wp)
{
    MEM_SLOT *slot;
    sint16 j;
    sint16 r;

    switch (wp->step_6)
    {
        case 0:
            MojiTaskKill();
            if (wp->mode_4 == 1)
            {
                MojiTaskExec(0, D_8008CAEC, -1);
            }
            else
            {
                MojiTaskExec(0, D_8008CB40, -1);
            }
            MojiTaskExec(1, D_8008CAF8[D_80098A48], -1);
            MojiTaskExec(2, D_8008CB20, -1);
            wp->timer_10 = 0x1E;
            wp->step_6++;
            break;
        case 1:
            if (Moji_flag & 0x60000000)
            {
                D_80098A04 = Moji_flag;
                if ((Moji_flag & 0xFF) == 5)
                {
                    wp->sub_5              = 0;
                    *(uint16 *)&wp->step_6 = 0;
                    Sound_call(0x83, 1, 0);
                    break;
                }
                slot = &D_800BAF38[Moji_flag & 0xFF];
                if ((wp->mode_4 == 2 && slot->byte_109 == 0) ||
                    (wp->mode_4 == 1 && slot->byte_109 < 3))
                {
                    for (j = 0; j < 13; j++)
                    {
                        D_8008D168[5][j] = D_8008CE94[slot->index_100][j];
                    }
                    for (j = 0; j < 8; j++)
                    {
                        D_8008D184[5][j] = slot->data_101[j];
                    }
                    D_8008D168[5][-2] = D_8008D029[D_80098A04];
                    MojiTaskExec(2, D_8008CB24, -1);
                    if (wp->mode_4 == 1)
                    {
                        MojiTaskExec(3, D_8008CB28, -1);
                    }
                    else
                    {
                        MojiTaskExec(3, D_8008CB4C, -1);
                    }
                    wp->step_6++;
                    Sound_call(0x82, 1, 0);
                }
                else
                {
                    Sound_call(0x83, 1, 0);
                    return 1;
                }
            }
            else if (wp->timer_10 != 0)
            {
                wp->timer_10--;
            }
            else
            {
                wp->timer_10 = 0x1E;
                if (card_info_get(D_80098A48) == 0)
                {
                    wp->sub_5              = 2;
                    *(uint16 *)&wp->step_6 = 0;
                }
            }
            break;
        case 2:
            if (Moji_flag & 0x40000000)
            {
                if (Moji_flag & 0xFF)
                {
                    wp->sub_5              = 5;
                    *(uint16 *)&wp->step_6 = 0;
                }
                else
                {
                    if (wp->mode_4 == 1)
                    {
                        MojiTaskExec(3, D_8008CB2C, -1);
                        wp->step_6++;
                    }
                    else
                    {
                        MojiTaskExec(3, D_8008CB50, -1);
                        wp->step_6 = 10;
                    }
                    wp->timer_0c = 0xF;
                }
            }
            else if (wp->timer_10 != 0)
            {
                wp->timer_10--;
            }
            else
            {
                wp->timer_10 = 0x1E;
                if (card_info_get(D_80098A48) == 0)
                {
                    wp->sub_5              = 2;
                    *(uint16 *)&wp->step_6 = 0;
                }
            }
            break;
        case 3:
            if (--wp->timer_0c != 0)
            {
                break;
            }
            save_image_make();
            save_image_sum_set();
            slot = &MEM_SAVE;
            switch ((sint16)card_file_write(D_80098A48, D_800980AC[D_80098A04], slot, 0x1F00))
            {
                case 0:
                    for (j = 0; j < 13; j++)
                    {
                        D_8008D168[5][j] = D_8008CE94[slot->index_100][j];
                    }
                    for (j = 0; j < 8; j++)
                    {
                        D_8008D184[5][j] = slot->data_101[j];
                    }
                    wp->step_6++;
                    MojiTaskExec(3, D_8008CB30, -1);
                    break;
                case 1:
                    wp->sub_5              = 2;
                    *(uint16 *)&wp->step_6 = 0;
                    return 0;
                case 2:
                    wp->sub_5              = 3;
                    *(uint16 *)&wp->step_6 = 0;
                    return 0;
                case 4:
                    wp->sub_5              = 4;
                    *(uint16 *)&wp->step_6 = 0;
                    return 0;
                case -1:
                default:
                    MojiTaskExec(3, D_8008CB34, -1);
                    wp->step_6++;
                    break;
            }
            break;
        case 4:
            if (Moji_flag & 0x80000)
            {
                wp->sub_5              = 0;
                *(uint16 *)&wp->step_6 = 0;
            }
            break;
        case 10:
            if (--wp->timer_0c != 0)
            {
                break;
            }
            r = card_file_read(D_80098A48, D_800980AC[D_80098A04], &MEM_SAVE, 0x1F00);
            if (r == 0)
            {
                r = save_image_sum_check();
            }
            switch (r)
            {
                case 0:
                    wp->step_6++;
                    MojiTaskExec(3, D_8008CB54, -1);
                    break;
                case 1:
                    wp->sub_5              = 2;
                    *(uint16 *)&wp->step_6 = 0;
                    return 0;
                case -1:
                case 2:
                default:
                    MojiTaskExec(3, D_8008CB58, -1);
                    wp->step_6 = 4;
                    break;
                case 4:
                    wp->sub_5              = 4;
                    *(uint16 *)&wp->step_6 = 0;
                    return 0;
            }
            break;
        case 11:
            if (Moji_flag & 0x80000000)
            {
                wp->mode_4 = 3;
                Game_work.flags_82 |= 1;
            }
            break;
    }
    return 1;
}

// Card_sub_tbl[6]: loading, and none of the five save files exists; shows the message,
// then back to the port select.
sint32 Card_no_data(MEM_WORK *wp)
{
    switch (wp->step_6)
    {
        case 0:
            MojiTaskKill();
            if (wp->mode_4 == 1)
            {
                MojiTaskExec(0, D_8008CAEC, -1);
            }
            else
            {
                MojiTaskExec(0, D_8008CB40, -1);
            }
            MojiTaskExec(1, D_8008CAF8[D_80098A48], -1);
            MojiTaskExec(2, D_8008CAD8, -1);
            wp->step_6++;
            break;
        case 1:
            if (Moji_flag & 0x80000)
            {
                wp->sub_5              = 0;
                *(uint16 *)&wp->step_6 = 0;
            }
            break;
    }
    return 1;
}

void card_slots_init(void)
{
    sint32 i;
    sint32 j;
    MEM_SLOT *slot;
    uint8 *dst;
    uint8 *sp;
    uint8 v;

    for (i = 0; i < 5; i++)
    {
        slot = &D_800BAF38[i];
        card_header_set((uint8 *)slot, D_8008FA64);
        slot->byte_109  = 0;
        slot->index_100 = 0;
        for (j = 0; j < 13; j++)
        {
            D_8008D168[i][j] = D_8008CE94[slot->index_100][j];
        }
        j = 0;
        do
        {
            sp  = (uint8 *)slot + j;
            v   = D_80097FFC[j];
            dst = D_8008D184[i] + j;
            j++;
            sp[0x101] = v;
            *dst      = v;
        } while (j < 8);
        D_8008D168[i][-2] = D_8008D029[i];
    }
}

// (the function compiles longer: retail shares the timer/30/60 quotients across digits)
void Play_time_label_set(void)
{
    uint32 sec;
    uint32 min;
    uint32 hour;
    uint32 digit;

    sec        = Game_work.play_time / 30;
    min        = sec / 60;
    digit      = sec % 10;
    D_80097FDB = D_8008D028[digit];
    D_80098805 = digit;
    digit      = (sec % 60) / 10;
    D_80097FDA = D_8008D028[digit];
    D_80098804 = digit;
    hour       = min / 60;
    digit      = min % 10;
    D_80097FD8 = D_8008D028[digit];
    D_80098803 = digit;
    digit      = (min % 60) / 10;
    D_80097FD7 = D_8008D028[digit];
    D_80098802 = digit;
    digit      = hour % 10;
    D_80097FD5 = D_8008D028[digit];
    D_80098801 = digit;
    digit      = (hour % 100) / 10;
    D_80097FD4 = D_8008D028[digit];
    D_80098800 = digit;
}

void func_80067B7C(void)
{
    uint32 sec;
    uint32 min;
    uint32 hour;
    uint32 digit;

    sec        = Game_work.timer_14 / 30;
    min        = sec / 60;
    digit      = sec % 10;
    D_80097FFB = D_8008D028[digit];
    digit      = (sec % 60) / 10;
    D_80097FFA = D_8008D028[digit];
    hour       = min / 60;
    digit      = min % 10;
    D_80097FF8 = D_8008D028[digit];
    digit      = (min % 60) / 10;
    D_80097FF7 = D_8008D028[digit];
    digit      = hour % 10;
    D_80097FF5 = D_8008D028[digit];
    digit      = (hour % 100) / 10;
    D_80097FF4 = D_8008D028[digit];
}

void save_title_set(void)
{
    D_8008FA84[0] = D_80098124[D_80098A04 + 1];
    D_8008FA84[9] = D_80098124[(uint8)D_80098805];
    D_8008FA84[8] = D_80098124[(uint8)D_80098804];
    D_8008FA84[6] = D_80098124[(uint8)D_80098803];
    D_8008FA84[5] = D_80098124[(uint8)D_80098802];
    D_8008FA84[3] = D_80098124[(uint8)D_80098801];
    D_8008FA84[2] = D_80098124[(uint8)D_80098800];
}

void func_80067E18(void)
{
    D_800BE428 = 0x3F00;
    D_800BE42C = 0x18;
    D_800BE430 = 0x80;
    D_800BE434 = 0;
}

void func_80067E4C(void)
{
    Sce_flag_on(0x4C0);
}

void Records_init(sint32 load)
{
    sint32 i;

    if (load == 0)
    {
        for (i = 0; i < 9; i++)
        {
            Game_work.records_84[i].id            = D_8008FF64[i];
            MEM_WORDS(Game_work.records_84[i])[1] = MEM_WORDS(D_8008D034[i])[0];
            MEM_WORDS(Game_work.records_84[i])[2] = MEM_WORDS(D_8008D034[i])[1];
            Game_work.records_84[i].value_c       = 0;
        }
        for (i = 0; i < 3; i++)
        {
            Game_work.records_84[i + 9].id             = D_8009813C[i];
            Game_work.records_84[i + 12].id            = D_80098148[i];
            MEM_WORDS(Game_work.records_84[i + 9])[1]  = MEM_WORDS(D_8008D07C[i])[0];
            MEM_WORDS(Game_work.records_84[i + 9])[2]  = MEM_WORDS(D_8008D07C[i])[1];
            MEM_WORDS(Game_work.records_84[i + 12])[1] = MEM_WORDS(D_8008D07C[i + 3])[0];
            MEM_WORDS(Game_work.records_84[i + 12])[2] = MEM_WORDS(D_8008D07C[i + 3])[1];
            Game_work.records_84[i + 9].value_c        = 0;
            Game_work.records_84[i + 12].value_c       = 0;
        }
    }
    else
    {
        func_800176A4(D_800B7AF0, Game_work.records_84, 15);
    }
}
