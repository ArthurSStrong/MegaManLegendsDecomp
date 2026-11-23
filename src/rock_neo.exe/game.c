#include "common.h"
#include "game.h"
#include "cd.h"
#include "code_800133d8.h"

void Game_task(void)
{
    GAME_WORK *wp;

    wp                        = &Game_work;
    GAME_SCRATCH_MODE         = 2;
    *(uint32 *)&wp->routine_0 = 0;
    while (1)
    {
        Game_main_tbl[(sint8)wp->routine_0](wp);
        Work_close_apply();
        play_time_count();
        soft_reset_check();
        Thread_wait(1);
    }
}

void Game_init(GAME_WORK *wp)
{
    sint32 mode;
    sint32 i;

    mode               = wp->flags_82 & 1;
    wp->work04.value04 = 0;
    wp->work08.value08 = 0;
    wp->value68        = -1;
    wp->value6a        = -1;
    wp->value6c        = -1;
    wp->flags_81       = 0;
    wp->flags_83       = 0;
    D_800C356C         = 0;
    Work_init_all(1);
    if (mode == 0)
    {
        func_800664BC();
    }
    else
    {
        func_80065AA8();
    }
    func_8003BE40(mode);
    Sce_flag_init(mode);
    func_8003B278(mode);
    func_8005DBB4(mode);
    func_80067E6C(mode);
    Message_event_clear();
    Sound_bgm_tbl_set();
    wp->routine_0 = 1;
    wp->routine_1 = 0;
    for (i = 0; i < 16; i++)
    {
        wp->slots[i] = 0;
    }
    func_8006471C();
    Sce_flag_on(0x1FE);
    Sce_flag_on(0x38A);
    Sce_flag_on(0x3C7);
}

void Game_stage_load(GAME_WORK *wp)
{
    GAME_ENTRY_WORK *entry;

    entry = &D_800C3558;
    switch ((sint8)wp->routine_1)
    {
        case 0:
            Sce_flag_on(0x527);
            if (!(wp->flags_82 & 1) && wp->option_5b == 0)
            {
                Sce_flag_on(0x510);
                Sce_flag_on(0x522);
                Sce_flag_on(0x446);
                Sce_flag_on(0x448);
            }
            break;
        case 1:
            Sound_fade_wait();
            break;
    }
    if (entry->flags15 == 0)
    {
        wp->stage_no = entry->stage;
        wp->area_no = entry->room;
        wp->sce_no   = wp->flags_53;
        player_entry_set(&Player_work, entry, 1);
    }
    Sce_stage_load();
    wp->routine_0 = 2;
    wp->routine_1 = 1;
    wp->flags_82 &= 0xFE;
}

void Game_room_load(GAME_WORK *wp)
{
    GAME_ENTRY_WORK *entry;

    entry = &D_800C3558;
    switch ((sint8)wp->routine_1)
    {
        case 0:
            if (D_800C356D == 0)
            {
                wp->area_no = D_800C356F;
                wp->sce_no   = wp->flags_53;
                player_entry_set(&Player_work, entry, 1);
            }
            Sound_fade_wait();
            // fall through
        case 1:
            func_80038648();
            Sce_room_load();
            entry->flags14 = 0;
            wp->routine_0  = 3;
            wp->routine_1  = 0;
            break;
    }
}

