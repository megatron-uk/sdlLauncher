#ifdef USE_ALLEGRO
#include <allegro.h>
#endif

#include "logging.h"
#include "menu.h"
#include "font.h"
#include "gfx.h"


void gfxInfo(FILE *log){
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: ------------ \n", __FILE__, __LINE__);
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: Allegro hardware driver capabilities\n", __FILE__, __LINE__);
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: %d GFX_CAN_SCROLL\n", __FILE__, __LINE__, (gfx_capabilities & GFX_CAN_SCROLL));	
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: %d GFX_CAN_TRIPLE_BUFFER\n", __FILE__, __LINE__, (gfx_capabilities & GFX_CAN_TRIPLE_BUFFER));	
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: %d GFX_HW_CURSOR\n", __FILE__, __LINE__, (gfx_capabilities & GFX_HW_CURSOR));
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: %d GFX_SYSTEM_CURSOR\n", __FILE__, __LINE__, (gfx_capabilities & GFX_SYSTEM_CURSOR));
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: %d GFX_HW_HLINE\n", __FILE__, __LINE__, (gfx_capabilities & GFX_HW_HLINE));	
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: %d GFX_HW_FILL\n", __FILE__, __LINE__, (gfx_capabilities & GFX_HW_FILL));	
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: %d GFX_HW_HLINE_XOR\n", __FILE__, __LINE__, (gfx_capabilities & GFX_HW_HLINE_XOR));
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: %d GFX_HW_HLINE_SOLID_PATTERN\n", __FILE__, __LINE__, (gfx_capabilities & GFX_HW_HLINE_SOLID_PATTERN));
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: %d GFX_HW_HLINE_COPY_PATTERN\n", __FILE__, __LINE__, (gfx_capabilities & GFX_HW_HLINE_COPY_PATTERN));
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: %d GFX_HW_FILL_XOR\n", __FILE__, __LINE__, (gfx_capabilities & GFX_HW_FILL_XOR));
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: %d GFX_HW_FILL_SOLID_PATTERN\n", __FILE__, __LINE__, (gfx_capabilities & GFX_HW_FILL_SOLID_PATTERN));
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: %d GFX_HW_FILL_COPY_PATTERN\n", __FILE__, __LINE__, (gfx_capabilities & GFX_HW_FILL_COPY_PATTERN));
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: %d GFX_HW_LINE\n", __FILE__, __LINE__, (gfx_capabilities & GFX_HW_LINE));
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: %d GFX_HW_LINE_XOR\n", __FILE__, __LINE__, (gfx_capabilities & GFX_HW_LINE_XOR));
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: %d GFX_HW_TRIANGLE\n", __FILE__, __LINE__, (gfx_capabilities & GFX_HW_TRIANGLE));
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: %d GFX_HW_TRIANGLE_XOR\n", __FILE__, __LINE__, (gfx_capabilities & GFX_HW_TRIANGLE_XOR));
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: %d GFX_HW_GLYPH\n", __FILE__, __LINE__, (gfx_capabilities & GFX_HW_GLYPH));
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: %d GFX_HW_VRAM_BLIT\n", __FILE__, __LINE__, (gfx_capabilities & GFX_HW_VRAM_BLIT));
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: %d GFX_HW_VRAM_BLIT_MASKED\n", __FILE__, __LINE__, (gfx_capabilities & GFX_HW_VRAM_BLIT_MASKED));
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: %d GFX_HW_MEM_BLIT\n", __FILE__, __LINE__, (gfx_capabilities & GFX_HW_MEM_BLIT));
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: %d GFX_HW_MEM_BLIT_MASKED\n", __FILE__, __LINE__, (gfx_capabilities & GFX_HW_MEM_BLIT_MASKED));
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: %d GFX_HW_SYS_TO_VRAM_BLIT\n", __FILE__, __LINE__, (gfx_capabilities & GFX_HW_SYS_TO_VRAM_BLIT));
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: %d GFX_HW_SYS_TO_VRAM_BLIT_MASKED\n", __FILE__, __LINE__, (gfx_capabilities & GFX_HW_SYS_TO_VRAM_BLIT_MASKED));
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: %d GFX_HW_VRAM_STRETCH_BLIT\n", __FILE__, __LINE__, (gfx_capabilities & GFX_HW_VRAM_STRETCH_BLIT));
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: %d GFX_HW_SYS_STRETCH_BLIT\n", __FILE__, __LINE__, (gfx_capabilities & GFX_HW_SYS_STRETCH_BLIT));
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: %d GFX_HW_VRAM_STRETCH_BLIT_MASKED\n", __FILE__, __LINE__, (gfx_capabilities & GFX_HW_VRAM_STRETCH_BLIT_MASKED));
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: %d GFX_HW_SYS_STRETCH_BLIT_MASKED\n", __FILE__, __LINE__, (gfx_capabilities & GFX_HW_SYS_STRETCH_BLIT_MASKED));
	log_debug(log, "[%s:%d]\t: (gfxInfo)\t: ------------ \n", __FILE__, __LINE__);
}

// ==================== Public methods below ======================

// Copies a bitmap (specified by a source window overlay), to a destination bitmap (with a destination window overlay) 
int gfxBlitBMP(
	FILE *log, 
	struct agnostic_bitmap *bmp_src, 
	struct agnostic_window *window_src, 
	struct agnostic_bitmap *bmp_dst, 
	struct agnostic_window *window_dst
){
	int r = 0;	// Return code
	
	//log_debug(log, "blit (%d,%d) -> (%d,%d) %dx%d\n", window_src->window.x, window_src->window.y, window_dst->window.x, window_dst->window.y, window_src->window.w, window_src->window.h);
	
	blit(bmp_src->bmp, 
		bmp_dst->bmp, 
		window_src->window.x, 
		window_src->window.y, 
		window_dst->window.x, 
		window_dst->window.y, 
		window_src->window.w, 
		window_src->window.h
	);
	
	return r;
}

