#include "common.h"
#include "system.h"
#include "sound.h"
#include "cd.h"
#include "game.h"
#include "psyq/libcd.h"

void Cd_init(void)
{
    sint32 i;

    while (CdInit() == 0)
    {
    }

    Cd_muted      = 0; // stored twice in the original; both needed for the match
    Cd_ring_write = 0;
    Cd_status     = 0;
    Cd_xa_state   = 7;
    Cd_mode       = 0;
    D_80098AA0    = 0;
    Cd_muted      = 0; // stored twice in the original; both needed for the match

    Cd_controlb_retry(CdlSetmode, &Cd_mode, Cd_result);

    Cd_sync_done = 1;

    for (i = 0; i < CD_CMD_QUEUE_LEN; i++)
    {
        Cd_cmd_queue[i].type = CD_CMD_END;
    }

    Cd_cmd_exec_ptr  = Cd_cmd_queue;
    Cd_cmd_write_ptr = Cd_cmd_queue;

    Cd_wait          = 0;
    Cd_movie_playing = 0;
    Cd_head_state    = 0;
    Cd_cmd_routine   = 0;
    Cd_play_mode     = 0;
}

void cd_cmd_nop(void)
{
}

void cd_cmd_read(void)
{
    switch ((uint32)Cd_cmd_routine)
    {
        case 0:
            if (Cd_sync_done == 0)
            {
                Cd_stop();
            }
            Cd_cmd_routine++;
            // fall through
        case 1:
            if (Cd_sync_done == 0)
            {
                break;
            }
            Cd_set_mode(CdlModeSpeed | CdlModeSize1);
            Cd_cmd_routine++;
            // fall through
        case 2:
            if (Cd_wait)
            {
                Cd_wait--;
                break;
            }
            // This command carries the destination pointer in its second argument word.
            Cd_read_start_buf(Cd_cmd_exec_ptr->args.arg1, (uint8 *)Cd_cmd_exec_ptr->args.arg2);
            Cd_play_mode = 0;
            Cd_wait      = 0;
            Cd_cmd_routine++;
            // fall through
        case 3:
            Cd_wait++;
            if (Cd_wait == CD_CMD_TIMEOUT)
            {
                Cd_error_flags |= CD_ERR_CMD_TIMEOUT;
            }
            else if (Cd_status == 0)
            {
                Cd_cmd_dequeue();
            }
            break;
    }
}

void cd_subcmd_nop(void)
{
}

void cd_subcmd_xa_stop(void)
{
    sint32 i;

    switch (Cd_cmd_routine)
    {
        case 0:
            for (i = CD_VOL_MAX; i > 0; i--)
            {
                Cd_set_volume(i);
            }

            Sound_work.flags |= SOUND_FLAG_CD_OFF;
            Cd_stop();
            Cd_play_mode = 1;
            Cd_cmd_routine++;
            // fall through
        case 1:
            if (Cd_sync_done)
            {
                Cd_xa_state = 1;
                Cd_cmd_dequeue();
            }
            break;
    }
}

void cd_subcmd_xa_pause(void)
{
    sint32 i;

    switch (Cd_cmd_routine)
    {
        case 0:
            for (i = CD_VOL_MAX; i > 0; i--)
            {
                Cd_set_volume(i);
            }

            Sound_work.flags |= SOUND_FLAG_CD_OFF;
            Cd_stop();
            Cd_play_mode = 1;
            Cd_cmd_routine++;
            // fall through
        case 1:
            if (Cd_sync_done)
            {
                Cd_xa_state = 2;
                Cd_cmd_dequeue();
            }
            break;
    }
}

void cd_subcmd_mute(void)
{
    sint32 i;
    uint16 tmp;

    for (i = CD_VOL_MAX; i > 0; i--)
    {
        Cd_set_volume(i);
    }

    Sound_work.flags |= SOUND_FLAG_CD_OFF;

    if (Cd_muted)
    {
        Cd_muted         = 0;
        Sound_work.unk06 = Cd_mute_save;
    }
    else
    {
        tmp              = Sound_work.unk06;
        Sound_work.unk06 = 0;
        Cd_muted         = 1;
        Cd_mute_save     = tmp;
    }

    Cd_cmd_dequeue();
}

void cd_cmd_subcmd(void)
{
    cd_subcmd_tbl[Cd_cmd_exec_ptr->args.arg2]();
}

