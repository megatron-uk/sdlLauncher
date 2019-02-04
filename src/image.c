#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// Our application headers 
#include "menu.h"
#include "logging.h"
#include "gfx.h"
#include "image.h"
#include "qdbmp.h"
#include "ataric2p_s.h"

#ifdef USE_GEM

// Load a Windows bitmap file from disk
int imageLoadBMP(FILE *log, char *src, struct agnostic_bitmap *bitmap){
		
	if (src == NULL){
		log_warn(log, "[%s:%d]\t: (imageLoadBMP)\t: Filename argument was empty!\n", __FILE__, __LINE__);		
		return -1;	
	}
	
	// Read bitmap data from data
	log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: Loading external bitmap\n", __FILE__, __LINE__, src);
	bitmap->bmp->pixels = BMP_ReadFile(log, src);
	if (BMP_GetError() != BMP_OK){
		BMP_CHECK_ERROR(log, -1);
		log_error(log, "[%s:%d]\t: (imageLoadBMP)\t: Unable to load external bitmap [%s]\n", __FILE__, __LINE__, src);
		return -1;
	}
	
	// Populate various Atari/GEM-specific data structures now
	bitmap->bmp->w = BMP_GetWidth(bitmap->bmp->pixels);
	bitmap->bmp->h = BMP_GetHeight(bitmap->bmp->pixels);
	bitmap->bmp->format.BitsPerPixel = BMP_GetDepth(bitmap->bmp->pixels);
	log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: Loaded bitmap [%dx%dx%dbpp]\n", __FILE__, __LINE__, bitmap->bmp->w, bitmap->bmp->h, bitmap->bmp->format.BitsPerPixel);
		
	return 0;
}

// Convert a loaded bitmap image to Atari VDI .IMG bitplane format 
int imageBMP2Bitplane(FILE *log, struct agnostic_bitmap *bitmap, int free){
	
	//int pixel_size = sizeof(bitmap->bmp->pixels);
	
	if (bitmap->bmp->pixels == NULL){
		log_warn(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: No pixel data founds\n", __FILE__, __LINE__);		
		return -1;	
	}
	
	log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Loaded pixel data [%dx%dx%dbpp]\n", __FILE__, __LINE__, bitmap->bmp->w, bitmap->bmp->h, bitmap->bmp->format.BitsPerPixel);
	
	log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Pixel data %d bytes\n", __FILE__, __LINE__, bitmap->bmp->pixels->Header.ImageDataSize);

	// Convert chunky to planar screen
	/*
	Atari_C2pConvert(
		bitmap->bmp->pixels->Data,
		destination,
		x2-x1,
		rects[i].h,
		doubleline,
		surface->pitch,
		XBIOS_pitch
	);
	*/
	
	// Auto free raw bitmap pixel data
	if (free){
		log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Freeing raw bitmap data\n", __FILE__, __LINE__);
		BMP_Free(bitmap->bmp->pixels);
		log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Done\n", __FILE__, __LINE__);
	}
	return 0;
}

#endif