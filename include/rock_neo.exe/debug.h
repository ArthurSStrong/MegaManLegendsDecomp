#ifndef ROCK_DEBUG_H
#define ROCK_DEBUG_H

#include "common.h"

#define DEBUG_MEMORY_ROWS      16
#define DEBUG_MEMORY_BASE      0x5000
#define DEBUG_MEMORY_FLAGS     0x10
#define DEBUG_MEMORY_SLOT_STEP 0x1000

typedef union
{
    uint32 words[2];
    uint16 halves[4];
    uint8 bytes[8];
} DEBUG_MEMORY_ROW;

typedef struct
{
    sint8 state_0;
    uint8 reserved1[3];
    sint8 value_4;
    uint8 reserved5;
    sint16 index_6;
} DEBUG_ENTRY;

typedef struct
{
    uint8 routine_0;
    uint8 routine_1;
    uint8 routine_2;
    uint8 routine_3;
    uint16 pad_4;
    uint16 pad_6;
    sint8 mode_8;
    sint8 sel_9;
    uint8 reserved_a[2];
    sint8 sel_c;
    sint8 value_d;
    uint8 reserved_e[2];
    DEBUG_ENTRY entry[5];
} DEBUG_WORK;

typedef sint32 (*DEBUG_FUNC)(DEBUG_WORK *wp);

typedef struct
{
    sint8 active_0;
    sint8 flag_1;
    sint8 kind_2;
    uint8 reserved3;
    sint8 value_4;
    sint8 value_5;
    uint8 reserved6[2];
    uint32 value_8;
} LOGO_WORK;

typedef void (*LOGO_FUNC)(LOGO_WORK *wp);

// Raw primitive words (tag, rgb+code, xy, uv+clut, wh)
typedef struct
{
    uint32 tag;
    uint32 rgbc;
    uint32 xy;
    uint32 uv_clut;
    uint32 wh;
} DEBUG_PRIM;

// Ordering-table words of the current display buffer, from +0x70.
#define DEBUG_OT ((uint32 *)((uint8 *)Disp_env_ptr + 0x70))

// Logo sprite record (0x10 bytes); a negative head ends the list.
typedef struct
{
    uint16 head_0; // high byte = type (signed); a negative halfword ends the list
    uint8 reserved2[2];
    uint32 xy_4;
    uint32 uv_8;
    uint32 wh_c;
} LOGO_SPRITE;

// Message event request (Message_event_set): one word at +0 (flag byte, mode byte, step half), then B4/B8.
typedef union
{
    uint32 word;
    uint8 flag;
} MESSAGE_EVENT_HEAD;

// View of the request through a pointer.
typedef struct
{
    uint8 flag_0;
    uint8 mode_1;
    uint8 reserved2[2];
    sint8 value_4;
    sint8 value_5;
    sint16 value_6;
    uint32 value_8;
} MESSAGE_EVENT;

extern MESSAGE_EVENT_HEAD Message_event;
extern uint8 D_800987B1;
extern sint16 D_800987B2;
extern uint32 D_800987B4;
extern uint32 D_800987B8;
extern DEBUG_WORK Debug_work;
extern DEBUG_FUNC Debug_mode_tbl[];
extern DEBUG_FUNC Debug_item_tbl[];
extern LOGO_FUNC Game_logo_draw_tbl[];
extern uint16 D_8008ECDC[][4];
extern uint16 D_8008ECDE[][4];
extern uint32 D_8008ECE0[][2];
extern LOGO_WORK Game_logo_work[4];
extern uint8 *D_8008CACC;
extern uint8 *D_8008CAD0;
extern uint8 D_800987D4;
extern char D_800979AC[];
extern char D_800979BC[];
extern char D_80010040[];

extern uint8 D_800987B5;
extern uint32 D_8008DD54[];
void MojiTaskExec2(uint8 a, uint8 b);
void MojiTaskKill(void);
void Message_event_update(void);
// Debug menu line: flags word then a text buffer, 0x10 bytes each, from 0x8008DCD4.
typedef struct
{
    uint32 flags;
    char text[12];
} DEBUG_MENU_LINE;
extern DEBUG_MENU_LINE Debug_weapon_left_tbl[];
extern DEBUG_MENU_LINE Debug_weapon_right_tbl[];
extern uint32 D_80098700; // held-direction repeat counter for the stage menu
sint32 Debug_item_weapon_left(DEBUG_WORK *wp);
extern DEBUG_MENU_LINE Debug_helmet_tbl[];
sint32 Debug_item_helmet(DEBUG_WORK *wp);
extern DEBUG_MENU_LINE Debug_key_type_tbl[];
sint32 Debug_item_key_type(DEBUG_WORK *wp);
extern DEBUG_MENU_LINE Debug_size_tbl[];
extern sint32 Debug_break;
sint32 Debug_item_size(DEBUG_WORK *wp);
void Pl_key_type_set(void *pl, sint32 arg);
#define DEBUG_GAME_54 (*((sint8 *)&Game_work + 0x54))
sint32 MojiTaskExec(sint32 arg0, uint8 *script, sint32 arg2);
void Obj_work_flag_change(sint32 mask, sint32 flags);
void Print(uint32 flags, const char *fmt, ...);

#endif // ROCK_DEBUG_H