void Game_play(GAME_WORK *wp)
{
    GAME_ENTRY_WORK *entry;
    PL_WORK *player;
    GAME_DELTA delta;

    Sce_update();
    Message_event_update();
    entry  = &D_800C3558;
    player = &Player_work;
    switch ((sint8)wp->routine_1)
    {
        case 0:
            if (Fade_busy == 0)
            {
                if (entry->fade_in == 255)
                {
                    wp->routine_1 += 2;
                }
                else
                {
                    Fade_start(entry->fade_in);
                    wp->routine_1++;
                }
                game_update();
                D_800A3A00 = D_800A39F0;
                Sound_bgm_start();
            }
            break;
        case 1:
            game_update();
            fade_wait_next(wp);
            break;
        case 2:
            if (entry->flags14 != 0)
            {
                if (entry->flags14 == -1)
                {
                    if (Scene_work == 0)
                    {
                        delta.y = 0;
                        delta.x = entry->x - player->position.x;
                        delta.z = entry->z - player->position.z;
                        D_800A39F0.x += delta.x;
                        D_800A39F4 += delta.y;
                        D_800A39F8 += delta.z;
                        func_80039D38(&delta);
                        Obj_work_flag_change(0x200, 0x80);
                        Work_room_change(0x1FE, &delta);
                    }
                    else
                    {
                        Work_room_change(0x1FE, NULL);
                    }
                    wp->area_no = entry->room;
                    wp->sce_no   = wp->flags_53;
                    player_entry_set(player, entry, 0);
                    Sce_room_load();
                    entry->flags14 = 0;
                }
                else
                {
                    wp->routine_1++;
                    player->flags_bd |= 0x40;
                }
            }
            else if (Scene_work == 0 && Fade_busy == 0 && !(Moji_flag & 0x0F000000) &&
                     player->value6e >= 0 && (sint8)player->flags_bd == 0 && player->kind == 1)
            {
                if ((D_800C0C2A & 8) || D_800C0C24 == 255)
                {
                    *(uint32 *)&wp->routine_0 = 4;
                    break;
                }
                if ((D_800C0C2A & 1) && wp->flags_83 == 0)
                {
                    if (D_80098004 != 0)
                    {
                        *(uint32 *)&wp->routine_0 = 7;
                        break;
                    }
                    if (wp->stage_no >= 3 && wp->stage_no != 26)
                    {
                        *(uint32 *)&wp->routine_0 = 8;
                        break;
                    }
                }
            }
            game_update();
            if (wp->flags_81 != 0)
            {
                if (wp->flags_81 == 1)
                {
                    *(uint32 *)&wp->routine_0 = 5;
                }
                if (wp->flags_81 == 2)
                {
                    *(uint32 *)&wp->routine_0 = 6;
                }
                wp->flags_81 = 0;
            }
            break;
        case 3:
            game_update();
            Sound_bgm_change_check();
            if (entry->fade_out == 255)
            {
                wp->routine_1 += 2;
                break;
            }
            if (entry->fade_out < 16)
            {
                wp->routine_1++;
            }
            else
            {
                player->value110 = 0;
                player->value111 = 0;
                player->flags06 |= 3;
                wp->routine_1 += 2;
            }
            Fade_start(entry->fade_out);
            break;
        case 4:
            game_update();
            fade_wait_next2(wp);
            break;
        case 5:
            game_update();
            entry->flags14++;
            wp->routine_1++;
            break;
        case 6:
            if (entry->flags14 == 3)
            {
                if (wp->stage_no != entry->stage)
                {
                    wp->routine_0 = 1;
                    wp->routine_1 = 1;
                }
                else
                {
                    wp->routine_0 = 2;
                    wp->routine_1 = 0;
                }
                func_8003B140();
                MojiTaskKill();
                Game_logo_kill(-1);
                Message_event_clear();
            }
            else
            {
                game_update();
            }
            break;
        case 7:
            game_update();
            wp->routine_0 = 9;
            wp->routine_1 = 0;
            break;
    }
}

void game_update(void)
{
    sint32 index;
    CODE800133D8_LINK *link;

    func_800175D8();
    func_800679B8();
    eve_work_move();
    func_8003C07C();
    Obj_player_dist_calc();
    func_8005C2B8();
    func_80017094();
    Em_work_move();
    shl_work_move();
    effect_stage_move();
    func_80017528();
    func_80051BE4();
    func_8003C2D8();
    pl_shl_work_move();
    effect_common_move();
    func_80038894();
    Atari_check();
    func_80037F60();
    func_8005DCB4();
    func_800238B4();
    func_80023144();
    func_80027678();
    func_800175FC();
    func_80028DE0();
    index = CODE800133D8_DISPLAY_INDEX;
    link  = D_80098934;
    setaddr(D_800A4D40[index], getaddr(&link->tag));
    setaddr(&link->tag, &D_800A4D40[index][0x1000] - 1);
}

