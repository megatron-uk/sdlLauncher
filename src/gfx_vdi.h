#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Use the TOS VDI/GEM functions
#include <mint/osbind.h>
#include <gem.h>

// Include our logging functions
#include "logging.h"
#include "gfx.h"
#include "image.h"

// ================================================================
//
// Unused by anything external to these functions
//
// ================================================================

void gfxClear(FILE *log){
	short pxyarray[4];

	log_debug(log, "[%s:%d]\t: (gfxClear)\t: Clearing screen\n", __FILE__, __LINE__);
	
	/* this makes it bg colored */
	
	vqf_attributes(gem_vdi_handle, gem_attrib);
	vswr_mode(gem_vdi_handle, MD_REPLACE);
	vsf_interior(gem_vdi_handle, 0);
	
	pxyarray[0] = 0;
	pxyarray[1] = 0;
	pxyarray[2] = gem_screen.w;
	pxyarray[3] = gem_screen.h;
	vr_recfl(gem_vdi_handle, pxyarray);

	log_debug(log, "[%s:%d]\t: (gfxClear)\t: filled\n", __FILE__, __LINE__);
	
	/* restore interior fill style */
	vsf_interior(gem_vdi_handle, gem_attrib[0]);
	vswr_mode(gem_vdi_handle, gem_attrib[3]);
	
	log_debug(log, "[%s:%d]\t: (gfxClear)\t: Cleared\n", __FILE__, __LINE__);
}

// Restore original desktop colours
int gfxLoadPens(FILE *log){
	short c;
	if (gem_old_rgb_saved == 1){
		log_debug(log, "[%s:%d]\t: (gfxLoadPens)\t: Loading previous colour settings\n", __FILE__, __LINE__);
		for (c=0; c<gem_max_pens; c++){
			vs_color(gem_vdi_handle,c,gem_old_rgb[c]);
			//log_debug(log, "[gfxLoadPens]\t: Load pen %d [%d,%d,%d]\n", c, gem_old_rgb[c][0], gem_old_rgb[c][1], gem_old_rgb[c][2]);
		}
	}
	return 0;
}

// Save the values of the original pen colours
int gfxSavePens(FILE *log){
	short c;
	log_debug(log, "[%s:%d]\t: (gfxSavePens)\t: Saving current colour settings\n", __FILE__, __LINE__);
	for (c=0; c<gem_max_pens; c++){
		vq_color(gem_vdi_handle,c,1,gem_old_rgb[c]);
		//log_debug(log, "[gfxSavePens]\t: Save pen %d [%d,%d,%d]\n", c, gem_old_rgb[c][0], gem_old_rgb[c][1], gem_old_rgb[c][2]);
	}
	gem_old_rgb_saved = 1;
	return 0;
}

// Enable/disable clipping
void gfxClip(FILE *log, int enabled){
	short pxyarray[4];
	pxyarray[0] = 0;
	pxyarray[1] = 0;
	pxyarray[2] = gem_screen.w;
	pxyarray[3] = gem_screen.h;
	log_debug(log, "[%s:%d]\t: (gfxClip)\t: Clipping %d\n", __FILE__, __LINE__, enabled);
	vs_clip(gem_vdi_handle, enabled, pxyarray);
}

