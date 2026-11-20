# CMake toolchain for a Windows build of the 64-bit port from Linux with MinGW-w64:
#   cmake -S port -B build/port-win64 -G Ninja -DPORT_BOOT64=ON -DCMAKE_BUILD_TYPE=Release \
#         -DCMAKE_TOOLCHAIN_FILE=port/windows/mingw-w64.cmake -DPORT_SDL3_MINGW=/path/to/SDL3-x.y.z
#   cmake --build build/port-win64 --target mml_game64
# PORT_SDL3_MINGW is the unpacked SDL3-devel-<version>-mingw archive from libsdl.org. The game's
# C still goes through Clang (wasm32), wasm-ld and wasm2c on the build machine; only the host
# compiler changes.
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

set(PORT_SDL3_MINGW "" CACHE PATH "Unpacked SDL3-devel-<version>-mingw archive")
set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32 ${PORT_SDL3_MINGW}/x86_64-w64-mingw32)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER) # clang, wasm-ld, wasm2c and Python run on the host
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH) # WABT's wasm-rt headers live on the host
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Every C file sees windows.h and the POSIX fill-ins first (port_win32.h).
# One process, as on Apple TV: no fork/execv (LoadExec, the script server).
set(CMAKE_C_FLAGS_INIT "-include ${CMAKE_CURRENT_LIST_DIR}/port_win32.h -DPORT_SINGLE_PROCESS")
# One .exe with no MinGW runtime DLLs beside it (SDL3.dll still is).
set(CMAKE_EXE_LINKER_FLAGS_INIT "-static -static-libgcc")
# clock_gettime and nanosleep live in winpthread on MinGW-w64.
set(CMAKE_C_STANDARD_LIBRARIES "-lwinpthread" CACHE STRING "" FORCE)

# pkg-config finds SDL3's Windows sdl3.pc through the prefix path.
list(APPEND CMAKE_PREFIX_PATH ${PORT_SDL3_MINGW}/x86_64-w64-mingw32)
