#ifndef CD_LOCAL_H
#define CD_LOCAL_H

// rock_neo.exe-specific: CD variables with no name yet. Included at the end of cd.h.
// The slus_006.03 counterparts (shim names in comments) are in include/slus_006.03/cd_local.h.

#include "common.h"
#include "psyq/libcd.h"
#include "psyq/libgpu.h"

#define CD_CMD_TIMEOUT       150
#define CD_ERR_CMD_TIMEOUT   0x02
#define CD_ERR_FILE_ID       0x20
#define CD_ERR_SECTOR        0x40
#define CD_ERR_BUFFER_FULL   0x01
#define CD_SECTOR_BYTES      2048
#define CD_SECTOR_WORDS      512
#define CD_READ_BUFFER_COUNT 10

typedef struct
{
    uint32 offset; // sector offset within the file (low 24 bits)
    uint32 unk4;
} CD_POS;

// A command slot with its three arguments as words (a struct field, unlike a cast of args.arg3).
typedef struct
{
    sint32 type;
    uint32 arg[3];
} CD_CMD_RAW;

extern uint8 Cd_play_mode;         // D_80059BC9
extern uint8 Cd_muted;         // D_80059BCA
extern sint32 Cd_ring_write;        // D_80059C00
extern sint32 Cd_head_state;        // D_80059C24
extern uint8 D_80098AA0;         // D_80059C48
extern uint8 Cd_xa_state;         // D_80059C74
extern CD_CMD_FUNC cd_subcmd_tbl[]; // run by cd_cmd_subcmd, indexed by the command's arg2

// Read setup and callbacks; widths verified against the read-ready handlers.
extern uint8 *Cd_read_buf;                      // next destination byte
extern CdlLOC D_80098814;                      // requested sector in BCD
extern sint32 D_8009881C;                      // bytes remaining
extern sint32 D_80098A7C;                      // next expected absolute sector
extern uint8 Cd_ring_ready[CD_READ_BUFFER_COUNT]; // ten sector-buffer occupancy flags

extern uint8 Cd_ring_buf[]; // sector header scratch and buffered sector data

// Packed playback references and command-dispatch state.
extern uint32 Cd_xa_end_sector;   // playback end sector
extern uint32 Cd_xa_id;   // packed reference retained for repeat playback
extern uint32 D_800989BC;   // saved command argument 2
extern uint32 Cd_xa_flags;   // saved final command word, including repeat bit
extern uint32 Cd_xa_sector;   // current playback sector
extern uint8 D_80098A9D[3]; // absolute M:S:F bytes at Cd_result + 5
extern CD_CMD_FUNC cd_cmd_tbl[];

// Unknown
extern uint16 Cd_mute_save;
extern uint8 D_80098AB8;
extern sint32 *Cd_xa_file_tbl[16]; // file index tables, by the top nibble of Cd_xa_seek_id's arg
extern CD_POS **Cd_xa_track_tbl[16]; // matching sector offset tables

void Cd_set_volume(uint8 vol);
void Cd_pause(void);
void Cd_stop(void);
void Cd_read_sync(void);

// Unknown
void cd_seek_cb(u_char status, u_char *result);
void Cd_read_start_buf(sint32 index, uint8 *dest);
void Cd_ready_cb_buf(u_char status, u_char *result);
void Cd_read_start_ring(sint32 index);
void Cd_ready_cb_ring(u_char status, u_char *result);
void cd_subcmd_mute(void);
void Cd_xa_play_id(uint32 arg);
void Cd_xa_seek_id(uint32 arg);
void Cd_seek(sint32 sector);
void Cd_seek_file(sint32 index);

// Buffered asset records (cd_cmd_read_comb). Each 0x800-byte buffer starts with a record header;
// record data begins at +0x100.
typedef struct
{
    sint32 type;       // 0x00: -1 ends the list
    uint32 size;       // 0x04: bytes of data that follow
    sint32 count;      // 0x08: sectors the record spans
    uint32 arg[61];    // 0x0C: meaning depends on the type
    uint8 data[0x700]; // 0x100
} CD_ASSET_REC;

// The record being loaded: header words copied from the buffer, then load progress.
typedef struct
{
    uint32 size;  // 0x00: bytes left
    sint32 count; // 0x04: sectors left
    union
    {
        uint8 *dest; // type 0: raw copy
        struct
        {
            sint32 clut_x, clut_y, clut_w, clut_h;
            sint32 x, y, w, h;
        } tim; // type 1: image streamed as 32x32 tiles
        struct
        {
            sint32 x, y, cols, rows;
        } tiles; // type 3
        struct
        {
            sint32 slot;
            sint32 id;
            sint32 arg;
            sint32 size;
            uint8 *head;
            uint8 load;
        } vab; // type 5: sound bank
        struct
        {
            sint32 slot;
            sint32 id;
            uint8 *dest;
            uint8 load;
        } seq; // type 7
        struct
        {
            sint32 slot;
            sint32 id;
            sint32 sub;
            uint8 *dest;
            uint8 load;
        } sep; // type 8
    } u;
    uint8 reserved28[8];
    sint32 col; // 0x30
    sint32 row; // 0x34
} CD_ASSET_WORK;

extern sint32 Cd_ring_read; // current sector buffer
extern sint32 D_80098A54; // type of the record being loaded
extern CD_ASSET_WORK D_800C5604;

#define CD_SECTOR_BUF(i) (&Cd_ring_buf[(i) << 11]) // sector buffer i (0x800 bytes)
extern RECT D_8009BE48[];
extern uint16 D_800AD168[]; // shadow copy of the texture pages
extern uint32 D_80098868;   // shadow rows touched

void Sound_seq_stop_all(void);
void func_80017684(void *src, void *dst, sint32 count);
void func_800176A4(void *src, void *dst, sint32 count);
void func_800176DC(void *src, void *dst, sint32 count);

#endif // CD_LOCAL_H
