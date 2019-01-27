#include <stdio.h>
#include <stdbool.h>

#ifdef USE_SDL
#include <SDL/SDL.h>
#endif

#ifdef USE_ALLEGRO
#include <allegro.h>
#endif

#include "graphics.h"

// =======================================
//
// Interface layer between the application and SDL/Allegro/foobar
//
// =======================================

// Load a bitmap file from disk into a bitmap structure
int loadBMP(char *filename, struct agnostic_bitmap *bmp){

#ifdef USE_ALLEGRO
	bmp->bmp = load_bitmap(filename);
#endif
#ifdef USE_SDL
	bmp->bmp = SDL_LoadBMP(filename);
#endif
	if (!bmp->bmp){
		// Error in loading bitmap
		return -1;	
	}
	return 0;
}

// Copies a bitmap (specified by a source window overlay), to a destination bitmap (with a destination window overlay) 
int blitSurface(
	struct agnostic_bitmap *bmp_src, 
	struct agnostic_window *window_src, 
	struct agnostic_bitmap *bmp_dst, 
	struct agnostic_window *window_dst
){
	int r = 0;	// Return code
	
#ifdef USE_ALLEGRO
	blit(bmp_src->bmp, 
		bmp_dst->bmp, 
		window_src->window.x, 
		window_src->window.y, 
		window_dst->window.x, 
		window_dst->window.y, 
		window_src->window.w, 
		window_src->window.h);
#endif
#ifdef USE_SDL
	r = SDL_BlitSurface(bmp_src->bmp, &window_src->window, bmp_dst->bmp, &window_dst->window);
#endif
	return r;
}

// Draw a box at the given coordinates in a given colour
int drawBox(
	struct agnostic_bitmap *screen, 
	struct agnostic_window *window, 
	struct agnostic_colours *rgb
){
	int r = 0;	// Return code
#ifdef USE_SDL
	r = SDL_FillRect(screen->bmp, &window->window, SDL_MapRGB(screen->bmp->format, rgb->r, rgb->g, rgb->b));
#endif
	return r;
}

// Update the buffers to refresh screen contents
void flip(struct agnostic_bitmap *screen){

#ifdef USE_ALLEGRO
	al_flip_display();
#endif
#ifdef USE_SDL
	SDL_Flip(screen->bmp);
#endif

}