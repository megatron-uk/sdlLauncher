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

#ifdef TOS

// Load a Windows bitmap file from disk
int imageLoadBMP(FILE *log, char *src, struct agnostic_bitmap *bitmap){

	unsigned short bpp;
	
	if (src == NULL){
		log_warn(log, "[%s:%d]\t: (imageLoadBMP)\t: Filename argument was empty!\n", __FILE__, __LINE__);		
		return -1;	
	}
	
	log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: Loading external bitmap [%s]\n", __FILE__, __LINE__, src);
	bitmap->bmp->pixels = BMP_ReadFile(log, src);
	
	if (BMP_GetError() != BMP_OK){
		BMP_CHECK_ERROR(log, -1);
		log_error(log, "[%s:%d]\t: (imageLoadBMP)\t: Unable to load external bitmap [%s]\n", __FILE__, __LINE__, src);
		return -1;
	}
	bitmap->bmp->w = BMP_GetWidth(bitmap->bmp->pixels);
	bitmap->bmp->h = BMP_GetHeight(bitmap->bmp->pixels);
	bitmap->bmp->format->BitsPerPixel = BMP_GetDepth(bitmap->bmp->pixels);
	log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: Loaded bitmap [%dx%dx%dbpp]\n", __FILE__, __LINE__, bitmap->bmp->w, bitmap->bmp->h, bitmap->bmp->format->BitsPerPixel);
	
	return 0;
}

// Convert a loaded bitmap image to Atari VDI .IMG bitplane format 
int imageBMP2Bitplane(FILE *log, struct agnostic_bitmap *bitmap, int free){
	
	//int pixel_size = sizeof(bitmap->bmp->pixels);
	
	if (bitmap->bmp->pixels == NULL){
		log_warn(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: No pixel data founds\n", __FILE__, __LINE__);		
		return -1;	
	}
	
	log_debug(log, "[%s:%d]\t: (imageBMP2Bitplane)\t: Loaded bitmap [%dx%dx%dbpp]\n", __FILE__, __LINE__, bitmap->bmp->w, bitmap->bmp->h, bitmap->bmp->format->BitsPerPixel);
	
	// Auto free raw bitmap pixel data
	if (free){
		BMP_Free(bitmap->bmp->pixels);
	}
	return 0;
}

#endif