// =================================================================
//
// Standard functions exposed
//
// =================================================================

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
	int x,
	int y,
	int w,
	int h,
	struct agnostic_colours *border,
	struct agnostic_colours *fill,
	int border_px,
	int shadow_px
){
	short r = 0;			// Return code
	short pxyarray[4];	// Box geometry
	
	w = w - 1;
	h = h - 1;
	
	// Drop shadow
	if (shadow_px > 0){
		pxyarray[0] = x + shadow_px;
		pxyarray[1] = y + shadow_px;
		pxyarray[2] = x + w + shadow_px;
		pxyarray[3] = y + h + shadow_px;
		//log_debug(log, "[%s:%d]\t: (gfxDrawBox)\t: Shadow [%d,%d -> %d,%d]\n", __FILE__, __LINE__, pxyarray[0], pxyarray[1], pxyarray[2], pxyarray[3]);
		vsf_interior(gem_vdi_handle, FIS_SOLID);
		vsf_perimeter(gem_vdi_handle, 1);
		v_bar(gem_vdi_handle, pxyarray);
	}
	// Perimeter
	pxyarray[0] = x;
	pxyarray[1] = y;
	pxyarray[2] = x + w;
	pxyarray[3] = y + h;
	
	// Set border width, if an odd number of pixels
	if (border_px % 2 != 0){
		vsl_width(gem_vdi_handle, border_px);
	}
	//vsf_color(gem_vdi_handle, 0);
	//log_debug(log, "[%s:%d]\t: (gfxDrawBox)\t: Perimeter [%d,%d -> %d,%d]\n", __FILE__, __LINE__, pxyarray[0], pxyarray[1], pxyarray[2], pxyarray[3]);
	vsf_interior(gem_vdi_handle, FIS_HOLLOW);
	vsf_perimeter(gem_vdi_handle, 1);
	v_bar(gem_vdi_handle, pxyarray);
	
	
	// We should only do a fill if the border
	/*
	// Fill
	pxyarray[0] = x + border_px;
	pxyarray[1] = y + border_px;
	pxyarray[2] = x + (w - border_px);
	pxyarray[3] = y + (h - border_px);
	log_debug(log, "[%s:%d]\t: (gfxDrawBox)\t: Fill [%d,%d -> %d,%d]\n", __FILE__, __LINE__, pxyarray[0], pxyarray[1], pxyarray[2], pxyarray[3]);
	vsf_interior(gem_vdi_handle, FIS_SOLID);
	vsf_perimeter(gem_vdi_handle, 1);
	v_bar(gem_vdi_handle, pxyarray);
	*/
	
	// Set border back to default
	if (border_px > 1){
		vsl_width(gem_vdi_handle, 1);
	}
	
	//log_debug(log, "[%s:%d]\t: (gfxDrawBox)\t: Done\n", __FILE__, __LINE__);
	return r;
}

// Update the buffers to refresh gem_screen contents
void gfxFlip(FILE *log, struct agnostic_bitmap *screen){
}

// Free bitplane pixels from memory
void gfxFreeBMP(FILE *log,  struct agnostic_bitmap *bmp){
	if (&bmp->bmp->bp_pixels != NULL){
		log_debug(log, "[%s:%d]\t: (gfxFreeBMP)\t\t: Freeing bitplane pixel buffer @ %p\n", __FILE__, __LINE__, (void *) &bmp->bmp->bp_pixels);
		free(&bmp->bmp->bp_pixels);
		bmp->bmp->bp_pixels = NULL;
	} else {
		log_warn(log, "[%s:%d]\t: (gfxFreeBMP)\t\t: Pixel buffer pointer is NULL - not freeing\n", __FILE__, __LINE__);
	}
}

// Get last driver error
char* gfxGetError(){
	return "No Error";
}

