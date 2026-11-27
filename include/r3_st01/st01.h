#ifndef ST01_H
#define ST01_H

#include "common.h"
#include "stage_common/stage.h"

extern STAGE_SLOT_FUNC *D_80100C88[];
extern STAGE_SLOT_FUNC *D_80100CC8[];
extern STAGE_SLOT_FUNC D_80100CCC[];
extern STAGE_SLOT_FUNC D_80100CD0[];
extern STAGE_SLOT_FUNC D_80100CD4[];
extern STAGE_SLOT_FUNC D_80100D58[];
extern void *D_80100D5C[];       // model table by room (Map_layer_set)
extern void *D_80100D88[];       // spawn list by room
extern void *D_80100DA4[];       // exit list by room
extern uint8 D_80100DA8[][0x28]; // by room (Fog_room_set)
extern uint8 D_80100E20[][8];    // by D_80100EF8[room]
extern sint32 D_80100EF8[];
extern STAGE_ROOM_FUNC st01_room_script_tbl[]; // room scripts by room
extern uint8 D_80100F1C[];
extern uint8 D_80100F30[];
extern STAGE_ROOM_FUNC D_80100F7C[];
extern STAGE_FUNC D_80100F8C[];      // routine_1 handlers (func_8010080C)
extern STAGE_FUNC st01_scroll_tbl[]; // routine_0 handlers (st01_scroll_move)

// Room model 0's x (STAGE_MODEL_TBL[0]), reached through its own symbol.
extern sint16 D_800BBA9C[];

void st01_event_init(STAGE_ROOM_WORK *wp);
void st01_event_step_call(STAGE_ROOM_WORK *wp);
void st01_scroll_line_draw(void *wp, sint32 a1, sint32 y, uint32 color);

#endif // ST01_H
