#include <SDL/SDL.h>

#include "logging.h"

// Copies a bitmap (specified by a source window overlay), to a destination bitmap (with a destination window overlay) 
int gfxBlitBMP(
	FILE *log, 
	struct agnostic_bitmap *bmp_src, 
	struct agnostic_window *window_src, 
	struct agnostic_bitmap *bmp_dst, 
	struct agnostic_window *window_dst
){
	int r = 0;	// Return code
	r = SDL_BlitSurface(bmp_src->bmp, &window_src->window, bmp_dst->bmp, &window_dst->window);
	if (r != 0 ){
		log_error(log, "[gfxBlitBMP]\t: Error: %s\n", SDL_GetError());
		return -1;	
	} else {
		return r;
	}
}

// Draw a box at the given coordinates in a given colour
int gfxDrawBox(
	FILE *log, 
	struct agnostic_bitmap *screen, 
	struct agnostic_window *window, 
	struct agnostic_colours *rgb
){
	int r = 0;	// Return code
	r = SDL_FillRect(screen->bmp, &window->window, SDL_MapRGB(screen->bmp->format, rgb->r, rgb->g, rgb->b));
	if (r != 0){
		log_error(log, "[gfxDrawBox]\t: Error: %s\n", SDL_GetError());
		return -1;
	} else {
		return r;
	}
}

// Update the buffers to refresh screen contents
void gfxFlip(FILE *log, struct agnostic_bitmap *screen){
	SDL_Flip(screen->bmp);
}

// Free a bitmap from memory
void gfxFreeBMP(FILE *log,  struct agnostic_bitmap *bmp){
	SDL_FreeSurface(bmp->bmp);
}

// Get last driver error
char* gfxGetError(){
	return SDL_GetError();
}

// Load video driver
int gfxInit(FILE *log){
	
	SDL_version sdl_compiled;
	int r = 0;
	
	r = SDL_Init(SDL_INIT_VIDEO);
	if (r != 0){
		log_error(log, "[gfxInit]\t: Error: %s\n", SDL_GetError());
		return -1;
	} else {
		SDL_VERSION(&sdl_compiled);
		log_info(log, "[gfxInit]\t: SDL v%d.%d.%d\n", sdl_compiled.major, sdl_compiled.minor, sdl_compiled.patch);
		return r;
	}
}

// Load a bitmap file from disk into a in-memory structure
int gfxLoadBMP(FILE *log, char *filename, struct agnostic_bitmap *bmp){

	bmp->bmp = SDL_LoadBMP(filename);
	if (!bmp->bmp){
		// Error in loading bitmap
		log_error(log, "[gfxLoadBMP]\t: Error: %s\n", SDL_GetError());
		return -1;	
	}
	return 0;
}

// Unload driver
int gfxQuit(FILE *log){
	SDL_Quit();
	return 0;
}

// Set screen mode
int gfxSetMode(FILE *log, struct agnostic_bitmap *screen, int screen_w, int screen_h, int screen_bpp){
	
	char vdriver[33];
	
	screen->bmp = SDL_SetVideoMode(screen_w, screen_h, screen_bpp, SDL_HWSURFACE);
	if (screen->bmp == NULL){
		// Error in screen init
		log_error(log, "[gfxSetMode]\t: Error: %s\n", SDL_GetError());
		return -1;
	} else {
		log_info(log, "[gfxSetMode]\t: Driver %s\n", SDL_VideoDriverName(vdriver, 32));
		log_info(log, "[gfxSetMode]\t: Mode %dx%dx%dbpp\n", screen->bmp->w, screen->bmp->h, screen->bmp->format->BitsPerPixel);
		log_info(log, "[gfxSetMode]\t: HW Surfaces? (%d)\n", SDL_GetVideoSurface()->flags & SDL_HWSURFACE);	
		return 0;
	}
}