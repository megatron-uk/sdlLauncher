#include <stdio.h>
#include <stdbool.h>

#include "menu.h"
#include "font.h"
#include "gfx.h"
#include "logging.h"

// Load the defined font bitmap into a SDL surface.
int loadfont(FILE *log, struct agnostic_bitmap *bmp, bool inverse){
	
	
	// Load font bitmap
	if (inverse == 1){
		log_info(log, "[loadfont]\t: Loading reverse font [%s]\n", FONT_BITMAP_REV);
		gfxLoadBMP(log, FONT_BITMAP_REV, (agnostic_bitmap *)bmp);
	} else {
		log_info(log, "[loadfont]\t: Loading normal font [%s]\n", FONT_BITMAP);
		gfxLoadBMP(log, FONT_BITMAP, (agnostic_bitmap *)bmp);
	}
	if (bmp->bmp == NULL){
		log_error(log, "[loadfont]\t: Error loading bitmap\n");
		return -1;
	} else {
		return 0;
	}
}

// Turn a string of text into bitmaps and blit them onto
// the main display surface.
int text2BMP(struct agnostic_bitmap *display, struct agnostic_bitmap *font_normal, struct agnostic_bitmap *font_reverse, FILE *log, char *text, int x, int y, bool inverse){

	agnostic_window src, dest;
	unsigned int i;
	unsigned int font_index;
	unsigned int found;
	unsigned int r;
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
	return 0;
}