void cd_cmd_xa_play(void)
{
    CdlLOC pos;
    CdlFILTER filter;
    sint16 i;

    switch ((uint32)Cd_cmd_routine)
    {
        case 0:
            if (Cd_sync_done == 0)
            {
                Cd_stop();
            }
            Cd_play_mode = 2;
            Cd_cmd_routine++;
            // fall through
        case 1:
            if (Cd_sync_done == 0)
            {
                break;
            }
            Sound_work.unk0E = CD_VOL_MAX;
            Cd_set_mode(CdlModeSpeed | CdlModeRT | CdlModeSF);
            Cd_cmd_routine++;
            // fall through
        case 2:
            if (Cd_wait)
            {
                Cd_wait--;
                break;
            }
            Cd_status      = 0;
            Cd_error_flags = 0;
            Cd_sync_done   = Cd_status;
            if (Cd_xa_state != 2)
            {
                filter.file      = 1;
                filter.chan      = ((uint8 *)&Cd_cmd_exec_ptr->args.arg2)[3] & 0x1F;
                Cd_xa_end_sector = Cd_file_tbl[Cd_cmd_exec_ptr->args.arg1].sector +
                              (*(uint32 *)&Cd_cmd_exec_ptr->args.arg3 & 0xFFFFFF);
                CdIntToPos(Cd_file_tbl[Cd_cmd_exec_ptr->args.arg1].sector +
                               (Cd_cmd_exec_ptr->args.arg2 & 0xFFFFFF),
                           &pos);
                Cd_controlb_retry(CdlSetfilter, (u_char *)&filter, Cd_result);
                Cd_controlb_retry(CdlSetloc, (u_char *)&pos, Cd_result);
                Cd_controlb_retry(CdlReadS, NULL, Cd_result);
                Cd_wait    = 0;
                D_80098AB8 = Cd_result[0];
            }
            Cd_cmd_routine++;
            // fall through
        case 3:
            if ((D_80098AB8 & 0x20) == 0)
            {
                Cd_wait++;
                if ((uint32)Cd_wait >= CD_CMD_TIMEOUT)
                {
                    Cd_error_flags |= CD_ERR_TIMEOUT;
                }
            }
            else
            {
                for (i = 0; i < CD_VOL_MAX; i++)
                {
                    Cd_set_volume(i);
                }
                Cd_xa_state = 0;
                Sound_work.flags &= ~SOUND_FLAG_CD_OFF;
                Cd_cmd_dequeue();
            }
            break;
    }
}

// Shadow-VRAM row for an image y position (the 0x100 bit selects the second texture page row).
#define SHADOW_ROW(y) ((y) + (((y) & 0x100) >> 4) - (((y) & 0x100) + 0xF0))