void player_entry_set(PL_WORK *wp, GAME_ENTRY_WORK *entry, sint32 reset)
{
    wp->position.x = wp->position_prev.x = entry->x;
    wp->position.y = wp->position_prev.y = entry->y;
    wp->position.z = wp->position_prev.z = entry->z;
    wp->position_fixed.x = wp->position_fixed_prev.x = entry->x << 16;
    wp->position_fixed.y = wp->position_fixed_prev.y = entry->y << 16;
    wp->position_fixed.z = wp->position_fixed_prev.z = entry->z << 16;
    wp->rotation.y = wp->rotation_prev.y = entry->angle;
    if (reset)
    {
        wp->value74 = wp->value75 = 0;
    }
    if (wp->kind == 3)
    {
        wp->position_saved       = wp->position;
        wp->position_saved_fixed = wp->position_fixed;
        wp->rotation_saved       = wp->rotation;
    }
}

void Game_sub_screen(GAME_WORK *wp)
{
    PL_WORK *player;

    player = &Player_work;
    switch ((sint8)wp->routine_1)
    {
        case 0:
            if (CODE800133D8_DISPLAY_INDEX != 0)
            {
                D_80098810 = 1;
                D_80098864 = 1;
                MojiTaskKill();
                Game_logo_init();
                Sound_key_off_voice(0x10);
                Sound_key_off_voice(0x16);
                Sound_key_off_voice(0x17);
                Fade_start(0);
                wp->routine_1++;
            }
            break;
        case 1:
            D_80098810 = 0;
            func_8005EBE4();
            Cd_subcmd(0, 1, 0);
            wp->routine_1++;
            break;
        case 2:
            wp->routine_1 += func_8005EC34();
            break;
        case 3:
            D_80098810 = 1;
            D_80098864 = 0;
            Sce_pl_parts_load();
            wp->routine_1++;
            break;
        case 4:
            if (Cd_read_sync2() == 0)
            {
                D_80098810 = 0;
                Sub_screen_sound_reinit(player);
                player->flags_bd &= 0xBF;
                MojiTaskKill();
                Game_logo_kill(-1);
                wp->routine_0 = 3;
                wp->routine_1 = 1;
            }
            break;
    }
}

