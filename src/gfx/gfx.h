#pragma once

#ifdef USE_SDL
#include <SDL/SDL.h>
#endif

#ifdef USE_ALLEGRO
#include <allegro.h>
#endif

#ifdef USE_GEM
#include <mint/osbind.h>
#include <gem.h>
#include "qdbmp.h"
#endif

#ifdef USE_GEM
// Gem_screen geometry
struct gem_screen {
	int w, h;
	int multitos;
} gem_screen;

// Structure to fake a bitmap object for GEM
typedef struct gem_bitmap {
	BMP *pixels; 	// Store raw bitmaps here - a pointer to a qdbmp structure (see qdbmp.h)
					// - hopefully freed() after being converted to bp_pixels.
	
	MFDB *mfdb;		// VDI format raster image device header to describe the converted 
					// bitplane image data held at *bp_pixels.
					
	unsigned char *bp_pixels;// The converted Atari bitplane format image here.
	unsigned short bplanes;
	unsigned short bp_pixels_set;
	unsigned short pixels_set;
	unsigned short mfdb_set;
	
} gem_bitmap;

// Number of pens (aka colours)
#define gem_max_pens 16

// VDI function call handles
short gem_vdi_handle;
short gem_win_handle;
short gem_work_in[12];
short gem_work_out[57];

// Store original colour settings so that we can restore on exit
short gem_old_rgb[gem_max_pens][3];
static bool gem_old_rgb_saved = 0;

// Store original GEM attributes, so that we restore on exit
short gem_attrib[5];
#endif

// =======================================
//
// Interface layer between the application and SDL/Allegro/foobar
//
// =======================================

// Structure to hold window size and coordinates
typedef struct window_coords {
	unsigned int x;
	unsigned int y;
	unsigned int w;
	unsigned int h;
} window_coords;

// Structure to hold RGB colour values
typedef struct agnostic_colours {
	unsigned int r;
	unsigned int g;
	unsigned int b;
} agnostic_colours;

// Structure to hold a bitmap object
typedef struct agnostic_bitmap {
#ifdef USE_ALLEGRO
	BITMAP *bmp;		// Allegro backbuffer bitmap structure - only accessed by gfx_allegro.c
	PALETTE pal;	// Pallette for 8bit bitmaps
#endif
#ifdef USE_GEM
	gem_bitmap *bmp;	// GEM bitmap structure - only accessed by gfx_vdi.c
#endif
#ifdef USE_SDL
	SDL_Surface *bmp;	// SDL_Surface bitmap structure - only accessed by gfx_sdl.c
#endif
	// Standard bitmap fields for all backends
	unsigned int w;		// Width in pixels of source bitmap
	unsigned int h;		// Height in pixels of source bitmap
	unsigned int bpp;	// Colour depth of source bitmap
} agnostic_bitmap;

// Structure to hold a window/rectangle/view
typedef struct agnostic_window {
#ifdef USE_ALLEGRO
	struct window_coords window;
#endif
#ifdef USE_GEM
	struct window_coords window;
#endif
#ifdef USE_SDL
	SDL_Rect window;
#endif
} agnostic_window;

// Allegro has a single global palette with some reserved colours that
// all bitmap palettes have to use.
#ifdef USE_ALLEGRO
static PALETTE global_palette;
static char colour_reservation_table[256];
#define PAL_BLACK 0
#define PAL_WHITE 1
#endif

// ======================================
//
// Function prototypes
//
// ======================================

// Draws and fills a rectangular box
int gfxDrawBox(FILE *log, struct agnostic_bitmap *display, int x, int y, int w, int h, struct agnostic_colours *border, struct agnostic_colours *fill, int border_px, int shadow_px);

// Blit a (partial) bitmap from one area to another - either SDL or Allegro
int gfxBlitBMP(FILE *log, struct agnostic_bitmap *bmp_src, struct agnostic_window *window_src, struct agnostic_bitmap *bmp_dst, struct agnostic_window *window_dst); 

// Update the display buffer
void gfxFlip(FILE *log, struct agnostic_bitmap *display);

// Unload a bitmap from memory
void gfxFreeBMP(FILE *log, struct agnostic_bitmap *bmp);

// Last driver error
char* gfxGetError();

// Load driver
int gfxInit(FILE *log);

// Load a bitmap from file - either SDL, GEM or Allegro
int gfxLoadBMP(FILE *log, char *filename, struct agnostic_bitmap *bmp);
int gfxLoadFont(FILE *log, char *filename, struct agnostic_bitmap *bmp);

// Unload driver
int gfxQuit(FILE *log);

// Initialise the display
int gfxSetMode(FILE *log, struct agnostic_bitmap *display, int screen_w, int screen_h, int screen_bpp);

// Print text
int gfxText2BMP(struct agnostic_bitmap *display, struct agnostic_bitmap *font_normal, struct agnostic_bitmap *font_reverse,	FILE *log, char *text, int x, int y, bool inverse);

#ifdef USE_GEM
void gfxClear(FILE *log);
int gfxLoadPens(FILE *log);
int gfxSavePens(FILE *log);
void gfxClip(FILE *log, int enabled);
#endif