// The buffered asset loader. One byte pointer serves as both the sector's record header and the
// destination of the byte-copy loops (one variable, which sets its register). D_8009BE48 is an
// incomplete array (a complete 8-byte RECT is small data to GCC, which then folds rect->x to the
// absolute address); the clut test reads back the stored clut_x; the seq/sep destination is a
// chained assignment.
void cd_cmd_read_comb(void)
{
    CD_ASSET_REC *rec;
    uint8 *src;
    uint8 *buf;
    RECT *rect;

    switch (Cd_cmd_routine)
    {
        case 0:
            if (Cd_sync_done == 0)
            {
                Cd_stop();
            }
            Cd_cmd_routine++;
            // fall through
        case 1:
            if (Cd_sync_done == 0)
            {
                break;
            }
            Cd_set_mode(0xA0);
            Cd_cmd_routine++;
            // fall through
        case 2:
            if (Cd_wait != 0)
            {
                Cd_wait--;
                break;
            }
            Cd_read_start_ring(Cd_cmd_exec_ptr->args.arg1);
            Cd_play_mode = 0;
            Cd_ring_read = 0;
            Cd_wait      = 0;
            Cd_cmd_routine++;
            // fall through
        case 3:
            if (++Cd_wait == CD_CMD_TIMEOUT)
            {
                Cd_error_flags |= CD_ERR_CMD_TIMEOUT;
                break;
            }
            while (Cd_ring_ready[Cd_ring_read] == 1)
            {
                switch (Cd_head_state)
                {
                    case 0:
                        buf        = CD_SECTOR_BUF(Cd_ring_read);
                        D_80098A54 = ((CD_ASSET_REC *)buf)->type;
                        if (D_80098A54 == -1)
                        {
                            Cd_cmd_routine = 4;
                            return;
                        }
                        D_800C5604.size  = ((CD_ASSET_REC *)buf)->size;
                        D_800C5604.count = ((CD_ASSET_REC *)buf)->count;
                        rec              = (CD_ASSET_REC *)buf;
                        switch (D_80098A54)
                        {
                            case 0:
                                D_800C5604.u.dest = (uint8 *)rec->arg[0];
                                D_800C5604.count--;
                                break;
                            case 1:
                                rect                    = D_8009BE48;
                                D_800C5604.u.tim.clut_x = rec->arg[0];
                                D_800C5604.u.tim.clut_y = rec->arg[1];
                                D_800C5604.u.tim.clut_w = rec->arg[2];
                                D_800C5604.u.tim.clut_h = rec->arg[3];
                                D_800C5604.u.tim.x      = rec->arg[4];
                                D_800C5604.u.tim.y      = rec->arg[5];
                                D_800C5604.u.tim.w      = rec->arg[6];
                                D_800C5604.u.tim.h      = rec->arg[7];
                                D_800C5604.row          = 0;
                                D_800C5604.col          = 0;
                                if (D_800C5604.u.tim.clut_x != -1)
                                {
                                    if (rec->arg[0] < 0x140)
                                    {
                                        func_800176A4(((CD_ASSET_REC *)buf)->data,
                                                      &D_800AD168[rec->arg[0] +
                                                                  (SHADOW_ROW(rec->arg[1]) << 8)],
                                                      ((uint32)(rec->arg[3] * rec->arg[2]) >> 4) *
                                                          2);
                                        D_80098868 |= 1 << SHADOW_ROW(rec->arg[1]);
                                    }
                                    else
                                    {
                                        rect->x = rec->arg[0];
                                        rect->y = rec->arg[1];
                                        rect->w = rec->arg[2];
                                        rect->h = rec->arg[3];
                                        LoadImage(rect, (u_long *)((CD_ASSET_REC *)buf)->data);
                                    }
                                }
                                rect->w = 0x20;
                                rect->h = 0x20;
                                D_800C5604.count--;
                                break;
                            case 3:
                                D_800C5604.u.tiles.x    = rec->arg[0];
                                D_800C5604.u.tiles.y    = rec->arg[1];
                                D_800C5604.u.tiles.cols = rec->arg[2];
                                D_800C5604.u.tiles.rows = rec->arg[3];
                                D_800C5604.row          = 0;
                                D_800C5604.col          = 0;
                                D_8009BE48[0].w         = 0x20;
                                D_8009BE48[0].h         = 0x20;
                                break;
                            case 4:
                                rect = D_8009BE48;
                            case 9:
                            case 10:
                                if (rec->arg[0] < 0x100)
                                {
                                    func_800176A4(
                                        ((CD_ASSET_REC *)buf)->data,
                                        &D_800AD168[rec->arg[0] + (SHADOW_ROW(rec->arg[1]) << 8)],
                                        ((uint32)(rec->arg[3] * rec->arg[2]) >> 4) * 2);
                                    D_80098868 |= 1 << SHADOW_ROW(rec->arg[1]);
                                }
                                else
                                {
                                    rect    = D_8009BE48;
                                    rect->x = rec->arg[0];
                                    rect->y = rec->arg[1];
                                    if (D_80098A54 != 10)
                                    {
                                        rect->w = rec->arg[2];
                                        rect->h = rec->arg[3];
                                    }
                                    else
                                    {
                                        rect->h = (uint32)(rec->arg[2] * rec->arg[3]) >> 6;
                                        rect->w = 0x40;
                                    }
                                    LoadImage(rect, (u_long *)((CD_ASSET_REC *)buf)->data);
                                }
                                Cd_head_state = 0;
                                goto next;
                            case 5:
                                D_800C5604.u.vab.slot = rec->arg[0];
                                D_800C5604.u.vab.id   = rec->arg[1];
                                D_800C5604.u.vab.arg  = rec->arg[2];
                                D_800C5604.u.vab.size = rec->arg[3];
                                D_800C5604.u.vab.head =
                                    (uint8 *)(rec->arg[0] * 0x1820 + 0x8014B000);
                                D_800C5604.u.vab.load = 1;
                                if (rec->arg[0] == 4)
                                {
                                    if (Game_work.sound_1b8[4].id == rec->arg[1])
                                    {
                                        D_800C5604.u.vab.load = 0;
                                    }
                                    if (Sound_flags & 0x800)
                                    {
                                        D_800C5604.u.vab.load = 1;
                                        Sound_seq_stop_all();
                                    }
                                }
                                if (D_800C5604.u.vab.load == 1)
                                {
                                    Game_work.sound_1b8[rec->arg[0]].id = rec->arg[1];
                                    *(sint32 *)&Game_work.sound_1b8[rec->arg[0]].reserved03[1] =
                                        rec->arg[2];
                                    if ((sint16)Game_work.sound_1b8[rec->arg[0]].vab != -1)
                                    {
                                        SsVabClose(Game_work.sound_1b8[rec->arg[0]].vab);
                                    }
                                }
                                D_800C5604.count--;
                                break;
                            case 7:
                                D_800C5604.u.seq.dest = (uint8 *)(D_80098AD4[rec->arg[0]][0] =
                                                                      rec->arg[2] + 0x8013B000);
                                D_800C5604.u.seq.load = 0;
                                if (D_80098AD0[rec->arg[0]].id != rec->arg[1])
                                {
                                    D_80098AD0[rec->arg[0]].id = rec->arg[1];
                                    D_800C5604.u.seq.load      = 1;
                                }
                                break;
                            case 8:
                                D_800C5604.u.sep.dest = (uint8 *)(D_80098AD4[rec->arg[0]][0] =
                                                                      rec->arg[3] + 0x8013B000);
                                D_800C5604.u.sep.load = 0;
                                if (D_80098AD0[rec->arg[0]].id != rec->arg[1])
                                {
                                    D_80098AD0[rec->arg[0]].id            = rec->arg[1];
                                    D_80098AD0[rec->arg[0]].reserved01[0] = rec->arg[2];
                                    D_800C5604.u.sep.load                 = 1;
                                }
                                break;
                        }
                        Cd_head_state = D_80098A54 + 1;
                        break;
                    case 1:
                        if (D_800C5604.size >= 0x800)
                        {
                            func_800176DC(CD_SECTOR_BUF(Cd_ring_read), D_800C5604.u.dest, 0x40);
                            D_800C5604.size -= 0x800;
                            D_800C5604.u.dest += 0x800;
                        }
                        else
                        {
                            src = CD_SECTOR_BUF(Cd_ring_read);
                            buf = D_800C5604.u.dest;
                            while (D_800C5604.size != 0)
                            {
                                *buf++ = *src++;
                                D_800C5604.size--;
                            }
                        }
                        if (--D_800C5604.count == 0)
                        {
                            Cd_head_state = 0;
                        }
                        break;
                    case 2:
                        rect    = D_8009BE48;
                        rect->x = D_800C5604.u.tim.x + (D_800C5604.col << 5);
                        rect->y = D_800C5604.u.tim.y + (D_800C5604.row << 5);
                        LoadImage(rect, (u_long *)CD_SECTOR_BUF(Cd_ring_read));
                        if (++D_800C5604.col == (uint32)D_800C5604.u.tim.w >> 5)
                        {
                            D_800C5604.col = 0;
                            if (++D_800C5604.row == (uint32)D_800C5604.u.tim.h >> 5)
                            {
                                Cd_head_state = 0;
                            }
                        }
                        break;
                    case 4:
                        rect    = D_8009BE48;
                        rect->x = D_800C5604.u.tiles.x + (D_800C5604.col << 5);
                        rect->y = D_800C5604.u.tiles.y + (D_800C5604.row << 5);
                        LoadImage(rect, (u_long *)CD_SECTOR_BUF(Cd_ring_read));
                        if (++D_800C5604.col == D_800C5604.u.tiles.cols)
                        {
                            D_800C5604.col = 0;
                            if (++D_800C5604.row == D_800C5604.u.tiles.rows)
                            {
                                Cd_head_state = 0;
                            }
                        }
                        break;
                    case 6:
                        if (D_800C5604.u.vab.load == 0)
                        {
                            if (--D_800C5604.count == 0)
                            {
                                Cd_head_state = 0;
                            }
                        }
                        else if (D_800C5604.u.vab.size >= 0x800)
                        {
                            func_800176DC(CD_SECTOR_BUF(Cd_ring_read), D_800C5604.u.vab.head, 0x40);
                            D_800C5604.u.vab.head += 0x800;
                            D_800C5604.u.vab.size -= 0x800;
                        }
                        else if (D_800C5604.u.vab.size != 0)
                        {
                            func_80017684(CD_SECTOR_BUF(Cd_ring_read), D_800C5604.u.vab.head,
                                          D_800C5604.u.vab.size / 4);
                            D_800C5604.u.vab.size                          = 0;
                            Game_work.sound_1b8[D_800C5604.u.vab.slot].vab = SsVabOpenHeadSticky(
                                (uint8 *)(D_800C5604.u.vab.slot * 0x1820 + 0x8014B000),
                                D_800C5604.u.vab.slot, D_800C5604.u.vab.arg);
                        }
                        else
                        {
                            if (SsVabTransBodyPartly(
                                    CD_SECTOR_BUF(Cd_ring_read), 0x800,
                                    Game_work.sound_1b8[D_800C5604.u.vab.slot].vab) ==
                                (sint16)Game_work.sound_1b8[D_800C5604.u.vab.slot].vab)
                            {
                                Cd_head_state = 0;
                            }
                            SsVabTransCompleted(1);
                        }
                        break;
                    case 8:
                        if (D_800C5604.size >= 0x800)
                        {
                            if (D_800C5604.u.seq.load != 0)
                            {
                                func_800176DC(CD_SECTOR_BUF(Cd_ring_read), D_800C5604.u.seq.dest,
                                              0x40);
                            }
                            D_800C5604.size -= 0x800;
                            D_800C5604.u.seq.dest += 0x800;
                        }
                        else
                        {
                            if (D_800C5604.u.seq.load != 0)
                            {
                                src = CD_SECTOR_BUF(Cd_ring_read);
                                buf = D_800C5604.u.seq.dest;
                                while (D_800C5604.size != 0)
                                {
                                    *buf++ = *src++;
                                    D_800C5604.size--;
                                }
                            }
                            Cd_head_state = 0;
                        }
                        break;
                    case 9:
                        if (D_800C5604.size >= 0x800)
                        {
                            if (D_800C5604.u.sep.load != 0)
                            {
                                func_800176DC(CD_SECTOR_BUF(Cd_ring_read), D_800C5604.u.sep.dest,
                                              0x40);
                            }
                            D_800C5604.size -= 0x800;
                            D_800C5604.u.sep.dest += 0x800;
                        }
                        else
                        {
                            if (D_800C5604.u.sep.load != 0)
                            {
                                src = CD_SECTOR_BUF(Cd_ring_read);
                                buf = D_800C5604.u.sep.dest;
                                while (D_800C5604.size != 0)
                                {
                                    *buf++ = *src++;
                                    D_800C5604.size--;
                                }
                            }
                            Cd_head_state = 0;
                        }
                        break;
                }
            next:
                Cd_ring_ready[Cd_ring_read] = 0;
                if (++Cd_ring_read == CD_READ_BUFFER_COUNT)
                {
                    Cd_ring_read = 0;
                }
            }
            break;
        case 4:
            Cd_cmd_dequeue();
            break;
    }
}

