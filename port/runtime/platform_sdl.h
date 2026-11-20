// SDL3 front end: a window showing VRAM (or the display area), and host input mapped to two
// PS1 pads. The only file of the runtime that includes SDL. Bindings, window size, scaling and
// the pad mode come from the settings file (port_config.h, [keyboard] [gamepad] [video] [pad]).
//
// Default mapping (pad 1; pad 2 takes a second gamepad):
//   gamepad  south/east/west/north = Cross/Circle/Square/Triangle, shoulders = L1/R1,
//            triggers = L2/R2, stick clicks = L3/R3, back = Select, start = Start,
//            d-pad = d-pad, sticks = analog sticks
//   keyboard arrows = d-pad, X/C/Z/S = Cross/Circle/Square/Triangle, Q/W = L2/L1,
//            E/R = R1/R2, Enter = Start, Right Shift = Select, I/J/K/L = left stick
//   window   F11 = fullscreen, Tab = toggle full VRAM / display area, F3 = toggle analog
//            mode, Esc / guide = the port menu (resume, controls, settings, quit; the game is
//            frozen while it is open); M / right-stick click = the mod panel (a host button,
//            PORT_HOST_MODS, never sent to the game)
#ifndef PORT_PLATFORM_SDL_H
#define PORT_PLATFORM_SDL_H

#include "pad.h"

#include <stdint.h>

#define PLATFORM_PADS 2

int platform_init(const char *title, int scale);
void platform_shutdown(void);

// Handles pending events and refreshes pads[0..PLATFORM_PADS-1]. Returns 0 once the user
// closes the window.
int platform_poll(PSX_PAD_STATE pads[PLATFORM_PADS]);

// Shows the GPU's VRAM: the display area, or all of it when full_vram is set (Tab toggles),
// with the port's text layer (port_overlay()) over it when that is visible.
void platform_present(void);

// The port menu (Esc): while platform_poll has it open, shows it over the last picture and
// handles it until it closes. Returns 0 if the player chose to quit (or closed the window).
// Only for a game that runs in the window's own process; the window host freezes its game.
int platform_menu_run(void);
// Shows all of VRAM (1) or the display area (0); Tab toggles it.
void platform_set_full_vram(int on);

// Opens the default playback device, 16-bit stereo at `rate`; SDL's audio thread calls `pull`
// for frames as it needs them. Returns 0, or -1 with a message.
int platform_audio_open(int rate, void (*pull)(int16_t *out, int frames));
// Holds the audio thread off (and drops what it has queued) while the source changes.
void platform_audio_lock(int lock);

// The window host (port_remote.h): shows the frames of the game process `child`, plays its
// sound and feeds it the pads until it exits; returns its exit status for ours.
int platform_host_run(int child);

#endif
