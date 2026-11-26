#ifndef ROCK_NEO_GAME_H
#define ROCK_NEO_GAME_H

#include "common.h"
#include "system.h"
#include "player.h"

#define GAME_TIMER_MAX 0xA4CB7F

typedef union
{
    uint32 value08;
    uint16 aperture;
    sint16 transition_aperture;
    struct
    {
        sint16 lo;
        sint16 value0a;
    } words;
} GAME_WORD08;

typedef union
{
    uint32 value04;
    struct
    {
        uint16 lo;
        sint16 timer06;
    } words;
} GAME_WORD04;

// Sound bank slot (eight-byte records from +0x1B8)
typedef struct
{
    sint16 vab; // PsyQ VAB id (-1 = none)
    uint8 id;
    uint8 reserved03[5];
} GAME_SOUND_SLOT;

// Only fields observed in this subsystem are described.
typedef struct
{
    uint8 routine_0;
    uint8 routine_1;
    uint8 routine_2;
    uint8 routine_3;
    GAME_WORD04 work04;
    GAME_WORD08 work08;
    uint32 play_time; // frames played (30 per second), capped at GAME_TIMER_MAX (99:59:59)
    uint32 value10;
    uint32 value14;
    uint16 flags_18;
    uint16 flags_1a;
    uint8 reserved1c[0x34];
    sint8 stage_no;
    uint8 area_no;
    uint8 sce_no; // scenario number (Capcom: sce_no)
    uint8 flags_53;
    uint8 flags_54;
    uint8 flags_55;
    uint8 reserved56[2];
    uint8 flags_58;
    uint8 flags_59;
    uint8 reserved5a;
    sint8 option_5b;
    uint8 reserved5c[4];
    sint16 timer60;
    uint8 reserved62[6];
    sint16 value68;
    sint16 value6a;
    sint16 value6c;
    uint8 reserved6e[2];
    sint16 value70[4];
    uint8 reserved78;
    uint8 flags_79;
    uint8 flags_7a;
    uint8 reserved7b[6];
    sint8 flags_81;
    sint8 flags_82;
    uint8 flags_83;
    uint8 reserved84[0xF0];
    uint32 slots[16];
    uint8 reserved1b4[4];
    GAME_SOUND_SLOT sound_1b8[5];
    uint8 reserved1e0[0x30];
} GAME_WORK;

typedef struct
{
    uint8 reserved00[8];
    sint16 x, y, z;
    uint8 reserved0e[4];
    sint16 angle;
    sint8 flags14;
    sint8 flags15;
    uint8 stage;
    uint8 room;
    uint8 fade_in, fade_out;
    uint8 reserved1a[0xE];
} GAME_ENTRY_WORK;

// Dispatch records: observed common prefix and each array stride. GAME_WORK_3C0 is Capcom's
// EM_WORK (Em_work_tbl, Open_EM_WORK/Close_EM_WORK); GAME_WORK_24 is EVE_WORK (Eve_work_tbl).
typedef struct
{
    sint8 flags;
    uint8 reserved01;
    uint8 kind;
    uint8 reserved03;
    uint8 id2; // Capcom: id2, the second index into Enemy_move_ptr
    uint8 reserved05;
    uint8 active;
    uint8 reserved07[0x3B9];
} GAME_WORK_3C0;
typedef void (*GAME_FUNC_3C0)(GAME_WORK_3C0 *wp);

typedef struct
{
    sint8 flags;
    uint8 reserved01;
    uint8 kind;
    uint8 reserved03;
    uint8 id2;
    uint8 reserved05;
    uint8 active;
    uint8 reserved07[0x59D];
} GAME_WORK_5A4;
typedef void (*GAME_FUNC_5A4)(GAME_WORK_5A4 *wp);

typedef struct
{
    sint8 flags;
    uint8 reserved01;
    uint8 kind;
    uint8 reserved03;
    uint8 routine;
    uint8 reserved05;
    uint8 active;
    uint8 reserved07[0x105];
} GAME_WORK_10C;
typedef void (*GAME_FUNC_10C)(GAME_WORK_10C *wp);
// The overflow table D_80089820 is called as returning a value (keeps its call distinct
// from the GAME_TABLE_10C call in shl_work_move).
typedef sint32 (*GAME_FUNC_10C_RET)(GAME_WORK_10C *wp);

typedef struct
{
    sint8 flags;
    uint8 reserved01;
    uint8 kind;
    uint8 reserved03;
    uint8 routine;
    uint8 reserved05;
    uint8 active;
    uint8 reserved07[0x41];
} GAME_WORK_48;
typedef void (*GAME_FUNC_48)(GAME_WORK_48 *wp);

typedef struct
{
    sint8 flags;
    uint8 reserved01;
    uint8 kind;
    uint8 reserved03;
    uint8 routine;
    uint8 reserved05;
    uint8 active;
    uint8 reserved07[0x1D];
} GAME_WORK_24;
typedef void (*GAME_FUNC_24)(GAME_WORK_24 *wp);

typedef struct
{
    sint8 flags;
    uint8 reserved01;
    uint8 kind;
    uint8 reserved03;
    uint8 routine;
    uint8 reserved05;
    uint8 active;
    uint8 reserved07[0x49];
} GAME_WORK_50;
typedef void (*GAME_FUNC_50)(GAME_WORK_50 *wp);

typedef void (*GAME_FUNC)(GAME_WORK *wp);

#define GAME_SCRATCH_MODE (*(uint8 *)0x1F800001)

