// Include the Reservoir gods Atari ST godlib.a library
#include <GODLIB/GODLIB.H>

// Include our logging functions
#include "../misc/logging.h"
#include "../menu.h"
#include "../misc/font.h"
#include "gfx.h"

// Copies a bitmap (specified by a source window overlay), to a destination bitmap (with a destination window overlay) 
int blitSurface(
	struct agnostic_bitmap *bmp_src, 
	struct agnostic_window *window_src, 
	struct agnostic_bitmap *bmp_dst, 
	struct agnostic_window *window_dst
){
	int r = 0;	// Return code
	return r;
}

// Draw a box at the given coordinates in a given colour
int drawBox(
	struct agnostic_bitmap *display, 
	struct agnostic_window *window, 
	struct agnostic_colours *rgb
){
	int r = 0;	// Return code
	return r;
}

// Update the buffers to refresh screen contents
void flip(struct agnostic_bitmap *display){

}

// Load a bitmap file from disk into a bitmap structure
int loadBMP(char *filename, struct agnostic_bitmap *bmp){

	return 0;
}


// Set screen mode
int setScreen(){
	
	return 0;
}
