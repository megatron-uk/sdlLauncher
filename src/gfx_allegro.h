#include <allegro.h>

// Load a bitmap file from disk into a bitmap structure
int loadBMP(char *filename, struct agnostic_bitmap *bmp){

	bmp->bmp = load_bitmap(filename);
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
	blit(bmp_src->bmp, 
		bmp_dst->bmp, 
		window_src->window.x, 
		window_src->window.y, 
		window_dst->window.x, 
		window_dst->window.y, 
		window_src->window.w, 
		window_src->window.h);
	return r;
}

// Draw a box at the given coordinates in a given colour
int drawBox(
	struct agnostic_bitmap *screen, 
	struct agnostic_window *window, 
	struct agnostic_colours *rgb
){
	int r = 0;	// Return code
	return r;
}

// Update the buffers to refresh screen contents
void flip(struct agnostic_bitmap *screen){
	al_flip_display();
}