void Game_support(GAME_WORK *wp)
{
    PL_WORK *player;
    sint32 sound;
    sint32 result;

    player = &Player_work;
    switch ((sint8)wp->routine_1)
    {
        case 0:
            player->flags_bd |= 0x40;
            if (Sound_fade_flags == 0)
            {
                if (*(uint16 *)&Game_work.stage_no == 0x603 ||
                    *(uint16 *)&Game_work.stage_no == 0xB13 ||
                    *(uint16 *)&Game_work.stage_no == 0x61B)
                {
                    sound = 0xCF;
                }
                else
                {
                    sound = 0xCE;
                }
                Cd_seek_file(sound);
                Fade_start(0x17);
                Sound_fade_start(0x10);
                wp->routine_1++;
            }
            game_update();
            break;
        case 1:
            if (D_80098810 == 0)
            {
                game_update();
            }
            else
            {
                wp->routine_1++;
            }
            break;
        case 2:
            if (Sound_fade_flags == 0)
            {
                Sound_seq_stop_all();
                func_8001B314();
                MojiTaskKill();
                Game_logo_init();
                if (*(uint16 *)&Game_work.stage_no == 0x603 ||
                    *(uint16 *)&Game_work.stage_no == 0xB13 ||
                    *(uint16 *)&Game_work.stage_no == 0x61B)
                {
                    sound = 0xCF;
                }
                else
                {
                    sound = 0xCE;
                }
                Cd_read_comb(sound);
                wp->routine_1++;
            }
            break;
        case 3:
            if (Cd_read_sync2() == 0)
            {
                func_801D8130();
                Light_player_set();
                wp->routine_1++;
            }
            break;
        case 4:
            if (Fade_busy != 0)
            {
                break;
            }
            Fade_start(1);
            wp->routine_1++;
            // fall through
        case 5:
            result = func_801D816C();
            if (result != 0)
            {
                if (result > 0)
                {
                    Cd_seek_file(0x17);
                    support_warp_set(result);
                    Sound_fade_start(0x10);
                    Fade_start(0x15);
                    wp->routine_1++;
                }
                else
                {
                    wp->routine_1 = 2;
                }
            }
            break;
        case 6:
            if (Sound_fade_flags == 0)
            {
                MojiTaskKill();
                Game_logo_kill(-1);
                Cd_read_comb(0x17);
                wp->routine_1++;
            }
            break;
        case 7:
            if (Fade_busy == 0 && Cd_read_sync2() == 0)
            {
                player->flags_bd &= 0xBF;
                func_80038648();
                if (D_800C356C == 0)
                {
                    Fade_start(1);
                    wp->routine_0 = 3;
                    wp->routine_1 = 1;
                    Sound_bgm_start();
                }
                else
                {
                    wp->routine_0 = 3;
                    wp->routine_1 = 6;
                }
            }
            break;
    }
}

void support_warp_set(sint32 mode)
{
    if (mode == 2)
    {
        D_800C356C         = 3;
        D_800C356E         = 24;
        D_800C3562         = -1;
        D_800C3570         = 255;
        Game_work.flags_79 = 27;
        D_800C356F         = 0;
        D_800C3560         = 0;
        D_800C3564         = 0;
        D_800C356A         = 0;
        D_800C356D         = 0;
        Game_work.flags_7a = 1;
    }
}

void Game_mem_card(GAME_WORK *wp)
{
    switch ((sint8)wp->routine_1)
    {
        case 0:
            wp->routine_1 += func_800665FC(1);
            break;
        case 1:
            wp->routine_0 = 3;
            wp->routine_1 = 2;
            break;
    }
    game_update();
}

void Game_auto_map(GAME_WORK *wp)
{
    switch ((sint8)wp->routine_1)
    {
        case 0:
            func_8005DF3C();
            PLAYER_WORK_PTR->flags_bd |= 0x40;
            MojiTaskKill();
            Game_logo_init();
            Fade_start(0);
            Sound_key_off_voice(0x10);
            Sound_key_off_voice(0x16);
            Sound_key_off_voice(0x17);
            wp->routine_1++;
            break;
        case 1:
            func_8005DF4C();
            if ((D_800C0C2A & 1) || (D_800C0C2A & 0x1000))
            {
                Sub_screen_sound_reinit(&Player_work);
                PLAYER_WORK_PTR->flags_bd &= 0xBF;
                MojiTaskKill();
                Game_logo_kill(-1);
                wp->routine_0 = 3;
                wp->routine_1 = 1;
            }
            break;
    }
}