void cd_seek_cb(u_char status, u_char *result)
{
    Cd_cmd_routine++;
    CdSyncCallback(0);
}

void cd_cmd_seek(void)
{
    CdlLOC pos;

    switch ((uint32)Cd_cmd_routine)
    {
        case 0:
            if (Cd_sync_done == 0)
            {
                Cd_stop();
            }
            Cd_cmd_routine++;
            // fall through
        case 1:
            if (Cd_sync_done == 0)
            {
                break;
            }
            Cd_play_mode = 0;
            CdSyncCallback(cd_seek_cb);
            CdIntToPos(Cd_cmd_exec_ptr->args.arg2, &pos);
            Cd_control_retry(CdlSeekL, (u_char *)&pos, Cd_result);
            Cd_wait = 0;
            Cd_cmd_routine++;
            // fall through
        case 2:
            Cd_wait++;
            if (Cd_wait == CD_CMD_TIMEOUT)
            {
                Cd_error_flags |= CD_ERR_CMD_TIMEOUT;
            }
            break;
        case 3:
            Cd_cmd_dequeue();
            break;
    }
}

void Cd_cmd_exec(void)
{
    uint32 sector;

    if (Cd_error_flags)
    {
        Cd_sync_done   = 0;
        Cd_mode        = 0;
        Cd_error_flags = 0;
        Cd_head_state  = 0;
        Cd_cmd_routine = 0;
        CdSyncCallback(NULL);
        CdReadyCallback(NULL);
        while (CdReset(0) == 0)
        {
        }
    }

    if (Cd_play_mode == 2)
    {
        Cd_controlb_retry(CdlNop, NULL, Cd_result);
        D_80098AB8 = Cd_result[0];
        if (Cd_result[0] & 0x20)
        {
            Cd_controlb_retry(CdlGetlocP, NULL, Cd_result);
            sector       = CdPosToInt((CdlLOC *)D_80098A9D);
            Cd_xa_sector = sector;
            if (sector >= Cd_xa_end_sector)
            {
                if (Cd_xa_flags & 0x40000000)
                {
                    Cd_xa_play_id(Cd_xa_id);
                }
                else
                {
                    Cd_stop();
                }
            }
        }
    }
    cd_cmd_tbl[Cd_cmd_exec_ptr->type]();
}