// Load video driver and register application within GEM
int gfxInit(FILE *log){
	
	short rgb[3];
	short app_id;
	short i;
	short r = 0;

	log_debug(log, "[%s:%d]\t: (gfxInit)\t: Start\n", __FILE__, __LINE__);
	
	// ====== gfxInit ============
	app_id = appl_init();
	if (app_id == -1){
		log_error(log, "[%s:%d]\t: (gfxInit)\t: Unable to register application with GEM/AES!\n", __FILE__, __LINE__);
		return -1;
	}
	gem_screen.multitos = (aes_global[1] == -1);

	// open virtual screen workstation (screen)
	log_debug(log, "[%s:%d]\t: (gfxInit)\t: Opening Workstation\n", __FILE__, __LINE__);
	for(i = 0; i < 10; gem_work_in[i++] = 1);
	gem_work_in[10] = 2;
	v_opnvwk(gem_work_in, &gem_vdi_handle, gem_work_out);

	log_debug(log, "[%s:%d]\t: (gfxInit)\t: Workstation opened\n", __FILE__, __LINE__);
	
	// screen info
	gem_screen.w = gem_work_out[0];
	gem_screen.h = gem_work_out[1];
	
	log_debug(log, "[%s:%d]\t: (gfxInit)\t: Opening Window\n", __FILE__, __LINE__);
	// ======= gfxSetMode =========
	if (gem_screen.multitos) {
		short x, y, w, h;
		/* create window, otherwise cannot get all events */
		x = 0; y = 0; w = gem_screen.w; h = gem_screen.h;
		gem_win_handle = wind_create(0, x, y, w, h);
		if (gem_win_handle > 0) {
			wind_open(gem_win_handle, x, y, w, h);
		} else {
			log_error(log, "[%s:%d]\t: (gfxInit)\t: Unable to acquire window handle from GEM/AES!\n", __FILE__, __LINE__);
			return -1;	
		}
	} else {
		/* reserve screen area (in multitasking AES = a no-op),
		 * is really needed only for ACCs as on non-multitos AES
		 * only those can come on top of other applications.
		 */
		form_dial(FMD_START, gem_screen.w>>1, gem_screen.h>>1, 0, 0, 0, 0, gem_screen.w, gem_screen.h);
	}

	log_debug(log, "[%s:%d]\t: (gfxInit)\t: Window opened\n", __FILE__, __LINE__);
	
	// hide mouse, stop updates
	graf_mouse(M_OFF, NULL);
	wind_update(BEG_UPDATE);
	
	// enable clipping & clear screen
	log_debug(log, "[%s:%d]\t: (gfxInit)\t: About to clear...\n", __FILE__, __LINE__);
	gfxClip(log, 1);
	gfxClear(log);
	
	// Enable reverse video
	rgb[0] = 1000;
	rgb[1] = 1000;
	rgb[2] = 1000;
	vs_color(gem_vdi_handle,1,rgb);
	rgb[0] = 0;
	rgb[1] = 0;
	rgb[2] = 0;
	vs_color(gem_vdi_handle,0,rgb);
	
	//v_show_c(gem_vdi_handle, 1);
	
	log_debug(log, "[%s:%d]\t: (gfxInit)\t: End\n", __FILE__, __LINE__);
	
	return r;
}

// Load a bitmap file from disk into a bitmap structure
int gfxLoadBMP(FILE *log, char *filename, struct agnostic_bitmap *bmp){
	
	int autofree = 1;
	int r = 0;
	bmp->bmp->bp_pixels = NULL;
	
	// Load raw chunky bitmap
	log_debug(log, "[%s:%d]\t: (gfxLoadBMP)\t\t: Loading chunky bitmap\n", __FILE__, __LINE__, filename);
	r = imageLoadBMP(log, filename, bmp);
	if (r != 0){
		return -1;	
	}	
	// Convert to bitplanes and auto free the raw bitmap
	log_debug(log, "[%s:%d]\t: (gfxLoadBMP)\t\t: Converting to bitplanes\n", __FILE__, __LINE__);
	r = imageBMP2Bitplane(log, bmp, autofree);
	if (r != 0){
		return -1;	
	}
	return 0;
	
}

// Load a font bitmap from disk into a bitmap structure
int gfxLoadFont(FILE *log, char *filename, struct agnostic_bitmap *bmp){
	
	// Not used on Atari VDI - we just use the built in OS ROM fonts
	log_warn(log, "[%s:%d]\t: (gfxLoadFont)\t: Font loading not supported on Atari GEM/VDI backend\n", __FILE__, __LINE__);
	return 0;
	
}