void Game_map_screen(GAME_WORK *wp)
{
    PL_WORK *player;

    player = &Player_work;
    switch ((sint8)wp->routine_1)
    {
        case 0:
            player->flags_bd |= 0x40;
            MojiTaskKill();
            Game_logo_init();
            Fade_start(0);
            Sound_key_off_voice(0x10);
            Sound_key_off_voice(0x16);
            Sound_key_off_voice(0x17);
            Cd_read_comb((sint8)wp->sce_no < 9 ? 0x21 : 0x23);
            wp->routine_1++;
            break;
        case 1:
            if (Cd_read_sync2() == 0)
            {
                Map_screen_init();
                wp->routine_1++;
            }
            break;
        case 2:
            if (Map_screen_task())
            {
                player->flags_bd &= 0xBF;
                MojiTaskKill();
                Game_logo_kill(-1);
                wp->routine_1++;
            }
            break;
        case 3:
            Cd_read_comb(0x15);
            wp->routine_1++;
            break;
        case 4:
            if (Cd_read_sync2() == 0)
            {
                Sub_screen_sound_reinit(player);
                wp->routine_0 = 3;
                wp->routine_1 = 1;
            }
            break;
    }
}

void Game_over(GAME_WORK *wp)
{
    switch ((sint8)wp->routine_1)
    {
        case 0:
            GAME_SCRATCH_MODE = 4;
            game_update();
            Cd_seek_file(0x19);
            Fade_start(11);
            wp->work08.transition_aperture = 120;
            wp->routine_1++;
            break;
        case 1:
            game_update();
            shutter_draw();
            if ((wp->work08.transition_aperture -= 2) < 0)
            {
                wp->work08.transition_aperture = 0;
            }
            goto wait;
        case 2:
            GAME_SCRATCH_MODE = 2;
            D_80098A70        = 0;
            MojiTaskKill();
            Message_event_clear();
            Game_logo_init();
            Cd_subcmd(0, 1, 0);
            Sound_seq_stop_all();
            func_8001B314();
            Cd_read_comb(0x19);
            Cd_read_sync();
            wp->work04.words.timer06 = 150;
            wp->routine_1++;
#ifdef PORT
            Sound_call(0x41, 0, 0);
#else
            // Retail passes no a1/a2 (whatever Cd_read_sync left); the port passes 0, 0.
            Sound_call(0x41);
#endif
            Fade_start(2);
            break;
        case 3:
            game_over_draw();
            fade_wait_next(wp);
            break;
        case 4:
            game_over_draw();
            if ((D_800C0C2A & 8) || Sound_seq_end_check(0) == 0)
            {
                wp->routine_1++;
                Sound_fade_start(0x10);
                Fade_start(10);
            }
            break;
        case 5:
            game_over_draw();
        wait:
            if (Fade_busy == 0 && Sound_fade_flags == 0)
            {
                wp->routine_1++;
            }
            break;
        case 6:
            Sce_flag_off(0x1FE);
            *(uint16 *)&Code800133D8_work.routine_0 = 1;
            Thread_change(Title_task);
            break;
    }
}

// Same as Game_over except the first fade (15, not 11).
void Game_over2(GAME_WORK *wp)
{
    switch ((sint8)wp->routine_1)
    {
        case 0:
            GAME_SCRATCH_MODE = 4;
            game_update();
            Cd_seek_file(0x19);
            Fade_start(15);
            wp->work08.transition_aperture = 120;
            wp->routine_1++;
            break;
        case 1:
            game_update();
            shutter_draw();
            if ((wp->work08.transition_aperture -= 2) < 0)
            {
                wp->work08.transition_aperture = 0;
            }
            goto wait;
        case 2:
            GAME_SCRATCH_MODE = 2;
            D_80098A70        = 0;
            MojiTaskKill();
            Message_event_clear();
            Game_logo_init();
            Cd_subcmd(0, 1, 0);
            Sound_seq_stop_all();
            func_8001B314();
            Cd_read_comb(0x19);
            Cd_read_sync();
            wp->work04.words.timer06 = 150;
            wp->routine_1++;
#ifdef PORT
            Sound_call(0x41, 0, 0);
#else
            // Retail passes no a1/a2 (whatever Cd_read_sync left); the port passes 0, 0.
            Sound_call(0x41);
#endif
            Fade_start(2);
            break;
        case 3:
            game_over_draw();
            fade_wait_next(wp);
            break;
        case 4:
            game_over_draw();
            if ((D_800C0C2A & 8) || Sound_seq_end_check(0) == 0)
            {
                wp->routine_1++;
                Sound_fade_start(0x10);
                Fade_start(10);
            }
            break;
        case 5:
            game_over_draw();
        wait:
            if (Fade_busy == 0 && Sound_fade_flags == 0)
            {
                wp->routine_1++;
            }
            break;
        case 6:
            Sce_flag_off(0x1FE);
            *(uint16 *)&Code800133D8_work.routine_0 = 1;
            Thread_change(Title_task);
            break;
    }
}

