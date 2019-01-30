#include <stdio.h>
#include <stdbool.h>

// Platform agnostic headers
#include "input.h"

// =======================================
//
// This is an interface layer between the 
// application and any platform specific
// input libraries.
//
// =======================================


// Linux, Risc-OS and most other platforms
#ifdef USE_SDL
#include "input_sdl.h"
#endif

// MS-DOS library
#ifdef USE_ALLEGRO
#include "input_allegro.h"
#endif

// Atari GEM/VDI input library
#ifdef USE_GEM
#include "input_gem.h"
#endif