// The command is copied field by field: a struct assignment is a block move, which
// reserves a fourth scratch register and shifts the allocation.
void Cd_cmd_dequeue(void)
{
    CD_CMD *p;

    for (p = Cd_cmd_queue; p->type != 0; p++)
    {
        p->type                  = p[1].type;
        p->args.arg1             = p[1].args.arg1;
        p->args.arg2             = p[1].args.arg2;
        *(uint32 *)&p->args.arg3 = *(uint32 *)&p[1].args.arg3;
    }

    p->type        = 0;
    Cd_head_state  = 0;
    Cd_cmd_routine = 0;
    Cd_cmd_write_ptr--;
}

void Cd_pause(void)
{
    Cd_error_flags = 0;
    CdReadyCallback(0);
    CdSyncCallback(Cd_sync_cb);
    Cd_control_retry(CdlPause, NULL, Cd_result);
}

void Cd_stop(void)
{
    sint32 i;

    for (i = CD_VOL_MAX; i > 0; i--)
    {
        Cd_set_volume(i);
    }

    Sound_work.flags |= SOUND_FLAG_CD_OFF;
    Cd_error_flags = 0;
    CdReadyCallback(0);
    CdSyncCallback(Cd_sync_cb);
    Cd_control_retry(CdlPause, 0, Cd_result);
    Cd_play_mode = 0;
    D_80098AB8   = 0;
}

