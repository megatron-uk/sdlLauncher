#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// Our application headers 
#include "../menu.h"
#include "../misc/logging.h"

#ifdef USE_GEM
#include "../gfx/ataric2p_s.h"
#include "../gfx/gfx.h"
#include "../image/image.h"
#include "../image/qdbmp.h"

// Print the contents of a MFDB block
void imagePrintBitmap(FILE *log, struct agnostic_bitmap *bmp){

#ifdef LOGGING
	log_debug(log, "[%s:%d]\t: (imagePrintBitmap)\t: ========== Bitmap data ==========\n", __FILE__, __LINE__);

	log_debug(log, "[%s:%d]\t: (imagePrintBitmap)\t: MFDB\n", __FILE__, __LINE__);
	log_debug(log, "[%s:%d]\t: (imagePrintBitmap)\t: MFDB set? %d\n", __FILE__, __LINE__, bmp->bmp->mfdb_set);
	if (bmp->bmp->mfdb_set == 1){
		log_debug(log, "[%s:%d]\t: (imagePrintBitmap)\t: MFDB header @ %p\n", __FILE__, __LINE__, (void *) bmp->bmp->mfdb);
		log_debug(log, "[%s:%d]\t: (imagePrintBitmap)\t: MFDB fd_addrt %p <- This is where your planar pixels live!\n", __FILE__, __LINE__, (void *) bmp->bmp->mfdb->fd_addr);
		log_debug(log, "[%s:%d]\t: (imagePrintBitmap)\t: MFDB fd_w %d\n", __FILE__, __LINE__, bmp->bmp->mfdb->fd_w);
		log_debug(log, "[%s:%d]\t: (imagePrintBitmap)\t: MFDB fd_h %d\n", __FILE__, __LINE__, bmp->bmp->mfdb->fd_h);
		log_debug(log, "[%s:%d]\t: (imagePrintBitmap)\t: MFDB fd_wdwidth %d\n", __FILE__, __LINE__, bmp->bmp->mfdb->fd_wdwidth);
		log_debug(log, "[%s:%d]\t: (imagePrintBitmap)\t: MFDB fd_stand %d\n", __FILE__, __LINE__, bmp->bmp->mfdb->fd_stand);
		log_debug(log, "[%s:%d]\t: (imagePrintBitmap)\t: MFDB n_planes %d\n", __FILE__, __LINE__, bmp->bmp->mfdb->fd_nplanes);
		log_debug(log, "[%s:%d]\t: (imagePrintBitmap)\t: MFDB fd_r1 %d\n", __FILE__, __LINE__, bmp->bmp->mfdb->fd_r1);
		log_debug(log, "[%s:%d]\t: (imagePrintBitmap)\t: MFDB fd_r2 %d\n", __FILE__, __LINE__, bmp->bmp->mfdb->fd_r2);
		log_debug(log, "[%s:%d]\t: (imagePrintBitmap)\t: MFDB fd_r3 %d\n", __FILE__, __LINE__, bmp->bmp->mfdb->fd_r3);
	} else {
		log_debug(log, "[%s:%d]\t: (imagePrintBitmap)\t: MFDB not set\n", __FILE__, __LINE__);	
	}
	
	log_debug(log, "[%s:%d]\t: (imagePrintBitmap)\t:\n", __FILE__, __LINE__);
	log_debug(log, "[%s:%d]\t: (imagePrintBitmap)\t: Chunky bitmap\n", __FILE__, __LINE__);
	log_debug(log, "[%s:%d]\t: (imagePrintBitmap)\t: Chunky pixel set? %d\n", __FILE__, __LINE__, bmp->bmp->pixels_set);
	if (bmp->bmp->pixels_set == 1){
		log_debug(log, "[%s:%d]\t: (imagePrintBitmap)\t: Chunky pixel data @ %p\n", __FILE__, __LINE__, (void *) bmp->bmp->pixels);
	} else {
		log_debug(log, "[%s:%d]\t: (imagePrintBitmap)\t: Chunky pixel data - not set\n", __FILE__, __LINE__);
	}
	
	log_debug(log, "[%s:%d]\t: (imagePrintBitmap)\t:\n", __FILE__, __LINE__);
	log_debug(log, "[%s:%d]\t: (imagePrintBitmap)\t: Planar data\n", __FILE__, __LINE__);
	log_debug(log, "[%s:%d]\t: (imagePrintBitmap)\t: Planar data set? %d\n", __FILE__, __LINE__, bmp->bmp->bp_pixels_set);
	if (bmp->bmp->bp_pixels_set == 1){
		log_debug(log, "[%s:%d]\t: (imagePrintBitmap)\t: Planar pixel data @ %p\n", __FILE__, __LINE__, (void *) bmp->bmp->bp_pixels);		
	} else {
		log_debug(log, "[%s:%d]\t: (imagePrintBitmap)\t: Planar pixel data - not set\n", __FILE__, __LINE__);
	}
	log_debug(log, "[%s:%d]\t: (imagePrintBitmap)\t: ===== End of Bitmap display =====\n", __FILE__, __LINE__);
#endif
}