void play_time_count(void)
{
    if (++Game_work.play_time > GAME_TIMER_MAX)
    {
        Game_work.play_time = GAME_TIMER_MAX;
    }
}

void soft_reset_check(void)
{
    if ((sint8)Game_work.routine_0 >= 3 && (sint8)Game_work.routine_0 != 6 && D_80098910 == 0)
    {
        if ((D_800C0C26 & 9) == 9)
        {
            if (Game_work.work08.words.value0a++ >= 61)
            {
                GAME_SCRATCH_MODE = 2;
                MojiTaskKill();
                Message_event_clear();
                Game_logo_init();
                func_800665E8();
                func_8001B314();
                Sound_seq_stop_all();
                Cd_subcmd(0, 1, 0);
                D_80098A70 = 0;
                Fade_start(8);
                Sce_flag_off(0x1FE);
                *(uint16 *)&Code800133D8_work.routine_0 = 1;
                Thread_change(Title_task);
            }
        }
        else
        {
            Game_work.work08.words.value0a = 0;
        }
    }
}

void fade_wait_next(GAME_WORK *wp)
{
    if (Fade_busy == 0)
    {
        wp->routine_1++;
    }
}

// Same code as fade_wait_next.
void fade_wait_next2(GAME_WORK *wp)
{
    if (Fade_busy == 0)
    {
        wp->routine_1++;
    }
}

void Sound_fade_wait(void)
{
    while (Sound_fade_flags != 0)
    {
        Thread_wait(1);
    }
    func_8001B314();
}

void game_over_draw(void)
{
    POLY_FT4 *p;

    p                        = *CODE800133D8_PACKET_PTR;
    *CODE800133D8_PACKET_PTR = p + 1;
    setPolyFT4(p);
    p->tpage = GetTPage(0, 0, 320, 256);
    p->clut  = GetClut(0, 496);
    p->x0    = 32;
    p->y0    = 56;
    p->x1    = 287;
    p->y1    = 56;
    p->x2    = 32;
    p->y2    = 184;
    p->x3    = 287;
    p->y3    = 184;
    p->u0    = 0;
    p->v0    = 0;
    p->u1    = 255;
    p->v1    = 0;
    p->u2    = 0;
    p->v2    = 128;
    p->u3    = 255;
    p->v3    = 128;
    p->r0    = 128;
    p->g0    = 128;
    p->b0    = 128;
    AddPrim(&D_80098934->tag78, p);
}

void shutter_draw(void)
{
    POLY_FT4 *packet;
    POLY_F4 *p;

    packet = *CODE800133D8_PACKET_PTR;
    p      = (POLY_F4 *)packet;
    setPolyF4(p);
    p->x0 = 0;
    p->y0 = 0;
    p->x1 = 320;
    p->y1 = 0;
    p->x2 = 0;
    p->y2 = 120 - GAME_WORK_PTR->work08.aperture;
    p->x3 = 320;
    p->y3 = 120 - GAME_WORK_PTR->work08.aperture;
    p->r0 = 0;
    p->g0 = 0;
    p->b0 = 0;
    p->code &= ~2;
    AddPrim(&D_80098934->tag74, packet++);
    p = (POLY_F4 *)packet;
    setPolyF4(p);
    p->x0 = 0;
    p->y0 = 119 + GAME_WORK_PTR->work08.aperture;
    p->x1 = 320;
    p->y1 = 119 + GAME_WORK_PTR->work08.aperture;
    p->x2 = 0;
    p->y2 = 240;
    p->x3 = 320;
    p->y3 = 240;
    p->r0 = 0;
    p->g0 = 0;
    p->b0 = 0;
    p->code &= ~2;
    AddPrim(&D_80098934->tag74, packet++);
    *CODE800133D8_PACKET_PTR = packet;
}

