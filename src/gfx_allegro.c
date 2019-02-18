#ifdef USE_ALLEGRO
#define ALLEGRO_STATICLINK
#include <allegro.h>
#endif

#include "logging.h"
#include "menu.h"
#include "font.h"
#include "gfx.h"

// Copies a bitmap (specified by a source window overlay), to a destination bitmap (with a destination window overlay) 
int gfxBlitBMP(
	FILE *log, 
	struct agnostic_bitmap *bmp_src, 
	struct agnostic_window *window_src, 
	struct agnostic_bitmap *bmp_dst, 
	struct agnostic_window *window_dst
){
	int r = 0;	// Return code
	return r;
}

// Draw a box at the given coordinates in a given colour
int gfxDrawBox(
	FILE *log, 
	struct agnostic_bitmap *screen,
	int x,
	int y,
	int w,
	int h,
	struct agnostic_colours *border,
	struct agnostic_colours *fill,
	int border_px,
	int shadow_px
){
	int r = 0;	// Return code
	return r;
}

// Update the buffers to refresh screen contents
void gfxFlip(FILE *log, struct agnostic_bitmap *screen){
}

// Free a bitmap from memory
void gfxFreeBMP(FILE *log,  struct agnostic_bitmap *bmp){
}

// Get last driver error
char* gfxGetError(){
	return 0;
}

// Load video driver
int gfxInit(FILE *log){
	int r = 0;
	
	//r = install_allegro(SYSTEM_AUTODETECT, &errno, atexit);
	if (r != 0){
		log_error(log, "[%s:%d]\t: (gfxInit)\t Unable to intialise Allegro library: Error %d", __FILE__, __LINE__, r);
		return r;
	} else {
		return r;
	}
}

// Load a bitmap file from disk into a in-memory structure
int gfxLoadBMP(FILE *log, char *filename, struct agnostic_bitmap *bmp){
	return 0;
}

// Load a font bitmap from disk into a bitmap structure
int gfxLoadFont(FILE *log, char *filename, struct agnostic_bitmap *bmp){
	return gfxLoadBMP(log, filename, bmp);
}

// Unload driver
int gfxQuit(FILE *log){
	
	allegro_exit();
	return 0;
}

// Set screen mode
int gfxSetMode(FILE *log, struct agnostic_bitmap *screen, int screen_w, int screen_h, int screen_bpp){
	return 0;	
}

// Print text
int gfxText2BMP(
	struct agnostic_bitmap *display, 
	struct agnostic_bitmap *font_normal, 
	struct agnostic_bitmap *font_reverse, 
	FILE *log, 
	char *text, 
	int x, 
	int y, 
	bool inverse
){
	int r = 0;
	return r;
}