// Load a Windows bitmap file from disk
int imageLoadBMP(FILE *log, char *src, struct agnostic_bitmap *bmp){
	
	int t1, t2;
	t1 = clock();
		
	if (src == NULL){
		log_warn(log, "[%s:%d]\t: (imageLoadBMP)\t: Filename argument was empty!\n", __FILE__, __LINE__);		
		return -1;	
	}
	
	// Read bitmap data from data
	bmp->bmp->pixels = BMP_ReadFile(log, src);
	if (BMP_GetError() != BMP_OK){
		BMP_CHECK_ERROR(log, -1);
		log_error(log, "[%s:%d]\t: (imageLoadBMP)\t: Unable to load external bitmap [%s]\n", __FILE__, __LINE__, src);
		return -1;
	}
	
	// Populate various Atari/GEM-specific data structures now
	bmp->w = BMP_GetWidth(bmp->bmp->pixels);
	bmp->h = BMP_GetHeight(bmp->bmp->pixels);
	bmp->bpp = BMP_GetDepth(bmp->bmp->pixels);
	bmp->bmp->pixels_set = 1; // Flag to indicate we've used this data structure
	
	t2 = clock();
	
	log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: Loaded chunky bitmap [%dx%dx%dbpp @ %p] Time [%.3fs]\n", __FILE__, __LINE__, bmp->w, bmp->h, bmp->bpp, (void *) bmp->bmp->pixels, (double) (t2 - t1) / CLOCKS_PER_SEC );
		
	return 0;
}