void Em_work_move(void)
{
    for (Em_work_cur = Em_work_tbl; (uint32)Em_work_cur < (uint32)&Em_work_tbl[16]; Em_work_cur++)
    {
        if (Em_work_cur->flags & Em_work_cur->active & 1)
        {
            Enemy_move_ptr[Em_work_cur->kind][Em_work_cur->id2](Em_work_cur);
        }
    }
}

void func_80017094(void)
{
    for (D_800989D0 = D_800C3580; (uint32)D_800989D0 < (uint32)&D_800C3580[4]; D_800989D0++)
    {
        if (D_800989D0->flags & D_800989D0->active & 1)
        {
            GAME_TABLE_5A4[D_800989D0->kind][D_800989D0->id2](D_800989D0);
        }
    }
}

void pl_shl_work_move(void)
{
    for (Shl_work_cur = Pl_shl_work_tbl; (uint32)Shl_work_cur < (uint32)&Pl_shl_work_tbl[16]; Shl_work_cur++)
    {
        if (Shl_work_cur->flags & Shl_work_cur->active & 1)
        {
            D_80089820[Shl_work_cur->kind](Shl_work_cur);
        }
    }
}

void shl_work_move(void)
{
    for (Shl_work_cur = Shl_work_tbl; (uint32)Shl_work_cur < (uint32)&Shl_work_tbl[32]; Shl_work_cur++)
    {
        if (Shl_work_cur->flags & Shl_work_cur->active & 1)
        {
            if (Shl_work_cur->kind < 0x80)
            {
                GAME_TABLE_10C[Shl_work_cur->kind](Shl_work_cur);
            }
            else
            {
                ((GAME_FUNC_10C_RET)D_80089820[Shl_work_cur->kind])(Shl_work_cur);
            }
        }
    }
}

void effect_stage_move(void)
{
    for (Effect_work_cur = Effect_work_tbl; (uint32)Effect_work_cur < (uint32)&Effect_work_tbl[64]; Effect_work_cur++)
    {
        if (Effect_work_cur->flags & Effect_work_cur->active & 1)
        {
            if (Effect_work_cur->kind < 0x80)
            {
                GAME_TABLE_48[Effect_work_cur->kind](Effect_work_cur);
            }
        }
    }
}

void effect_common_move(void)
{
    for (Effect_work_cur = Effect_work_tbl; (uint32)Effect_work_cur < (uint32)&Effect_work_tbl[64]; Effect_work_cur++)
    {
        if (Effect_work_cur->flags & Effect_work_cur->active & 1)
        {
            if (Effect_work_cur->kind >= 0x80)
            {
                D_80089858[Effect_work_cur->kind](Effect_work_cur);
            }
        }
    }
}

void eve_work_move(void)
{
    for (Eve_work_cur = Eve_work_tbl; (uint32)Eve_work_cur < (uint32)&Eve_work_tbl[16]; Eve_work_cur++)
    {
        if (Eve_work_cur->flags & Eve_work_cur->active & 1)
        {
            GAME_TABLE_24[Eve_work_cur->kind](Eve_work_cur);
        }
    }
}

void func_80017528(void)
{
    for (D_80098924 = D_800BF118; (uint32)D_80098924 < (uint32)&D_800BF118[32]; D_80098924++)
    {
        if (D_80098924->flags & D_80098924->active & 1)
        {
            GAME_TABLE_50[D_80098924->kind](D_80098924);
        }
    }
}
