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
	t1 = clock() / (CLOCKS_PER_SEC / 1000);
	
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
	
	t2 = clock() / (CLOCKS_PER_SEC / 1000);
	
	log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: Loaded chunky bitmap [%dx%dx%dbpp @ %p] Time [%dms]\n", __FILE__, __LINE__, bitmap->w, bitmap->h, bitmap->bpp, (void *) bitmap->bmp->pixels, (t2 - t1));
		
	return 0;
}

// Convert a loaded bitmap image to Atari VDI .IMG bitplane format 
int imageBMP2Bitplane(FILE *log, struct agnostic_bitmap *bitmap, int free){
	
	int bitplanes_num = 4;
	int bitplane_px_size = 0;
	int i;
	int ii;
	int *p;
	
	int t1, t2;
	
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
			bitplane_px_size = bitmap->w * bitmap->h * 3;
			log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Using 3 bytes per pixel (24bpp)\n", __FILE__, __LINE__, bitmap->bpp);
			break;
		default:
			log_warn(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Unsupported colour depth %d\n", __FILE__, __LINE__, bitmap->bpp);
			break;
	}
	log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Allocating %d byte bitplane pixel buffer\n", __FILE__, __LINE__, bitplane_px_size);
	
	// Start timer
	t1 = clock() / (CLOCKS_PER_SEC / 1000);
	
	bitmap->bmp->bp_pixels = malloc(bitplane_px_size * sizeof(unsigned char));
	if (bitmap->bmp->bp_pixels == NULL){
		bitmap->bmp->bp_pixels = NULL;
		log_error(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Memory allocation error\n", __FILE__, __LINE__);
		return -1;
	}
	
	// Downsample 24bpp to 8bpp
	// https://stackoverflow.com/questions/40112383/24bpp-to-8bpp-conversion-c-with-raw-image-data
	/*
	unsigned char r, g, b;
	for(row = 0; row < 512; row++){
		for(col = 0; col < 512; col++){
			r = in[row][col][0] >> 5;   // keep 3 bits 
			g = in[row][col][1] >> 5;
			b = in[row][col][2] >> 6;   // keep 2 bits
			pix = (r << 5) | (g << 2) | b;
			fputc(pix, output);
		}
	}
	*/
	
	// Convert chunky to planar screen
	// This routine was sourced from SDL-1.2.15/src/video/ataricommon/ataric2p_s.h
	Atari_C2pConvert8(
		bitmap->bmp->pixels->Data,
		bitmap->bmp->bp_pixels,
		bitmap->w,
		bitmap->h,
		0,
		bitmap->w,
		(bitmap->w / 2)
	);
	
	//log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: FIRST here\n", __FILE__, __LINE__);
	// Fill in GEM VDI MFDB header
	
	//bitmap->bmp->mfdb->fd_addr = 
	//p = (void *) bitmap->bmp->bp_pixels;
	//bitmap->bmp->mfdb->fd_addr = p;
	/*
	bitmap->bmp->mfdb->fd_w = (unsigned short) bitmap->w;
	bitmap->bmp->mfdb->fd_h = (unsigned short) bitmap->h;
	bitmap->bmp->mfdb->fd_wdwidth = ((bitmap->w / sizeof(unsigned char)) / bitplanes_num) / 2;
	bitmap->bmp->mfdb->fd_stand = 1;
	bitmap->bmp->mfdb->fd_nplanes = bitplanes_num;
	bitmap->bmp->mfdb->fd_r1 = 0;
	bitmap->bmp->mfdb->fd_r2 = 0;
	bitmap->bmp->mfdb->fd_r3 = 0;
	*/
	//log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: here\n", __FILE__, __LINE__);
		
	//imagePrintMFDB(log, bitmap->bmp->mfdb);
	
	// End timer
	t2 = clock() / (CLOCKS_PER_SEC / 1000);
	log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Copy to planar pixel buffer [@ %p] Time [%dms]\n", __FILE__, __LINE__, (void *) &bitmap->bmp->bp_pixels, (t2 - t1));
	
	// Auto free raw bitmap pixel data
	if (free){
		log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Freeing raw bitmap data @ %p\n", __FILE__, __LINE__, (void *) bitmap->bmp->pixels);
		BMP_Free(bitmap->bmp->pixels);
		log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Done\n", __FILE__, __LINE__);
	}
	return 0;
}

#endif