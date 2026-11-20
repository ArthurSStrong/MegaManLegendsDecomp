// Included first in every C file of the Windows build (mingw-w64.cmake: -include). Brings in
// windows.h once, before the port's headers, and undoes what clashes with them; then fills the
// few POSIX gaps the runtime meets on MinGW-w64.
#ifndef PORT_WIN32_H
#define PORT_WIN32_H
#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN // no winsock: its s_addr macro breaks PSX_EXE_INFO.s_addr
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
// PsyQ functions the game calls by these names (LoadImage, OpenEvent): not the Win32 macros.
#undef LoadImage
#undef OpenEvent
// libetc's callback typedef, not the calling-convention macro (port code uses WINAPI).
#undef CALLBACK
// PsyQ's EnterCriticalSection(void) is not Win32's: every port file uses this name for it.
#define EnterCriticalSection psyq_EnterCriticalSection

#include <direct.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <sys/stat.h>

// POSIX mkdir(path, mode): Windows has no modes.
#define mkdir(path, mode) _mkdir(path)
// No close-on-exec here; every open() that asks for it reads or writes binary data (memory
// cards), which Windows must be told about or it translates line ends.
#ifndef O_CLOEXEC
#define O_CLOEXEC O_BINARY
#endif

// rename() over an existing file fails on Windows; the port saves through a temporary file and
// a rename (memory cards, the settings file), which POSIX makes replace the target.
static inline int port_win32_rename(const char *from, const char *to)
{
    return MoveFileExA(from, to, MOVEFILE_REPLACE_EXISTING) ? 0 : -1;
}
#define rename(from, to) port_win32_rename(from, to)

#include <stdint.h>
#include <unistd.h>

// pread: read at an offset (the disc image). ReadFile with an offset; the game is one thread.
static inline long port_win32_pread(int fd, void *buf, size_t n, int64_t offset)
{
    HANDLE h     = (HANDLE)_get_osfhandle(fd);
    OVERLAPPED o = {0};
    DWORD got    = 0;

    o.Offset     = (DWORD)offset;
    o.OffsetHigh = (DWORD)((uint64_t)offset >> 32);
    if (!ReadFile(h, buf, (DWORD)n, &got, &o))
    {
        return GetLastError() == ERROR_HANDLE_EOF ? 0 : -1;
    }
    return (long)got;
}
#define pread(fd, buf, n, offset) port_win32_pread(fd, buf, n, offset)
#define fsync(fd)                 _commit(fd)

#endif
#endif
