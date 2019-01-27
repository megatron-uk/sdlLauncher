#include <stdio.h>
#include <stdbool.h>
#include <SDL/SDL.h>

#include "menu.h"
#include "font.h"
#include "graphics.h"
#include "logging.h"

// Load the defined font bitmap into a SDL surface.
SDL_Surface* loadfont(FILE *log, bool inverse){
	
	agnostic_bitmap bmp;
	
	// Load splash bitmap
	if (inverse == 1){
		loadBMP(FONT_BITMAP_REV, &bmp);
		log_info(log, "loadfont: Loading reverse font [%s]\n", FONT_BITMAP_REV);
	} else {
		loadBMP(FONT_BITMAP, &bmp);
		log_info(log, "loadfont: Loading normal font [%s]\n", FONT_BITMAP);
	}
	if (bmp.bmp == NULL){
		log_error(log, "loadfont: Error loading bitmap\n");
		return NULL;
	}
	return bmp.bmp;
}

// Turn a string of text into bitmaps and blit them onto
// the main display surface.
int text2surface(SDL_Surface *display, SDL_Surface *font_normal, SDL_Surface *font_reverse, FILE *log, char *text, int x, int y, bool inverse){

	SDL_Surface *font = NULL;
	SDL_Rect src, dest;
	unsigned int i;
	unsigned int font_index;
	unsigned int found;
	unsigned int r;
	unsigned int next_x;
	char c;
	
	if (inverse == 1){
		font = font_reverse;
	} else {
		font = font_normal;
	}

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
					src.x = FONT_W * CHAR_LIST[font_index].x;
					src.y = FONT_H * CHAR_LIST[font_index].y;
					src.w = FONT_W;
					src.h = FONT_H;
					//printf("%d : %c (x:%d,y:%d) at (%d,%d)px \n", i, c, CHAR_LIST[font_index].x, CHAR_LIST[font_index].y, src.x, src.y);
					dest.x = next_x;
					dest.y = y;
					dest.w = FONT_W;
					dest.h = FONT_H;
					//printf("%d : %c dest.x:%d dest.y:%d\n", i, c, dest.x, dest.y);				
					r = SDL_BlitSurface(font, &src, display, &dest);
					if ( r != 0){
						log_error(log, "text2surface: SDL Blit Error: %s\n", SDL_GetError());
						return r;
					}
					next_x += FONT_W;
				}
			}
			if (found == 0){
				log_warn(log, "text2surface: pos %d [%c] - No matching ASCII character found, blanking!\n", i, c);
				// Copy a blank or some placeholder here
				src.x = FONT_W * CHAR_LIST[FONT_PLACEHOLDER].x;
				src.y = FONT_H * CHAR_LIST[FONT_PLACEHOLDER].y;
				src.w = FONT_W;
				src.h = FONT_H;
				dest.x = next_x;
				dest.y = y;
				dest.w = FONT_W;
				dest.h = FONT_H;
				r = SDL_BlitSurface(font, &src, display, &dest);
				if ( r != 0){
					log_error(log, "text2surface: SDL Blit Error: %s\n", SDL_GetError());
					return r;
				}
				next_x += FONT_W;
			}
		}
	}
	return 0;
}