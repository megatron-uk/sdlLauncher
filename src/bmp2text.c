#include <stdio.h>
#include <stdbool.h>

#include "menu.h"
#include "font.h"
#include "gfx.h"
#include "logging.h"

// Load the defined font bitmap into a SDL surface.
int loadfont(FILE *log, struct agnostic_bitmap *bmp, bool inverse){
	
	int r = 0;
	
	// Load font bitmap
	if (inverse == 1){
		log_info(log, "[loadfont]\t: Loading reverse font [%s]\n", FONT_BITMAP_REV);
		r = gfxLoadFont(log, FONT_BITMAP_REV, (agnostic_bitmap *)bmp);
	} else {
		log_info(log, "[loadfont]\t: Loading normal font [%s]\n", FONT_BITMAP);
		r = gfxLoadFont(log, FONT_BITMAP, (agnostic_bitmap *)bmp);
	}
	if (r == -1){
		log_error(log, "[loadfont]\t: Error loading bitmap\n");
		return -1;
	} else {
		return 0;
	}
}

// Turn a string of text into bitmaps and blit them onto
// the main display surface.
int text2BMP(struct agnostic_bitmap *display, struct agnostic_bitmap *font_normal, struct agnostic_bitmap *font_reverse, FILE *log, char *text, int x, int y, bool inverse){

	return gfxText2BMP(display, font_normal, font_reverse, log, text, x, y, inverse);
}