void Cd_sync_cb(u_char status, u_char *result)
{
    CdSyncCallback(0);

    if (status == CdlComplete)
    {
        Cd_sync_done = 1;
        Cd_status    = 0;
    }
    else
    {
        Cd_status = 0x80;
        Cd_error_flags |= CD_ERR_SYNC;
    }
}

void Cd_read_start_buf(sint32 index, uint8 *dest)
{
    sint32 sector;

    sector         = Cd_file_tbl[index].sector;
    Cd_status      = 1;
    Cd_xa_state    = 7;
    Cd_error_flags = 0;
    Cd_wait        = 0;
    Cd_sync_done   = 0;
    D_80098A7C     = sector;
    CdIntToPos(sector, &D_80098814);
    Cd_read_buf = dest;
    D_8009881C  = Cd_file_tbl[index].size;
    CdReadyCallback(Cd_ready_cb_buf);
    Cd_controlb_retry(CdlSetloc, (u_char *)&D_80098814, Cd_result);
    Cd_controlb_retry(CdlReadN, NULL, Cd_result);
}

void Cd_ready_cb_buf(u_char status, u_char *result)
{
    sint32 sector;
    sint32 count;
    uint8 *src;

    Cd_wait = 0;
    if (status == CdlDataReady)
    {
        CdGetSector(Cd_ring_buf, 3);
        sector = CdPosToInt((CdlLOC *)Cd_ring_buf);
        if (sector != D_80098A7C)
        {
            Cd_error_flags |= CD_ERR_SECTOR;
            goto read_error;
        }
        D_80098A7C = sector + 1;
        if (D_8009881C >= CD_SECTOR_BYTES)
        {
            CdGetSector(Cd_read_buf, CD_SECTOR_WORDS);
            if (Cd_head_state == 0)
            {
                if (Cd_file_tbl[Cd_cmd_exec_ptr->args.arg1].unk8 != *(sint32 *)Cd_read_buf)
                {
                    Cd_error_flags |= CD_ERR_FILE_ID;
                    goto read_error;
                }
                Cd_head_state = status;
            }
            D_8009881C -= CD_SECTOR_BYTES;
            if (D_8009881C == 0)
            {
                CdReadyCallback(NULL);
                CdSyncCallback(Cd_sync_cb);
                Cd_control_retry(CdlPause, NULL, Cd_result);
                return;
            }
            Cd_read_buf += CD_SECTOR_BYTES;
            return;
        }
        CdGetSector(Cd_ring_buf, CD_SECTOR_WORDS);
        count = D_8009881C;
        src   = Cd_ring_buf;
        while (count != 0)
        {
            *Cd_read_buf = *src;
            Cd_read_buf++;
            src++;
            count--;
        }
        CdSyncCallback(Cd_sync_cb);
        Cd_control_retry(CdlPause, NULL, Cd_result);
    }
    else
    {
        Cd_error_flags |= CD_ERR_READY;
    read_error:
        Cd_status = 0x80;
    }
    CdReadyCallback(NULL);
}

