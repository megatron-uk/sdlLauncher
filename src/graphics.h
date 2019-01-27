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

// Load a bitmap from file - either SDL or Allegro
int loadBMP(char *filename, struct agnostic_bitmap *bmp);

// Draws and fills a rectangular box
int drawBox(struct agnostic_bitmap *screen, struct agnostic_window *window, struct agnostic_colours *rgb);

// Blit a (partial) bitmap from one area to another - either SDL or Allegro
int blitSurface(struct agnostic_bitmap *bmp_src, struct agnostic_window *window_src, struct agnostic_bitmap *bmp_dst, struct agnostic_window *window_dst); 