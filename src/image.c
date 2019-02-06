#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// Our application headers 
#include "menu.h"
#include "logging.h"
#include "gfx.h"
#include "image.h"
#include "qdbmp.h"
#include "ataric2p_s.h"

#ifdef USE_GEM

// Print the contents of a MFDB block
void imagePrintMFDB(FILE *log, MFDB *mfdb){

	log_debug(log, "[%s:%d]\t: (imagePrintMFDB)\t: ================\n", __FILE__, __LINE__);
	log_debug(log, "[%s:%d]\t: (imagePrintMFDB)\t: MFDB fd_addrt %p\n", __FILE__, __LINE__, (void *) &mfdb->fd_addr);
	log_debug(log, "[%s:%d]\t: (imagePrintMFDB)\t: MFDB fd_w %d\n", __FILE__, __LINE__, mfdb->fd_w);
	log_debug(log, "[%s:%d]\t: (imagePrintMFDB)\t: MFDB fd_h %d\n", __FILE__, __LINE__, mfdb->fd_h);
	log_debug(log, "[%s:%d]\t: (imagePrintMFDB)\t: MFDB fd_wdwidth %d\n", __FILE__, __LINE__, mfdb->fd_wdwidth);
	log_debug(log, "[%s:%d]\t: (imagePrintMFDB)\t: MFDB fd_stand %d\n", __FILE__, __LINE__, mfdb->fd_stand);
	log_debug(log, "[%s:%d]\t: (imagePrintMFDB)\t: MFDB n_planes %d\n", __FILE__, __LINE__, mfdb->fd_nplanes);
	log_debug(log, "[%s:%d]\t: (imagePrintMFDB)\t: MFDB fd_r1 %d\n", __FILE__, __LINE__, mfdb->fd_r1);
	log_debug(log, "[%s:%d]\t: (imagePrintMFDB)\t: MFDB fd_r2 %d\n", __FILE__, __LINE__, mfdb->fd_r2);
	log_debug(log, "[%s:%d]\t: (imagePrintMFDB)\t: MFDB fd_r3 %d\n", __FILE__, __LINE__, mfdb->fd_r3);
}

// Load a Windows bitmap file from disk
int imageLoadBMP(FILE *log, char *src, struct agnostic_bitmap *bitmap){
	
	int t1, t2;
	t1 = clock();
	
	if (src == NULL){
		log_warn(log, "[%s:%d]\t: (imageLoadBMP)\t: Filename argument was empty!\n", __FILE__, __LINE__);		
		return -1;	
	}
	
	// Read bitmap data from data
	bitmap->bmp->pixels = BMP_ReadFile(log, src);
	if (BMP_GetError() != BMP_OK){
		BMP_CHECK_ERROR(log, -1);
		log_error(log, "[%s:%d]\t: (imageLoadBMP)\t: Unable to load external bitmap [%s]\n", __FILE__, __LINE__, src);
		return -1;
	}
	
	// Populate various Atari/GEM-specific data structures now
	bitmap->w = BMP_GetWidth(bitmap->bmp->pixels);
	bitmap->h = BMP_GetHeight(bitmap->bmp->pixels);
	bitmap->bpp = BMP_GetDepth(bitmap->bmp->pixels);
	bitmap->bmp->bp_pixels_set = true;
	
	t2 = clock();
	
	log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: Loaded chunky bitmap [%dx%dx%dbpp @ %p] Time [%.3fs]\n", __FILE__, __LINE__, bitmap->w, bitmap->h, bitmap->bpp, (void *) bitmap->bmp->pixels, (double) (t2 - t1) / CLOCKS_PER_SEC );
		
	return 0;
}