void Cd_read_start_ring(sint32 index)
{
    sint32 i;
    sint32 sector;

    for (i = 0; i < 10; i++)
    {
        Cd_ring_ready[i] = 0;
    }

    sector         = Cd_file_tbl[index].sector;
    Cd_status      = 1;
    Cd_xa_state    = 7;
    Cd_error_flags = 0;
    Cd_wait        = 0;
    Cd_sync_done   = 0;
    Cd_ring_write  = 0;
    D_80098A7C     = sector;
    CdIntToPos(sector, &D_80098814);
    D_8009881C = Cd_file_tbl[index].size;
    CdReadyCallback(Cd_ready_cb_ring);
    Cd_controlb_retry(CdlReadN, (u_char *)&D_80098814, Cd_result);
}

void Cd_ready_cb_ring(u_char status, u_char *result)
{
    sint32 sector;

    Cd_wait = 0;
    if (status == CdlDataReady)
    {
        if (Cd_ring_ready[Cd_ring_write] == 0)
        {
            CdGetSector(Cd_ring_buf + Cd_ring_write * CD_SECTOR_BYTES, 3);
            sector = CdPosToInt((CdlLOC *)(Cd_ring_buf + Cd_ring_write * CD_SECTOR_BYTES));
            if (sector != D_80098A7C)
            {
                Cd_error_flags |= CD_ERR_SECTOR;
                goto read_error;
            }
            D_80098A7C = sector + 1;
            CdGetSector(Cd_ring_buf + Cd_ring_write * CD_SECTOR_BYTES, CD_SECTOR_WORDS);
            Cd_ring_ready[Cd_ring_write] = 1;
            Cd_ring_write++;
            if (Cd_ring_write == CD_READ_BUFFER_COUNT)
            {
                Cd_ring_write = 0;
            }
            D_8009881C -= CD_SECTOR_BYTES;
            if (D_8009881C <= 0)
            {
                CdReadyCallback(NULL);
                Cd_control_retry(CdlPause, NULL, Cd_result);
                CdSyncCallback(Cd_sync_cb);
            }
        }
        else
        {
            Cd_status = 0x80;
            Cd_error_flags |= CD_ERR_BUFFER_FULL;
            goto stop_read;
        }
    }
    else
    {
        Cd_error_flags |= CD_ERR_READY;
    read_error:
        Cd_status = 0x80;
    stop_read:
        CdReadyCallback(NULL);
        CdSyncCallback(Cd_sync_cb);
        Cd_control_retry(CdlPause, NULL, Cd_result);
    }
}

void Cd_control_retry(u_char com, u_char *param, u_char *result)
{
    while (CdSync(1, result) == 0)
    {
    }

    while (CdControl(com, param, result) == 0)
    {
    }
}

void Cd_controlb_retry(u_char com, u_char *param, u_char *result)
{
    while (CdSync(1, result) == 0)
    {
    }

    while (CdControlB(com, param, result) == 0)
    {
    }
}

void Cd_set_mode(u_char mode)
{
    Cd_wait = 0;

    if (Cd_mode != mode)
    {
        if ((Cd_mode & CdlModeSpeed) != (mode & CdlModeSpeed))
        {
            Cd_wait = 2;
        }

        Cd_mode = mode;
        Cd_controlb_retry(CdlSetmode, &Cd_mode, Cd_result);
    }
}

void Cd_set_volume(uint8 vol)
{
    SOUND_WORK *sw = &Sound_work;

    if (Sound_work.flags & SOUND_FLAG_MONO)
    {
        // mono: both inputs to both outputs
        Sound_work.atv.val0 = vol;
        Sound_work.atv.val1 = vol;
        Sound_work.atv.val2 = vol;
        Sound_work.atv.val3 = vol;
    }
    else
    {
        // stereo: left to left, right to right
        Sound_work.atv.val0 = vol;
        Sound_work.atv.val1 = 0;
        Sound_work.atv.val2 = vol;
        Sound_work.atv.val3 = 0;
    }

    CdMix(&sw->atv);
}

void Cd_xa_play(sint32 arg1, sint32 arg2)
{
    Cd_cmd_write_ptr->type      = 4;
    Cd_cmd_write_ptr->args.arg1 = arg1;
    Cd_cmd_write_ptr->args.arg2 = arg2;
    Cd_cmd_write_ptr++;
}

void Cd_read_comb(sint32 arg1)
{
    Cd_cmd_write_ptr->type      = 6;
    Cd_cmd_write_ptr->args.arg1 = arg1;
    Cd_cmd_write_ptr++;
}

void Cd_read(sint32 arg1, sint32 arg2)
{
    Cd_cmd_write_ptr->type      = 1;
    Cd_cmd_write_ptr->args.arg1 = arg1;
    Cd_cmd_write_ptr->args.arg2 = arg2;
    Cd_cmd_write_ptr++;
}

