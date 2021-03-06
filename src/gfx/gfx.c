#include <stdio.h>
#include <stdbool.h>

// Platform agnostic headers
#include "gfx.h"

// =======================================
//
// This is an interface layer between the 
// application and any platform specific
// graphics libraries.
//
// =======================================


// Linux, Risc-OS and most other platforms
#ifdef USE_SDL
#include "gfx_sdl.c"
#endif

// MS-DOS graphics library
#ifdef USE_ALLEGRO
#include "gfx_allegro.c"
#endif

// Atari low level graphics library - not yet implemented, too many non-gcc m68k asm
#ifdef USE_GODLIB
#include "gfx_godlib.c"
#endif

// Atari GEM/VDI graphics library
#ifdef USE_GEM
#include "gfx_vdi.c"
#endif