// Convert a loaded bitmap image to Atari VDI .IMG bitplane format 
int imageBMP2Bitplane(FILE *log, struct agnostic_bitmap *bitmap, int auto_free){
	
	int bitplanes_num = 4;
	int bitplane_px_size = 0;
	int row = 0;
	int col = 0;
	int t1, t2;
	unsigned char r, g, b, rgb, r_8, g_8, b_8, rgb_8;
	unsigned char *buf;
	unsigned char *ptr;
	bool used_buf = false;
	
	if (bitmap->bmp->pixels == NULL){
		log_warn(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: No pixel data founds\n", __FILE__, __LINE__);		
		return -1;	
	}
	
	// Allocate buffer for bitplane conversion
	switch(bitmap->bpp){
		case 8:
			bitplane_px_size = bitmap->w * bitmap->h;
			log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Using 1 bytes per pixel (8bpp)\n", __FILE__, __LINE__, bitmap->bpp);
			break;
		case 24:
			// Allocate a buffer to hold the processed data
			log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Using 3 bytes per pixel (24bpp)\n", __FILE__, __LINE__, bitmap->bpp);
			buf = malloc(bitmap->w * bitmap->h * sizeof(unsigned char));
			log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Downsample buffer allocated [%d bytes @ %p]\n", __FILE__, __LINE__, (bitmap->w * bitmap->h * sizeof(unsigned char)), (void *) buf);
			used_buf = true;
			t1 = clock();
			// Downsample 24bpp to 8bpp
			// https://stackoverflow.com/questions/40112383/24bpp-to-8bpp-conversion-c-with-raw-image-data
			for(row = 0; row < bitmap->h; row++){
				for(col = 0; col < bitmap->w; col++){
					BMP_GetPixelRGB(bitmap->bmp->pixels, row, col, &r, &g, &b);	
					r_8 = r >> 5;   // keep 3 bits 
					g_8 = g >> 5;	// kepp 3 bits
					b_8 = b >> 6;   // keep 2 bits
					rgb_8 = (r_8 << 5) | (g_8 << 2) | b_8;
					buf = &rgb_8;
					buf++;
				}
			}
			
			bitplane_px_size = bitmap->w * bitmap->h;
			t2 = clock();
			log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Now using 1 bytes per pixel (8bpp)\n", __FILE__, __LINE__, bitmap->bpp);
			log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Downsample time [%.3fs]\n", __FILE__, __LINE__, (double) (t2 - t1) / CLOCKS_PER_SEC);
			break;

		default:
			log_warn(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Unsupported colour depth %d\n", __FILE__, __LINE__, bitmap->bpp);
			return -1;
	}
	log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Allocating %d byte bitplane pixel buffer\n", __FILE__, __LINE__, bitplane_px_size);
	
	// Start timer
	t1 = clock();
	
	bitmap->bmp->bp_pixels = malloc(bitplane_px_size * sizeof(unsigned char));
	if (bitmap->bmp->bp_pixels == NULL){
		bitmap->bmp->bp_pixels = NULL;
		log_error(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Memory allocation error\n", __FILE__, __LINE__);
		return -1;
	}
		
	// Convert chunky to planar screen
	// This routine was sourced from SDL-1.2.15/src/video/ataricommon/ataric2p_s.h
	if (used_buf){
		ptr = buf;
		log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Using downsample buffer\n", __FILE__, __LINE__);
	} else {
		ptr = bitmap->bmp->pixels->Data;
		log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Using chunky bitmap\n", __FILE__, __LINE__);
	}
	Atari_C2pConvert8(
		ptr,
		bitmap->bmp->bp_pixels,
		bitmap->w,
		bitmap->h,
		0,
		bitmap->w,
		(bitmap->w / 2)
	);
	
	log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: here\n", __FILE__, __LINE__);
	
	// Fill in GEM VDI MFDB header
	bitmap->bmp->mfdb = malloc(sizeof(MFDB));
	bitmap->bmp->mfdb->fd_addr = bitmap->bmp->bp_pixels;
	bitmap->bmp->mfdb->fd_w = (unsigned short) bitmap->w;
	bitmap->bmp->mfdb->fd_h = (unsigned short) bitmap->h;
	bitmap->bmp->mfdb->fd_wdwidth = ((bitmap->w / sizeof(unsigned char)) / bitplanes_num) / 2;
	bitmap->bmp->mfdb->fd_stand = 1;
	bitmap->bmp->mfdb->fd_nplanes = bitplanes_num;
	bitmap->bmp->mfdb->fd_r1 = 0;
	bitmap->bmp->mfdb->fd_r2 = 0;
	bitmap->bmp->mfdb->fd_r3 = 0;	
	bitmap->bmp->mfdb_set = false;
	
	log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: here\n", __FILE__, __LINE__);
	
	// End timer
	t2 = clock();
	log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Copy to planar pixel buffer [@ %p] Time [%.3fs]\n", __FILE__, __LINE__, (void *) bitmap->bmp->bp_pixels, (double) (t2 - t1) / CLOCKS_PER_SEC);
	
	imagePrintMFDB(log, bitmap->bmp->mfdb);
	
	// Auto free raw bitmap pixel data
	if (auto_free){
		log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Freeing raw bitmap data @ %p\n", __FILE__, __LINE__, (void *) bitmap->bmp->pixels);
		BMP_Free(bitmap->bmp->pixels);
		bitmap->bmp->pixels_set = false;
	}
	
	// Free downsample buffer
	if (used_buf){
		log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Freeing downsample buffer @ %p\n", __FILE__, __LINE__, (void *) buf);
		free(buf);	
	}
	return 0;
}

#endif