// The stage's enemy handlers, Enemy_move_ptr[id][id2] (Capcom's name, from em1c_move_erase).
#define Enemy_move_ptr (*(GAME_FUNC_3C0 ***)0x1F800058)
#define GAME_TABLE_5A4 (*(GAME_FUNC_5A4 ***)0x1F80005C)
#define GAME_TABLE_10C (*(GAME_FUNC_10C **)0x1F800060)
#define GAME_TABLE_48  (*(GAME_FUNC_48 **)0x1F800064)
#define GAME_TABLE_24  (*(GAME_FUNC_24 **)0x1F800068)
#define GAME_TABLE_50  (*(GAME_FUNC_50 **)0x1F80006C)

typedef struct
{
    sint16 x, y, z;
} GAME_DELTA;

extern PL_FIXED_VECTOR D_800A39F0;
extern PL_FIXED_VECTOR D_800A3A00;
extern sint32 D_800A39F4;
extern sint32 D_800A39F8;
extern sint32 D_80098004;
extern uint8 D_800C0C24;
extern uint8 Scene_work;
extern GAME_WORK_3C0 Em_work_tbl[16];
extern GAME_WORK_3C0 *Em_work_cur;
extern GAME_WORK_5A4 Work_5A4_tbl[4];
extern GAME_WORK_5A4 *Work_5A4_cur;
extern GAME_WORK_10C Pl_shl_work_tbl[16];
extern GAME_WORK_10C Shl_work_tbl[32];
extern GAME_WORK_10C *Shl_work_cur;
extern GAME_FUNC_10C D_80089820[];
extern GAME_WORK_48 Effect_work_tbl[64];
extern GAME_WORK_48 *Effect_work_cur;
extern GAME_FUNC_48 D_80089858[];
extern GAME_WORK_24 Eve_work_tbl[16];
extern GAME_WORK_24 *Eve_work_cur;
extern GAME_WORK_50 Work_50_tbl[32];
extern GAME_WORK_50 *Work_50_cur;

extern GAME_FUNC Game_main_tbl[];
extern GAME_WORK Game_work;
extern GAME_ENTRY_WORK D_800C3558;
extern sint16 Fade_busy;
extern uint8 Sound_fade_flags;
extern uint16 D_80098910;
extern sint16 D_800C3560;
extern sint16 D_800C3562;
extern sint16 D_800C3564;
extern sint16 D_800C356A;
extern sint8 D_800C356C;
extern sint8 D_800C356D;
extern uint8 D_800C356E;
extern uint8 D_800C356F;
extern uint8 D_800C3570;

sint32 Card_screen_exec(sint32 arg);
void game_update(void);
void Sound_se_stop_all(void);
void Work_close_apply(void);
void play_time_count(void);
void soft_reset_check(void);

void Sound_fade_wait(void);
void player_entry_set(PL_WORK *wp, GAME_ENTRY_WORK *entry, sint32 reset);
void Sce_flag_on(sint32 flag);
#ifdef __wasm32__
sint32 Sce_stage_load(void);
#else
void Sce_stage_load(void);
#endif
#ifdef __wasm32__
sint32 Sce_room_load(void);
#else
void Sce_room_load(void);
#endif
void Camera_init(void);
void Sce_flag_init(sint32 mode);
void Map_obj_state_reset(sint32 mode);
void Player_init(sint32 mode);
void Automap_reset(sint32 mode);
void Records_init(sint32 mode);
void Param_tbl_select(void);
void Game_work_load(void);
void Game_work_new(void);
void Work_init_all(sint32 mode);
void Message_event_clear(void);
void Sound_bgm_tbl_set(void);

void Sound_seq_stop_all(void);
void Card_screen_init(void);
void Sce_flag_off(sint32 flag);
void Cd_subcmd(sint32 arg0, sint32 arg1, sint32 arg2);

void Automap_scroll_reset(void);
void Automap_screen_update(void);
void Sound_key_off_voice(sint32 voice);
void Sub_screen_sound_reinit(PL_WORK *wp);
void Game_logo_kill(sint32 index);

void Sub_screen_open(void);
sint32 Sub_screen_main(void);
void Sce_pl_parts_load(void);

void func_800175D8(void);
void Play_time_label_set(void);
void eve_work_move(void);
void Player_move(void);
void Obj_player_dist_calc(void);
void Gauge_update(void);
void work_5a4_move(void);
void Em_work_move(void);
void shl_work_move(void);
void effect_stage_move(void);
void work_50_move(void);
void Solid_check(void);
void Player_atari_set(void);
void pl_shl_work_move(void);
void effect_common_move(void);
void Camera_update(void);
void Atari_check(void);
void Map_obj_break_update(void);
void Automap_update(void);
void Map_draw(void);
void Map_layer_draw(void);
void Work_draw_all(void);
void func_800175FC(void);
void Em_work_draw(void);
void Map_screen_init(void);
sint32 Map_screen_task(void);

void Sound_fade_start(sint32 arg);
sint32 Sound_seq_end_check(sint32 arg);
void game_over_draw(void);
void shutter_draw(void);
void fade_wait_next(GAME_WORK *wp);
void fade_wait_next2(GAME_WORK *wp);
void support_warp_set(sint32 mode);
void Sound_bgm_start(void);
void Light_player_set(void);
void func_801D8130(void);
sint32 func_801D816C(void);

void Sce_update(void);
void Message_event_update(void);
void Camera_pos_shift(GAME_DELTA *delta);
void Obj_work_flag_change(sint32 mask, sint32 flags);
void Work_room_change(sint32 flag, GAME_DELTA *delta);
void Sound_bgm_change_check(void);
void Map_obj_state_save(void);

// Game_work by its fixed address: GCC 2.7 rebuilds a literal address with lui at each
// access instead of keeping the symbol's address in a register (Sound_bgm_start).
#define GAME_WORK_PTR ((GAME_WORK *)0x800C1B10)

#endif
