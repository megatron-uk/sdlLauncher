// Include the SDL library functions
#include <SDL/SDL.h>

// Include our logging functions
#include "logging.h"
#include "menu.h"
#include "font.h"

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
		log_error(log, "[%s:%d]\t: (gfxBlitBMP)\t: Error: %s\n", __FILE__, __LINE__, SDL_GetError());
		return -1;	
	} else {
		return r;
	}
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
	agnostic_window box;	// A bounding box for the borders we want to draw
	int r = 0;			// Return code
	
	// Drop shadow
	if (shadow_px > 0){
		box.window.x = x + shadow_px;
		box.window.y = y + shadow_px;
		box.window.w = w;
		box.window.h = h;
		r = SDL_FillRect(screen->bmp, &box.window, SDL_MapRGB(screen->bmp->format, border->r, border->g, border->b));
		if (r != 0){
			log_error(log, "[%s:%d]\t: (gfxDrawBox)\t: Shadow Error: %s\n", __FILE__, __LINE__, SDL_GetError());
			return -1;
		}
	}
	// Perimeter
	box.window.x = x;
	box.window.y = y;
	box.window.w = w;
	box.window.h = h;
	r = SDL_FillRect(screen->bmp, &box.window, SDL_MapRGB(screen->bmp->format, border->r, border->g, border->b));
	if (r != 0){
		log_error(log, "[%s:%d]\t: (gfxDrawBox)\t: Perimeter Error: %s\n", __FILE__, __LINE__, SDL_GetError());
		return -1;
	}
	
	// Fill
	box.window.x = x + border_px;
	box.window.y = y + border_px;
	box.window.w = w - (border_px + 1);
	box.window.h = h - (border_px + 1);
	r = SDL_FillRect(screen->bmp, &box.window, SDL_MapRGB(screen->bmp->format, fill->r, fill->g, fill->b));
	if (r != 0){
		log_error(log, "[%s:%d]\t: (gfxDrawBox)\t: Fill Error: %s\n", __FILE__, __LINE__, SDL_GetError());
		return -1;
	}
	
	return 0;
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
		log_error(log, "[%s:%d]\t: (gfxInit)\t: Error: %s\n", __FILE__, __LINE__, SDL_GetError());
		return -1;
	} else {
		SDL_VERSION(&sdl_compiled);
		log_info(log, "[%s:%d]\t: (gfxInit)\t: SDL v%d.%d.%d\n", __FILE__, __LINE__, sdl_compiled.major, sdl_compiled.minor, sdl_compiled.patch);
		return r;
	}
}

// Load a bitmap file from disk into a in-memory structure
int gfxLoadBMP(FILE *log, char *filename, struct agnostic_bitmap *bmp){
	
	bmp->bmp = SDL_LoadBMP(filename);
	if (!bmp->bmp){
		// Error in loading bitmap
		log_error(log, "[%s:%d]\t: (gfxLoadBMP)\t: Error: %s\n", __FILE__, __LINE__, SDL_GetError());
		return -1;	
	}
	return 0;
}

// Load a font bitmap from disk into a bitmap structure
int gfxLoadFont(FILE *log, char *filename, struct agnostic_bitmap *bmp){
	return gfxLoadBMP(log, filename, bmp);
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
		log_error(log, "[%s:%d]\t: (gfxSetMode)\t: Error setting screen mode: %s\n", __FILE__, __LINE__, SDL_GetError());
		return -1;
	} else {
		log_info(log, "[%s:%d]\t: (gfxSetMode)\t: Driver %s\n", __FILE__, __LINE__, SDL_VideoDriverName(vdriver, 32));
		log_info(log, "[%s:%d]\t: (gfxSetMode)\t: Mode %dx%dx%dbpp\n", __FILE__, __LINE__, screen->bmp->w, screen->bmp->h, screen->bmp->format->BitsPerPixel);
		log_info(log, "[%s:%d]\t: (gfxSetMode)\t: HW Surfaces? (%d)\n", __FILE__, __LINE__, SDL_GetVideoSurface()->flags & SDL_HWSURFACE);	
		return 0;
	}
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
	agnostic_window src, dest;
	unsigned int i;
	unsigned int font_index;
	unsigned int found;
	unsigned int r = 0;
	unsigned int next_x;
	char c;

	// Loop through every character of the text string
	next_x = x;
	for (i = 0; i < strlen(text); i++){
		c = text[i];
		found = 0;
		if (c != '\0'){
			// Find a matching character in the font table
			for (font_index = 0; font_index < CHAR_LIST_SIZE; font_index++){
				if (CHAR_LIST[font_index].sym == c){
					found = 1;
					
					// Add this bitmap fragment to the outgoing image surface
					src.window.x = FONT_W * CHAR_LIST[font_index].x;
					src.window.y = FONT_H * CHAR_LIST[font_index].y;
					src.window.w = FONT_W;
					src.window.h = FONT_H;
					dest.window.x = next_x;
					dest.window.y = y;
					dest.window.w = FONT_W;
					dest.window.h = FONT_H;
					if (inverse){
						r = gfxBlitBMP(log, (agnostic_bitmap *)&font_reverse, &src, display, &dest);
					} else {
						r = gfxBlitBMP(log, (agnostic_bitmap *)&font_normal, &src, display, &dest);
					}
					if ( r != 0){
						log_error(log, "[text2BMP]\t: Blit Error\n");
						return r;
					}
					next_x += FONT_W;
				}
			}
			if (found == 0){
				log_warn(log, "[text2BMP]\t: Pos %d [%c] - No matching ASCII character found, blanking!\n", i, c);
				// Copy a blank or some placeholder here
				src.window.x = FONT_W * CHAR_LIST[FONT_PLACEHOLDER].x;
				src.window.y = FONT_H * CHAR_LIST[FONT_PLACEHOLDER].y;
				src.window.w = FONT_W;
				src.window.h = FONT_H;
				dest.window.x = next_x;
				dest.window.y = y;
				dest.window.w = FONT_W;
				dest.window.h = FONT_H;
				if (inverse){
					r = gfxBlitBMP(log, (agnostic_bitmap *)&font_reverse, &src, display, &dest);
				} else {
					r = gfxBlitBMP(log, (agnostic_bitmap *)&font_normal, &src, display, &dest);
				}
				if ( r != 0){
					log_error(log, "[text2BMP]\t: Blit Error\n");
					return r;
				}
				next_x += FONT_W;
			}
		}
	}
	return r;
}