// Unload driver
int gfxQuit(FILE *log){
	
	short rgb[3];
	short r = 0;
	
	log_debug(log, "[%s:%d]\t: (gfxQuit)\t: Unloading driver\n", __FILE__, __LINE__);
	
	// Undo Reverse video
	rgb[0] = 1000;
	rgb[1] = 1000;
	rgb[2] = 1000;
	vs_color(gem_vdi_handle,0,rgb);
	rgb[0] = 0;
	rgb[1] = 0;
	rgb[2] = 0;
	vs_color(gem_vdi_handle,1,rgb);
	
	// Restore original colours
	gfxLoadPens(log);
	
	// Unload all the VDI and GEM handlers
	gfxClip(log, 0);
	wind_update(END_UPDATE);
	graf_mouse(M_ON, NULL);
	form_dial(FMD_FINISH, gem_screen.w>>1, gem_screen.h>>1, 0, 0, 0, 0, gem_screen.w, gem_screen.h);
	v_clsvwk(gem_vdi_handle);
	
	//v_show_c(gem_vdi_handle, 0);
	appl_exit();
	Cursconf(2, 2);
	
	log_debug(log, "[%s:%d]\t: (gfxQuit)\t: Unloaded\n", __FILE__, __LINE__);
	return r;
}

// Set gem_screen mode
int gfxSetMode(FILE *log, struct agnostic_bitmap *screen, int screen_w, int screen_h, int screen_bpp){
	
	short char_w, char_h, cell_w, cell_h;
	short hin, vin, hout, vout;
	short r = 0;
	short point;
	
	log_info(log, "[%s:%d]\t: (gfxSetMode)\t: Setting screen mode [%dx%d]\n", __FILE__, __LINE__, screen_w, screen_h);
	
	// GEM screens count from pixel 0; hence a 320 pixel wide screen is reported as 319 pixels
	if ((gem_screen.w != (screen_w - 1)) || (gem_screen.h != (screen_h - 1))){
		log_error(log, "[%s:%d]\t: (gfxSetMode)\t: Requested screen mode [%dx%d]\n", __FILE__, __LINE__, screen_w, screen_h);
		log_error(log, "[%s:%d]\t: (gfxSetMode)\t: Current GEM screen mode [%dx%d]\n", __FILE__, __LINE__, gem_screen.w, gem_screen.h);
		log_error(log, "[%s:%d]\t: (gfxSetMode)\t: The requested screen mode != GEM screen mode\n", __FILE__, __LINE__);
		return -1;
	}
	
	if (screen_bpp != 0){
		log_warn(log, "[%s:%d]\t: (gfxSetMode)\t: Driver does not support setting pixel depth\n", __FILE__, __LINE__);
	}
	
	// Set text mode
	// Set font and font cell sizes
	char_w = 6;
	char_h = 8;
	cell_w = 6;
	cell_h = 9;
	
	// Font alignment hints
	hin = 0;
	vin = 5;
	
	// Set font alignment & size`
	vst_alignment(gem_vdi_handle, hin, vin, &hout, &vout);	
	point = vst_point(gem_vdi_handle, 8, &char_w, &char_h, &cell_w, &cell_h);
	log_debug(log, "[%s:%d]\t: (gfxSetMode)\t: Font set to %dpt\n", __FILE__, __LINE__, point);
	
	// Disable xbios cursor
	Cursconf(0, 0);
	
	return r;
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
	short r = 0;
	
	//log_debug(log, "[%s:%d]\t: (gfxText2BMP)\t: Printing %d characters\n", __FILE__, __LINE__, strlen(text));

	// Reverse video mode
	if (inverse){
		vswr_mode(gem_vdi_handle, 4);
	}	
	
	v_gtext(gem_vdi_handle, x, y + 1, text);
	
	// Toggle inverse mode back off again
	if (inverse){
		vswr_mode(gem_vdi_handle, 1);
	}	
	
	return r;
}