#ifndef SYSTEM_H
#define SYSTEM_H

#include "common.h"
#include "thread.h"

#define SYSTEM_EVENT_CLASS 0xF0000010

typedef void (*SYSTEM_STATE_FUNC)(sint8 *state);

extern THREAD_FUNC Thread_change_entry;
extern sint32 Thread_change_flag;
extern long D_800988D8;
extern sint8 Main_routine;
extern sint8 D_80098B1D;
extern SYSTEM_STATE_FUNC Main_routine_tbl[];

long exception_viewer(void);

void Thread_wait(sint32 arg);
void Thread_open(sint32 index, THREAD_FUNC entry);
void Thread_change(THREAD_FUNC entry);
void Moji_init(void);

#endif // SYSTEM_H
