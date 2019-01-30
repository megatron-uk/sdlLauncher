#ifdef USE_SDL
#include <SDL/SDL.h>
#endif

#ifdef USE_ALLEGRO
#include <allegro.h>
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
	BITMAP *bmp;
#endif
#ifdef USE_SDL
	SDL_Surface *bmp;
#endif
} agnostic_bitmap;

// Structure to hold a window/rectangle/view
typedef struct agnostic_window {
#ifdef USE_ALLEGRO
	struct window_coords window;
#endif
#ifdef USE_SDL
	SDL_Rect window;
#endif
} agnostic_window;

// ======================================
//
// Function prototypes
//
// ======================================

// Draws and fills a rectangular box
int gfxDrawBox(FILE *log, struct agnostic_bitmap *screen, int x, int y, int w, int h, struct agnostic_colours *border, struct agnostic_colours *fill, int border_px, int shadow_px);

// Blit a (partial) bitmap from one area to another - either SDL or Allegro
int gfxBlitBMP(FILE *log, struct agnostic_bitmap *bmp_src, struct agnostic_window *window_src, struct agnostic_bitmap *bmp_dst, struct agnostic_window *window_dst); 

// Update the display buffer
void gfxFlip(FILE *log, struct agnostic_bitmap *screen);

// Unload a bitmap from memory
void gfxFreeBMP(FILE *log, struct agnostic_bitmap *bmp);

// Last driver error
char* gfxGetError();

// Load driver
int gfxInit(FILE *log);

// Load a bitmap from file - either SDL or Allegro
int gfxLoadBMP(FILE *log, char *filename, struct agnostic_bitmap *bmp);

// Unload driver
int gfxQuit(FILE *log);

// Initialise the display
int gfxSetMode(FILE *log, struct agnostic_bitmap *screen, int screen_w, int screen_h, int screen_bpp);