// Convert a loaded bitmap image to Atari VDI .IMG bitplane format 
int imageBMP2Bitplane(FILE *log, struct agnostic_bitmap *bmp, int auto_free){
	
	int bitplanes_num = 4;
	int bitplane_px_size = 0;
	int row = 0;
	int col = 0;
	int t1, t2;
	unsigned char r, g, b, rgb, r_8, g_8, b_8, rgb_8;
	unsigned char *buf;
	unsigned char *ptr;
	bool used_buf = false;
	
	if (bmp->bmp->pixels == NULL){
		log_warn(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: No pixel data founds\n", __FILE__, __LINE__);
		return -1;	
	}
	
	// Allocate buffer for bitplane conversion
	switch(bmp->bpp){
		case 8:
			bitplane_px_size = bmp->w * bmp->h;
			log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Using 1 bytes per pixel (8bpp)\n", __FILE__, __LINE__, bmp->bpp);
			break;
		case 24:
			// Allocate a buffer to hold the processed data
			log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Using 3 bytes per pixel (24bpp)\n", __FILE__, __LINE__, bmp->bpp);
			buf = malloc(bmp->w * bmp->h * sizeof(unsigned char));
			log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Downsample buffer allocated [%d bytes @ %p]\n", __FILE__, __LINE__, (bmp->w * bmp->h * sizeof(unsigned char)), (void *) buf);
			used_buf = true;
			t1 = clock();
			// Downsample 24bpp to 8bpp
			// https://stackoverflow.com/questions/40112383/24bpp-to-8bpp-conversion-c-with-raw-image-data
			for(row = 0; row < bmp->h; row++){
				for(col = 0; col < bmp->w; col++){
					BMP_GetPixelRGB(bmp->bmp->pixels, row, col, &r, &g, &b);	
					r_8 = r >> 5;   // keep 3 bits 
					g_8 = g >> 5;	// kepp 3 bits
					b_8 = b >> 6;   // keep 2 bits
					rgb_8 = (r_8 << 5) | (g_8 << 2) | b_8;
					buf = &rgb_8;
					buf++;
				}
			}
			
			bitplane_px_size = bmp->w * bmp->h;
			t2 = clock();
			log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Now using 1 bytes per pixel (8bpp)\n", __FILE__, __LINE__, bmp->bpp);
			log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Downsample time [%.3fs]\n", __FILE__, __LINE__, (double) (t2 - t1) / CLOCKS_PER_SEC);
			break;

		default:
			log_warn(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Unsupported colour depth %d\n", __FILE__, __LINE__, bmp->bpp);
			return -1;
	}
	
	// Start timer
	t1 = clock();
	
	bmp->bmp->bp_pixels = malloc(bitplane_px_size * sizeof(unsigned char));
	if (bmp->bmp->bp_pixels == NULL){
		log_error(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Memory allocation error for planar pixel buffer\n", __FILE__, __LINE__);
		return -1;
	} else {
		log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Allocated bitplane pixel buffer [%d bytes @ %p]\n", __FILE__, __LINE__, bitplane_px_size, (void *) bmp->bmp->bp_pixels);
	}
		
	// Convert chunky to planar screen
	// This routine was sourced from SDL-1.2.15/src/video/ataricommon/ataric2p_s.h
	if (used_buf){
		ptr = buf;
		log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Using downsample buffer [@ %p]\n", __FILE__, __LINE__, (void *) buf);
	} else {
		ptr = bmp->bmp->pixels->Data;
		log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Using chunky bitmap [@ %p]\n", __FILE__, __LINE__, (void *) bmp->bmp->pixels->Data);
	}
	Atari_C2pConvert8(
		ptr,
		bmp->bmp->bp_pixels,
		bmp->w,
		bmp->h,
		0,
		bmp->w,
		(bmp->w / 2)
	);
	
	// End timer for C2P routine
	t2 = clock();
		
	// Allocate and fill in GEM VDI MFDB header
	bmp->bmp->mfdb = malloc(sizeof(MFDB));
	if (bmp->bmp->mfdb == NULL){
		log_error(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Memory allocation error for MFDB header\n", __FILE__, __LINE__);
		return -1;	
	} else {
		log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: New MFDB header @ %p\n", __FILE__, __LINE__, (void *) bmp->bmp->mfdb);	
	}
	bmp->bmp->mfdb->fd_addr = bmp->bmp->bp_pixels;
	bmp->bmp->mfdb->fd_w = (unsigned short) bmp->w;
	bmp->bmp->mfdb->fd_h = (unsigned short) bmp->h;
	bmp->bmp->mfdb->fd_wdwidth = ((bmp->w / sizeof(unsigned char)) / bitplanes_num) / 2;
	bmp->bmp->mfdb->fd_stand = 1;
	bmp->bmp->mfdb->fd_nplanes = bitplanes_num;
	bmp->bmp->mfdb->fd_r1 = 0;
	bmp->bmp->mfdb->fd_r2 = 0;
	bmp->bmp->mfdb->fd_r3 = 0;	
	
	log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: C2P [%p -> %p] Time [%.3fs]\n", __FILE__, __LINE__, (void *) ptr, (void *) bmp->bmp->bp_pixels, (double) (t2 - t1) / CLOCKS_PER_SEC);
	//imagePrintBitmap(log, bmp);
	
	// Auto free raw bitmap pixel data
	if (auto_free){
		log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Freeing raw bitmap data @ %p\n", __FILE__, __LINE__, (void *) bmp->bmp->pixels);
		BMP_Free(bmp->bmp->pixels);
		bmp->bmp->pixels_set = 0; // Flag to indicate we've destroyed this data structure
	}
	
	// Free downsample buffer
	if (used_buf){
		log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Freeing downsample buffer @ %p\n", __FILE__, __LINE__, (void *) buf);
		free(buf);	
	}
	return 0;
}

#endif