// Draw a box at the given coordinates in a given colour
int gfxDrawBox(
	FILE *log, 
	struct agnostic_bitmap *display,
	int x,
	int y,
	int w,
	int h,
	struct agnostic_colours *border,
	struct agnostic_colours *fill,
	int border_px,
	int shadow_px
){
	int r = 0;	// Return code
	int fill_colour = 0;
	int border_colour = 0;
	
	// Map RGB to allegro integer colours
	border_colour = makecol(border->r, border->g, border->b);
	fill_colour = makecol(fill->r, fill->g, fill->b);
	
	// Are we drawing a drop shadow?
	if (shadow_px > 0){
		rectfill(display->bmp, x+shadow_px, y+shadow_px, x+w + shadow_px, y+h + shadow_px, border_colour);
	}
	
	// Border
	rectfill(display->bmp, x, y, x+w, y+h, border_colour);

	// Fill
	rectfill(display->bmp, x+border_px, y+border_px, x + (w - (border_px)), y + (h - (border_px)), fill_colour);
	
	return r;
}

// Update the buffers to refresh display contents
void gfxFlip(FILE *log, struct agnostic_bitmap *display){
	// Flip buffer to screen
	blit(display->bmp, screen, 0, 0, 0, 0, MENU_SCREEN_W, MENU_SCREEN_H);
	// Clear buffer
	// clear_bitmap(display->bmp);
}

// Free a bitmap from memory
void gfxFreeBMP(FILE *log,  struct agnostic_bitmap *bmp){
	destroy_bitmap(bmp->bmp);
}

// Get last driver error
char* gfxGetError(){
	return allegro_error;
}

// Load video driver
int gfxInit(FILE *log){
	int r = 0;
	
	r = allegro_init();
	if (r != 0){
		log_error(log, "[%s:%d]\t: (gfxInit)\t: Unable to intialise Allegro library: Error %d\n", __FILE__, __LINE__, r);
		return r;
	} else {
		log_info(log, "[%s:%d]\t: (gfxInit)\t: Allegro library loaded\n", __FILE__, __LINE__);
		// If allegro loaded, also initialise timer and keyboard input support
		install_keyboard(); 
		install_timer();
		gfxInfo(log);		
		return r;
	}
}

// Load a bitmap file from disk into a in-memory structure
int gfxLoadBMP(FILE *log, char *filename, struct agnostic_bitmap *bmp){
	
	// Word of warning - bitmaps loaded by Allegro must check the 'Do not encode colorspace information'
	// option if they have been edited by GIMP.
	
	if (MENU_SCREEN_BPP == 8){
		//set_color_conversion(COLORCONV_REDUCE_TO_256);
		bmp->bmp = load_bitmap(filename, bmp->pal);
	} else {
		bmp->bmp = load_bitmap(filename, NULL);
	}
	if (!bmp->bmp){
		log_error(log, "[%s:%d]\t: (gfxLoadBMP)\t: Unable to load bitmap image: Error %s [errno %d]\n", __FILE__, __LINE__, allegro_error, errno);
		return -1;
	} else {
		log_debug(log, "[%s:%d]\t: (gfxLoadBMP)\t: Loaded bitmap image %dx%d\n", __FILE__, __LINE__, bmp->bmp->w, bmp->bmp->h);
	}
	return 0;
}

// Load a font bitmap from disk into a bitmap structure
int gfxLoadFont(FILE *log, char *filename, struct agnostic_bitmap *bmp){
	return gfxLoadBMP(log, filename, bmp);
}

// Unload driver
int gfxQuit(FILE *log){
	allegro_exit();
	return 0;
}

// Set screen mode
int gfxSetMode(FILE *log, struct agnostic_bitmap *display, int screen_w, int screen_h, int screen_bpp){
	int r  = 0;
	
	// Set bpp
	set_color_depth(MENU_SCREEN_BPP);
	// Set res
	r = set_gfx_mode(GFX_AUTODETECT, MENU_SCREEN_W, MENU_SCREEN_H, 0, 0);
	if (r != 0){
		log_error(log, "[%s:%d]\t: (gfxSetMode)\t: Unable to set Allegro screen mode %dx%dx%dbpp: Error %d\n", __FILE__, __LINE__, MENU_SCREEN_W, MENU_SCREEN_H, MENU_SCREEN_BPP, r);
		allegro_exit();
		return r;
	} else {
		log_info(log, "[%s:%d]\t: (gfxSetMode)\t: Allegro graphics mode set to %dx%dx%dbpp\n", __FILE__, __LINE__, MENU_SCREEN_W, MENU_SCREEN_H, MENU_SCREEN_BPP);
		log_debug(log, "[%s:%d]\t: (gfxSetMode)\t: Creating backbuffer\n", __FILE__, __LINE__);
		// Create screen sized buffer
		display->bmp = create_bitmap(MENU_SCREEN_W, MENU_SCREEN_H);
		clear_bitmap(display->bmp);
		log_debug(log, "[%s:%d]\t: (gfxSetMode)\t: Ready to go!\n", __FILE__, __LINE__);
		return r;
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