void Cd_subcmd(sint32 arg1, sint32 arg2, sint32 arg3)
{
    CD_CMD *p;
    CD_CMD *q;

    if (arg2 == 1)
    {
        for (p = &Cd_cmd_queue[1]; p < Cd_cmd_write_ptr; p++)
        {
            if (p->type == 2 && p->args.arg2 == 1)
            {
                for (q = p; q + 1 < Cd_cmd_write_ptr; q++)
                {
                    q->type                  = q[1].type;
                    q->args.arg1             = q[1].args.arg1;
                    q->args.arg2             = q[1].args.arg2;
                    *(uint32 *)&q->args.arg3 = *(uint32 *)&q[1].args.arg3;
                }
                Cd_cmd_write_ptr--;
            }
        }
    }

    Cd_cmd_write_ptr->type      = 2;
    Cd_cmd_write_ptr->args.arg1 = arg1;
    Cd_cmd_write_ptr->args.arg2 = arg2;
    // This command uses the complete final word, including the movie argument's padding.
    *(uint32 *)&Cd_cmd_write_ptr->args.arg3 = arg3;
    Cd_cmd_write_ptr++;
}

// As in Cd_xa_seek_id: `arg &= 0xFFFF` keeps retail's three shifts and one table pointer serves
// both rows. The type store written through a plain word pointer (not the struct field) keeps
// Cd_xa_id's store above it; the final word through CD_CMD_RAW lets the exec-pointer load move
// above it.
void Cd_xa_play_id(uint32 arg)
{
    uint32 hi;
    uint32 mid;
    uint32 rep;
    sint32 *tbl;
    CD_CMD *cmd;

    cmd            = Cd_cmd_write_ptr;
    hi             = (arg >> 28) & 3;
    Cd_xa_id       = arg;
    *(sint32 *)cmd = 4;
    mid            = (arg << 4) >> 20;
    rep            = arg & 0x40000000;
    arg &= 0xFFFF;
    tbl                         = Cd_xa_file_tbl[hi];
    cmd->args.arg1              = tbl[mid];
    tbl                         = (sint32 *)Cd_xa_track_tbl[hi][mid];
    cmd->args.arg2              = ((CD_POS *)tbl)[arg].offset;
    ((CD_CMD_RAW *)cmd)->arg[2] = ((CD_POS *)tbl)[arg].unk4 | rep;
    D_800989BC                  = Cd_cmd_exec_ptr->args.arg2;
    Cd_xa_flags                 = *(uint32 *)&Cd_cmd_exec_ptr->args.arg3;
    Cd_cmd_write_ptr            = cmd + 1;
}

// `arg &= 0xFFFF` between the middle index and its first use keeps retail's three shifts (combine
// can't fold them across the redefinition). The one table pointer reused for the file row and then
// the offset row gives retail's registers.
void Cd_xa_seek_id(uint32 arg)
{
    uint32 hi;
    uint32 mid;
    sint32 *tbl;
    sint32 file;

    hi  = arg >> 28;
    tbl = Cd_xa_file_tbl[hi];
    mid = (arg << 4) >> 20;
    arg &= 0xFFFF;
    file = tbl[mid];
    tbl  = (sint32 *)Cd_xa_track_tbl[hi][mid];
    hi   = Cd_file_tbl[file].sector + (((CD_POS *)tbl)[arg].offset & 0xFFFFFF);
    Cd_seek(hi);
}

void Cd_seek(sint32 sector)
{
    CD_CMD *p;
    CD_CMD *q;

    for (p = &Cd_cmd_queue[1]; p < Cd_cmd_write_ptr; p++)
    {
        if (p->type == 7)
        {
            for (q = p; q + 1 < Cd_cmd_write_ptr; q++)
            {
                q->type                  = q[1].type;
                q->args.arg1             = q[1].args.arg1;
                q->args.arg2             = q[1].args.arg2;
                *(uint32 *)&q->args.arg3 = *(uint32 *)&q[1].args.arg3;
            }
            Cd_cmd_write_ptr--;
        }
    }

    Cd_cmd_write_ptr->type      = 7;
    Cd_cmd_write_ptr->args.arg2 = sector;
    Cd_cmd_write_ptr++;
}

void Cd_seek_file(sint32 index)
{
    Cd_seek(Cd_file_tbl[index].sector);
}

void Cd_read_sync(void)
{
    while (Cd_cmd_write_ptr != Cd_cmd_queue)
    {
        Thread_wait(1);
    }
}

sint32 Cd_read_sync2(void)
{
    return Cd_cmd_write_ptr != Cd_cmd_queue;
}
