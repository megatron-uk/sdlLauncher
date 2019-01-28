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
#include "gfx_sdl.h"
#endif

// MS-DOS graphics library
#ifdef USE_ALLEGRO
#include "gfx_allegro.h"
#endif

// Atari graphics library
#ifdef USE_GODLIB
#include "gfx_godlib.h"
#endif

