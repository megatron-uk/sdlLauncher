// Use the TOS VDI/GEM functions
#include <mint/osbind.h>
#include <gem.h>

// Include our logging functions
#include "logging.h"

// Defines that are only valid for GEM/VDI

// Number of pens (aka colours)
#define MAX_PENS 16

// Screen geometry
static struct {
	int w, h;
	int multitos;
} screen;

// VDI function call handles
static short vdi_handle;
static short win_handle;

// Store original colour settings so that we can restore on exit
static short old_rgb[MAX_PENS][3];

// Copies a bitmap (specified by a source window overlay), to a destination bitmap (with a destination window overlay) 
int gfxBlitBMP(
	FILE *log,
	struct agnostic_bitmap *bmp_src, 
	struct agnostic_window *window_src, 
	struct agnostic_bitmap *bmp_dst, 
	struct agnostic_window *window_dst
){
	int r = 0;	// Return code
	return r;
}

// Draw a box at the given coordinates in a given colour
int gfxDrawBox(
	FILE *log,
	struct agnostic_bitmap *screen, 
	struct agnostic_window *window, 
	struct agnostic_colours *rgb
){
	int r = 0;	// Return code
	
	short pxyarray[4];
	
	pxyarray[0] = window->x;
	pxyarray[1] = window->y;
	pxyarray[2] = window->x;
	pxyarray[3] = window->y + window->h;
	v_pline(screen->vdi_handle, 2, pxyarray);
	
	pxyarray[0] = window->x;
	pxyarray[1] = window->y;
	pxyarray[2] = window->x + window->w;
	pxyarray[3] = window->y;
	v_pline(screen->vdi_handle, 2, pxyarray);
	
	pxyarray[0] = window->x + window->w;
	pxyarray[1] = window->y;
	pxyarray[2] = window->x + window->w;
	pxyarray[3] = window->y + window->h;
	v_pline(screen->vdi_handle, 2, pxyarray);
	
	pxyarray[0] = window->x;
	pxyarray[1] = window->y + window->h;
	pxyarray[2] = window->x + window->w;
	pxyarray[3] = window->y + window->h;
	v_pline(screen->vdi_handle, 2, pxyarray);
	
	//if (fill){
		// Fill inner area
		pxyarray[0] = x1 + 1;
		pxyarray[1] = y1 + 1;
		pxyarray[2] = (x2 - x1) - 1;
		pxyarray[3] = (y2 - y1) - 1;
		vr_recfl(screen->vdi_handle, pxyarray);
	//}
	
	return r;
}

// Update the buffers to refresh screen contents
void gfxFlip(FILE *log, struct agnostic_bitmap *screen){
}

// Free a bitmap from memory
void gfxFreeBMP(FILE *log,  struct agnostic_bitmap *bmp){
}

// Get last driver error
char* gfxGetError(){
}

// Load video driver
int gfxInit(FILE *log){
	
	short app_id;
	app_id = appl_init();
	screen.multitos = (aes_global[1] == -1);
	
	/* open virtual screen workstation (screen) */
	for(i = 0; i < 10; work_in[i++] = 1);
	work_in[10] = 2;
	v_opnvwk(work_in, &vdi_handle, work_out);

	/* screen info */
	screen.w = work_out[0];
	screen.h = work_out[1];
	
	return 0;
}

// Load a bitmap file from disk into a bitmap structure
int gfxLoadBMP(FILE *log, char *filename, struct agnostic_bitmap *bmp){
	return 0;
}

// Restore original desktop colours
int gfxLoadPens(FILE *log, short rgb[MAX_PENS][3]){
	short c;
	log_debug(log, "[gfxLoadPens]\t: Loading previous colour settings\n");
	for (c=0; c<MAX_PENS; c++){
		vs_color(vdi_handle,c,old_rgb[c]);
		log_debug(log, "[gfxLoadPens]\t: Load pen %d [%d,%d,%d]\n", c, rgb[c][0], rgb[c][1], rgb[c][2]);
	}
	return 0;
}

// Unload driver
int gfxQuit(FILE *log){
	short pxyarray[4];
	pxyarray[0] = 0;
	pxyarray[1] = 0;
	pxyarray[2] = screen.w;
	pxyarray[3] = screen.h;
	
	// Restore original colours
	gfxLoadPens(log, old_rgb);
	
	// Unload all the VDI and GEM handlers
	vs_clip(vdi_handle, 0, pxyarray);
	wind_update(END_UPDATE);
	graf_mouse(M_ON, NULL);
	form_dial(FMD_FINISH, screen.w>>1, screen.h>>1, 0, 0, 0, 0, screen.w, screen.h);
	v_clsvwk(vdi_handle);
	appl_exit();
	return 0;
}

// Save the values of the original pen colours
int gfxSavePens(FILE *log, short rgb[MAX_PENS][3]){
	short c;
	log_debug(log, "[gfxSavePens]\t: Saving current colour settings\n");
	for (c=0; c<MAX_PENS; c++){
		vq_color(vdi_handle,c,1,rgb[c]);
		log_debug(log, "[gfxSavePens]\t: Save pen %d [%d,%d,%d]\n", c, rgb[c][0], rgb[c][1], rgb[c][2]);
	}
	return 0;
}

// Set screen mode
int gfxSetMode(FILE *log, struct agnostic_bitmap *screen, int screen_w, int screen_h, int screen_bpp){
	
	// Save colours
	gfxSavePens(log, old_rgb);
	
	// Blank screen
	return 0;
}
