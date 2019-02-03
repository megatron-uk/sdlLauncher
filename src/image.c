#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// Our application headers 
#include "menu.h"
#include "logging.h"
#include "image.h"

// Bitmap file types - indicated by header size field
#define BITMAPINFOHEADER 40
#define BITMAPV2INFOHEADER 52
#define BITMAPV3INFOHEADER 56
#define BITMAPV4HEADER 108
#define BITMAPV5HEADER 124

// ===========================================
//
// Image data structures shamelessly ripped from:
// SDL-1.2.15/include/SDL_video.h
//
// ===========================================

#define RW_SEEK_SET     0       /**< Seek from the beginning of data */
#define RW_SEEK_CUR     1       /**< Seek relative to current read point */
#define RW_SEEK_END     2       /**< Seek relative to the end of data */

#ifndef BI_RGB
#define BI_RGB		0
#define BI_RLE8		1
#define BI_RLE4		2
#define BI_BITFIELDS	3
#endif

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;
typedef int8_t          Sint8;
typedef uint8_t         Uint8;
typedef int16_t         Sint16;
typedef uint16_t        Uint16;
typedef int32_t         Sint32;
typedef uint32_t        Uint32;

typedef struct {
        Uint32 hi;
        Uint32 lo;
} Uint64, Sint64;

typedef struct imageRect {
	Sint16 x, y;
	Uint16 w, h;
} imageRect;

typedef struct imageColor {
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 unused;
} imageColor;
#define imageColour imageColor

typedef struct imagePalette {
	int       ncolors;
	imageColor *colors;
} imagePalette;

typedef struct imagePixelFormat {
	imagePalette *palette;
	Uint8  BitsPerPixel;
	Uint8  BytesPerPixel;
	Uint8  Rloss;
	Uint8  Gloss;
	Uint8  Bloss;
	Uint8  Aloss;
	Uint8  Rshift;
	Uint8  Gshift;
	Uint8  Bshift;
	Uint8  Ashift;
	Uint32 Rmask;
	Uint32 Gmask;
	Uint32 Bmask;
	Uint32 Amask;
	/** RGB color key information */
	Uint32 colorkey;
	/** Alpha value information (per-surface alpha) */
	Uint8  alpha;
} imagePixelFormat;

typedef struct imageSurface {
	Uint32 flags;                           /**< Read-only */
	imagePixelFormat *format;                /**< Read-only */
	int w, h;                               /**< Read-only */
	Uint16 pitch;                           /**< Read-only */
	void *pixels;                           /**< Read-write */
	int offset;                             /**< Private */
	struct private_hwdata *hwdata;
	struct imageRect clip_rect;             /**< Read-only */
	Uint32 unused1;                         /**< for binary compatibility */
	Uint32 locked;                          /**< Private */
	//struct SDL_BlitMap *map;                /**< Private */
	unsigned int format_version;            /**< Private */
	int refcount;                           /**< Read-mostly */
} imageSurface;

typedef struct imageContext {
	Uint32 type;
	FILE *fp;
} imageContext;

// ==========================================================
//
// Utility functions for m68k platforms, shamelessly ripped
// from:
// SDL-1.2.15/include/SDL_endian.h
//
// ==========================================================

#define static_cast(type, expression) ((type)(expression))

#ifdef USE_BIG_ENDIAN
// __m68k__
static __inline__ Uint16 imageSwap16(Uint16 x){
	__asm__("rorw #8,%0" : "=d" (x) :  "0" (x) : "cc");
	return x;
}
// __m68k__
static __inline__ Uint32 imageSwap32(Uint32 x){
	__asm__("rorw #8,%0\n\tswap %0\n\trorw #8,%0" : "=d" (x) :  "0" (x) : "cc");
	return x;
}
#else
static __inline__ Uint16 imageSwap16(Uint16 x) {
	return static_cast(Uint16, ((x<<8)|(x>>8)));
}
static __inline__ Uint32 imageSwap32(Uint32 x) {
	return static_cast(Uint32, ((x<<24)|((x<<8)&0x00FF0000)|((x>>8)&0x0000FF00)|(x>>24)));
}
#endif 

#ifdef USE_BIG_ENDIAN
#define imageSwapLE16(X)	imageSwap16(X)
#define imageSwapLE32(X)	imageSwap32(X)
#else
#define imageSwapLE16(X)	(X)
#define imageSwapLE32(X)	(X)
#endif

void imageFreeRW(imageContext *area){
	//imagefree(area);
}

// ===============================================
//
// Re-implementation of SDL_rwops routines without
// all of the bitplane, colourspace and SDL surface
// stuff that we won't use on non-SDL backends.
//
// ===============================================

int imageSeek(FILE *log, struct imageContext *context, int offset, int whence){
	if (fseek(context->fp, offset, whence) == 0){
		log_debug(log, "[%s:%d]\t: (imageSeek)\t: Image seek %d from %d\n", __FILE__, __LINE__, offset, whence);
		return(ftell(context->fp));
	} else {
		log_error(log, "[%s:%d]\t: (imageSeek)\t: Image context seek error\n", __FILE__, __LINE__);
		return(-1);
	}
}

int imageRead(FILE *log, struct imageContext *context, void *ptr, int size, int maxnum){
	size_t nread;

	nread = fread(ptr, size, maxnum, context->fp); 
	if (nread == 0 && ferror(context->fp)){
		log_error(log, "[%s:%d]\t: (imageRead)\t: Image context read error\n", __FILE__, __LINE__);
	} else {
		log_debug(log, "[%s:%d]\t: (imageRead)\t: Image context read [%d] bytes\n", __FILE__, __LINE__, size);
	}
	return(nread);
}

int imageWrite(FILE *log, struct imageContext *context, const void *ptr, int size, int num){
	size_t nwrote;

	nwrote = fwrite(ptr, size, num, context->fp);
	if (nwrote == 0 && ferror(context->fp)){
		log_error(log, "[%s:%d]\t: (imageWrite)\t: Image context write error\n", __FILE__, __LINE__);
	} else {
		log_debug(log, "[%s:%d]\t: (imageWrite)\t: Image context wrote [%d] bytes\n", __FILE__, __LINE__, size);
	}
	return(nwrote);	
}

int imageClose(FILE *log, struct imageContext *context){
	if (context){
		fclose(context->fp);
		imageFreeRW(context);
	}
	return(0);
}

Uint16 imageReadLE16(FILE *log, struct imageContext *context){
	Uint16 value;
	imageRead(log, context, &value, (sizeof value), 1);
	return(imageSwapLE32(value));
}

Uint32 imageReadLE32(FILE *log, struct imageContext *context){
	Uint32 value;
	imageRead(log, context, &value, (sizeof value), 1);
	return(imageSwapLE32(value));
}

imagePixelFormat *imageAllocFormat(FILE *log, int bpp,
			Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask){
	imagePixelFormat *format;
	Uint32 mask;

	/* Allocate an empty pixel format structure */
	format = malloc(sizeof(*format));
	if ( format == NULL ) {
		log_error(log, "[%s:%d]\t: (imageAllocFormat)\t: Out of memory\n", __FILE__, __LINE__);
		return(NULL);
	}
	memset(format, 0, sizeof(*format));
	format->alpha = 0;

	/* Set up the format */
	format->BitsPerPixel = bpp;
	format->BytesPerPixel = (bpp+7)/8;
	if ( Rmask || Bmask || Gmask ) { /* Packed pixels with custom mask */
		format->palette = NULL;
		format->Rshift = 0;
		format->Rloss = 8;
		if ( Rmask ) {
			for ( mask = Rmask; !(mask&0x01); mask >>= 1 )
				++format->Rshift;
			for ( ; (mask&0x01); mask >>= 1 )
				--format->Rloss;
		}
		format->Gshift = 0;
		format->Gloss = 8;
		if ( Gmask ) {
			for ( mask = Gmask; !(mask&0x01); mask >>= 1 )
				++format->Gshift;
			for ( ; (mask&0x01); mask >>= 1 )
				--format->Gloss;
		}
		format->Bshift = 0;
		format->Bloss = 8;
		if ( Bmask ) {
			for ( mask = Bmask; !(mask&0x01); mask >>= 1 )
				++format->Bshift;
			for ( ; (mask&0x01); mask >>= 1 )
				--format->Bloss;
		}
		format->Ashift = 0;
		format->Aloss = 8;
		if ( Amask ) {
			for ( mask = Amask; !(mask&0x01); mask >>= 1 )
				++format->Ashift;
			for ( ; (mask&0x01); mask >>= 1 )
				--format->Aloss;
		}
		format->Rmask = Rmask;
		format->Gmask = Gmask;
		format->Bmask = Bmask;
		format->Amask = Amask;
	} else if ( bpp > 8 ) {		/* Packed pixels with standard mask */
		/* R-G-B */
		if ( bpp > 24 )
			bpp = 24;
		format->Rloss = 8-(bpp/3);
		format->Gloss = 8-(bpp/3)-(bpp%3);
		format->Bloss = 8-(bpp/3);
		format->Rshift = ((bpp/3)+(bpp%3))+(bpp/3);
		format->Gshift = (bpp/3);
		format->Bshift = 0;
		format->Rmask = ((0xFF>>format->Rloss)<<format->Rshift);
		format->Gmask = ((0xFF>>format->Gloss)<<format->Gshift);
		format->Bmask = ((0xFF>>format->Bloss)<<format->Bshift);
	} else {
		/* Palettized formats have no mask info */
		format->Rloss = 8;
		format->Gloss = 8;
		format->Bloss = 8;
		format->Aloss = 8;
		format->Rshift = 0;
		format->Gshift = 0;
		format->Bshift = 0;
		format->Ashift = 0;
		format->Rmask = 0;
		format->Gmask = 0;
		format->Bmask = 0;
		format->Amask = 0;
	}
	if ( bpp <= 8 ) {			/* Palettized mode */
		int ncolors = 1<<bpp;
		format->palette = (imagePalette *)malloc(sizeof(imagePalette));
		if ( format->palette == NULL ) {
			//SDL_FreeFormat(format);
			log_error(log, "[%s:%d]\t: (imageAllocFormat)\t: Out of memory\n", __FILE__, __LINE__);
			return(NULL);
		}
		(format->palette)->ncolors = ncolors;
		(format->palette)->colors = (imageColor *)malloc(
				(format->palette)->ncolors*sizeof(imageColor));
		if ( (format->palette)->colors == NULL ) {
			//SDL_FreeFormat(format);
			log_error(log, "[%s:%d]\t: (imageAllocFormat)\t: Out of memory\n", __FILE__, __LINE__);
			return(NULL);
		}
		if ( Rmask || Bmask || Gmask ) {
			/* create palette according to masks */
			int i;
			int Rm=0,Gm=0,Bm=0;
			int Rw=0,Gw=0,Bw=0;
			if(Rmask)
			{
				Rw=8-format->Rloss;
				for(i=format->Rloss;i>0;i-=Rw)
					Rm|=1<<i;
			}
			if(Gmask)
			{
				Gw=8-format->Gloss;
				for(i=format->Gloss;i>0;i-=Gw)
					Gm|=1<<i;
			}
			if(Bmask)
			{
				Bw=8-format->Bloss;
				for(i=format->Bloss;i>0;i-=Bw)
					Bm|=1<<i;
			}
			for(i=0; i < ncolors; ++i) {
				int r,g,b;
				r=(i&Rmask)>>format->Rshift;
				r=(r<<format->Rloss)|((r*Rm)>>Rw);
				format->palette->colors[i].r=r;

				g=(i&Gmask)>>format->Gshift;
				g=(g<<format->Gloss)|((g*Gm)>>Gw);
				format->palette->colors[i].g=g;

				b=(i&Bmask)>>format->Bshift;
				b=(b<<format->Bloss)|((b*Bm)>>Bw);
				format->palette->colors[i].b=b;
				format->palette->colors[i].unused=0;
			}
		} else if ( ncolors == 2 ) {
			/* Create a black and white bitmap palette */
			format->palette->colors[0].r = 0xFF;
			format->palette->colors[0].g = 0xFF;
			format->palette->colors[0].b = 0xFF;
			format->palette->colors[1].r = 0x00;
			format->palette->colors[1].g = 0x00;
			format->palette->colors[1].b = 0x00;
		} else {
			/* Create an empty palette */
			memset((format->palette)->colors, 0,
				(format->palette)->ncolors*sizeof(imageColor));
		}
	}
	return(format);
}

imageSurface * imageCreateRGBSurface (FILE *log, Uint32 flags,
			int width, int height, int depth,
			Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask){
	imageSurface *screen;
	imageSurface *surface;

	/* Make sure the size requested doesn't overflow our datatypes */
	/* Next time I write a library like SDL, I'll use int for size. :) */
	if ( width >= 16384 || height >= 65536 ) {
		log_error(log, "[%s:%d]\t: (imageCreateRGBSurface)\t: Width or height is too large\n", __FILE__, __LINE__);
		return(NULL);
	}

	/* Check to see if we desire the surface in video memory */
	screen = NULL;

	/* Allocate the surface */
	surface = (imageSurface *)malloc(sizeof(*surface));
	if ( surface == NULL ) {
		log_error(log, "[%s:%d]\t: (imageCreateRGBSurface)\t: Out of memory\n", __FILE__, __LINE__);
		return(NULL);
	}
	depth = screen->format->BitsPerPixel;
	Rmask = screen->format->Rmask;
	Gmask = screen->format->Gmask;
	Bmask = screen->format->Bmask;
	Amask = screen->format->Amask;
	
	surface->format = imageAllocFormat(log, depth, Rmask, Gmask, Bmask, Amask);
	if ( surface->format == NULL ) {
		//SDL_free(surface);
		return(NULL);
	}
	surface->w = width;
	surface->h = height;
	//surface->pitch = SDL_CalculatePitch(surface);
	surface->pixels = NULL;
	surface->offset = 0;
	surface->hwdata = NULL;
	surface->locked = 0;
	//surface->map = NULL;
	surface->unused1 = 0;
	//SDL_SetClipRect(surface, NULL);
	//SDL_FormatChanged(surface);

	// Get the pixels
	/*
	if ( ((flags&SDL_HWSURFACE) == SDL_SWSURFACE) || 
				(video->AllocHWSurface(this, surface) < 0) ) {
		if ( surface->w && surface->h ) {
			surface->pixels = malloc(surface->h*surface->pitch);
			if ( surface->pixels == NULL ) {
				//SDL_FreeSurface(surface);
				log_error(log. "[%s:%d]\t: (imageCreateRGBSurface)\t: Out of memory\n", __FILE__, __LINE__);
				return(NULL);
			}
			// This is important for bitmaps 
			memset(surface->pixels, 0, surface->h*surface->pitch);
		}
	}*/

	/* Allocate an empty mapping */
	//surface->map = SDL_AllocBlitMap();
	//if ( surface->map == NULL ) {
		//SDL_FreeSurface(surface);
	//	return(NULL);
	//}

	/* The surface is ready to go */
	surface->refcount = 1;
	return(surface);
}

// ===========================================================
//
// Public image functions for platforms not using SDL.
//
// Load bitmap from disk. This was shamelessly ripped from:
// SDL-1.2.15/src/SDL_bmp.c
//
// ===========================================================
int imageLoadBMP(FILE *log, char *src, int freesrc){
	FILE *fp;
	bool was_error;
	long fp_offset = 0;
	int bmpPitch;
	int i, pad;
	Uint32 Rmask;
	Uint32 Gmask;
	Uint32 Bmask;
	imageSurface *surface;
	imagePalette *palette;
	Uint8 *bits;
	Uint8 *top, *end;
	bool topDown;
	int ExpandBMP;

	/* The Win32 BMP file header (14 bytes) */
	char   magic[2];
	Uint32 bfSize;
	Uint16 bfReserved1;
	Uint16 bfReserved2;
	Uint32 bfOffBits;

	/* The Win32 BITMAPINFOHEADER struct (40 bytes) */
	Uint32 biSize;
	Sint32 biWidth;
	Sint32 biHeight;
	Uint16 biPlanes;
	Uint16 biBitCount;
	Uint32 biCompression;
	Uint32 biSizeImage;
	Sint32 biXPelsPerMeter;
	Sint32 biYPelsPerMeter;
	Uint32 biClrUsed;
	Uint32 biClrImportant;

	imageContext ctx;
	
#ifdef USE_BIG_ENDIAN
	log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: BIG_ENDIAN\n", __FILE__, __LINE__);
#else
	log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: LITTLE_ENDIAN\n", __FILE__, __LINE__);
#endif
	
	// Make sure we are passed a valid data source
	surface = NULL;
	was_error = false;
	if (src == NULL){
		log_error(log, "[%s:%d]\t: (imageLoadBMP)\t: Source file is NULL\n", __FILE__, __LINE__);
		was_error = true;
		goto done;
	} else {
		log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: Source file name %s\n", __FILE__, __LINE__, src);
	}

	ctx.fp = fopen(src, "r");
	if (ctx.fp == NULL){
		log_error(log, "[%s:%d]\t: (imageLoadBMP)\t: File open error\n", __FILE__, __LINE__);
		was_error = true;
		goto done;
	} else {
		log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: File opened\n", __FILE__, __LINE__);
	}
	
	// Read in the BMP file header
	//fp_offset = SDL_RWtell(src);
	fp_offset = imageSeek(log, &ctx, 0, RW_SEEK_CUR);
	if (imageRead(log, &ctx, magic, 1, 2) != 2){
		log_error(log, "[%s:%d]\t: (imageLoadBMP)\t: File read error\n", __FILE__, __LINE__);
		was_error = true;
		goto done;
	} else {
		log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: File read\n", __FILE__, __LINE__);
	}
	if (strncmp(magic, "BM", 2) != 0){
		log_error(log, "[%s:%d]\t: (imageLoadBMP)\t: File is not a Windows BMP file\n", __FILE__, __LINE__);
		was_error = true;
		goto done;
	} else {
		log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: File is BMP [%s]\n", __FILE__, __LINE__, magic);	
	}
	
	log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: Read bfSize\n", __FILE__, __LINE__);
	bfSize		= imageReadLE32(log, &ctx);
	log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: File size is %d\n", __FILE__, __LINE__, bfSize);

	log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: Read bfReserved1\n", __FILE__, __LINE__);
	bfReserved1	= imageReadLE16(log, &ctx);
	log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: Read bfReserved1\n", __FILE__, __LINE__);
	bfReserved2	= imageReadLE16(log, &ctx);
	log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: Read data offset\n", __FILE__, __LINE__);
	bfOffBits	= imageReadLE32(log, &ctx);
	log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: Data aofset [%d]\n", __FILE__, __LINE__, bfOffBits);
	
	// Read the Win32 BITMAPINFOHEADER
	log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: Read header size\n", __FILE__, __LINE__);
	biSize		= imageReadLE32(log, &ctx);
	log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: Header size [%d]\n", __FILE__, __LINE__, biSize);
	switch(biSize){
		case(BITMAPINFOHEADER):
			log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: Format is standard BMP\n", __FILE__, __LINE__);
			break;
		case(BITMAPV2INFOHEADER):
			log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: Format is BMP v2\n", __FILE__, __LINE__);
			break;
		case(BITMAPV3INFOHEADER):
			log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: Format is BMP v3\n", __FILE__, __LINE__);
			break;
		case(BITMAPV4HEADER):
			log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: Format is BMP v4\n", __FILE__, __LINE__);
			break;
		case(BITMAPV5HEADER):
			log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: Format is BMP v5\n", __FILE__, __LINE__);
			break;
		default:
			break;
	}
	
	if (biSize == 12){
		biWidth		= (Uint32)imageReadLE16(log, &ctx);
		biHeight	= (Uint32)imageReadLE16(log, &ctx);
		biPlanes	= imageReadLE16(log, &ctx);
		biBitCount	= imageReadLE16(log, &ctx);
		biCompression	= BI_RGB;
		biSizeImage	= 0;
		biXPelsPerMeter	= 0;
		biYPelsPerMeter	= 0;
		biClrUsed	= 0;
		biClrImportant	= 0;
	} else {
		biWidth		= imageReadLE32(log, &ctx);
		biHeight	= imageReadLE32(log, &ctx);
		biPlanes	= imageReadLE16(log, &ctx);
		biBitCount	= imageReadLE16(log, &ctx);
		biCompression	= imageReadLE32(log, &ctx);
		biSizeImage	= imageReadLE32(log, &ctx);
		biXPelsPerMeter	= imageReadLE32(log, &ctx);
		biYPelsPerMeter	= imageReadLE32(log, &ctx);
		biClrUsed	= imageReadLE32(log, &ctx);
		biClrImportant	= imageReadLE32(log, &ctx);
	}

	log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: Geometry [%dx%d]\n", __FILE__, __LINE__, biWidth, biHeight);
	log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: Planes [%d]\n", __FILE__, __LINE__, biPlanes);
	log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: Bitcount [%d]\n", __FILE__, __LINE__, biBitCount);
	log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: Colours [%d]\n", __FILE__, __LINE__, biClrUsed);
	log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: Compression [%d]\n", __FILE__, __LINE__, biCompression);
	
	// stop some compiler warnings.
	(void) bfSize;
	(void) bfReserved1;
	(void) bfReserved2;
	(void) biPlanes;
	(void) biSizeImage;
	(void) biXPelsPerMeter;
	(void) biYPelsPerMeter;
	(void) biClrImportant;

	if (biHeight < 0){
		log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: Inverted image\n", __FILE__, __LINE__);
		topDown = true;
		biHeight = -biHeight;
	} else {
		topDown = false;
	}

	// Check for read error
	//if ( SDL_strcmp(SDL_GetError(), "") != 0 ) {
	//	was_error = true;
	//	goto done;
	//}
	
	// Expand 1 and 4 bit bitmaps to 8 bits per pixel
	switch (biBitCount){
		case 1:
		case 4:
			ExpandBMP = biBitCount;
			biBitCount = 8;
			break;
		default:
			ExpandBMP = 0;
			break;
	}
	
	// We don't support any BMP compression right now
	Rmask = Gmask = Bmask = 0;
	switch (biCompression){
		case BI_RGB:
			// If there are no masks, use the defaults
			if (bfOffBits == (14+biSize)){
				// Default values for the BMP format
				switch (biBitCount){
					case 15:
					case 16:
						Rmask = 0x7C00;
						Gmask = 0x03E0;
						Bmask = 0x001F;
						break;
					case 24:
// On 68k swap endian-ness
#ifdef USE_BIG_ENDIAN
							log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: Big-endian RGB mask-swap\n", __FILE__, __LINE__);
					        Rmask = 0x000000FF;
					        Gmask = 0x0000FF00;
					        Bmask = 0x00FF0000;
						break;
#endif
					case 32:
						Rmask = 0x00FF0000;
						Gmask = 0x0000FF00;
						Bmask = 0x000000FF;
						break;
					default:
						break;
				}
				break;
			}
			// Fall through -- read the RGB masks

		case BI_BITFIELDS:
			switch (biBitCount){
				case 15:
				case 16:
				case 32:
					Rmask = imageReadLE32(log, &ctx);
					Gmask = imageReadLE32(log, &ctx);
					Bmask = imageReadLE32(log, &ctx);
					break;
				default:
					break;
			}
			break;
		default:
			log_error(log, "[%s:%d]\t: (imageLoadBMP)\t: Compressed BMP files unsupported\n", __FILE__, __LINE__);
			was_error = true;
			goto done;
	}
	
	// Create a compatible surface, note that the colors are RGB ordered
	log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t:\n", __FILE__, __LINE__);
	surface = imageCreateRGBSurface(log, 0, biWidth, biHeight, biBitCount, Rmask, Gmask, Bmask, 0);
	//if (surface == NULL ){
		//was_error = true;
		//goto done;
	//}

	// Load the palette, if any
	log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t:\n", __FILE__, __LINE__);
	palette = (surface->format)->palette;
	log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t:\n", __FILE__, __LINE__);
	if (palette){
		if (biClrUsed == 0){
			log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t:\n", __FILE__, __LINE__);
			biClrUsed = 1 << biBitCount;
			log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t:\n", __FILE__, __LINE__);
		}
		if (biSize == 12){
			log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t:\n", __FILE__, __LINE__);
			for (i = 0; i < (int)biClrUsed; ++i){
				imageRead(log, &ctx, &palette->colors[i].b, 1, 1);
				imageRead(log, &ctx, &palette->colors[i].g, 1, 1);
				imageRead(log, &ctx, &palette->colors[i].r, 1, 1);
				palette->colors[i].unused = 0;
			}	
		} else {
			log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t:\n", __FILE__, __LINE__);
			for (i = 0; i < (int)biClrUsed; ++i){
				imageRead(log, &ctx, &palette->colors[i].b, 1, 1);
				imageRead(log, &ctx, &palette->colors[i].g, 1, 1);
				imageRead(log, &ctx, &palette->colors[i].r, 1, 1);
				imageRead(log,& ctx, &palette->colors[i].unused, 1, 1);
			}	
		}
		palette->ncolors = biClrUsed;
	}
	
	log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t:\n", __FILE__, __LINE__);

	// Read the surface pixels.  Note that the bmp image is upside down
	if (imageSeek(log, &ctx, fp_offset+bfOffBits, RW_SEEK_SET) < 0){
		log_error(log, "[%s:%d]\t: (imageLoadBMP)\t: File seek error\n", __FILE__, __LINE__);
		was_error = true;
		goto done;
	}
	
	log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t:\n", __FILE__, __LINE__);
	
	top = (Uint8 *)surface->pixels;
	end = (Uint8 *)surface->pixels+(surface->h*surface->pitch);
	switch (ExpandBMP){
		case 1:
			bmpPitch = (biWidth + 7) >> 3;
			pad  = (((bmpPitch)%4) ? (4-((bmpPitch)%4)) : 0);
			break;
		case 4:
			bmpPitch = (biWidth + 1) >> 1;
			pad  = (((bmpPitch)%4) ? (4-((bmpPitch)%4)) : 0);
			break;
		default:
			pad  = ((surface->pitch%4) ?
					(4-(surface->pitch%4)) : 0);
			break;
	}
	if (topDown){
		bits = top;
	} else {
		bits = end - surface->pitch;
	}
	while (bits >= top && bits < end){
		switch (ExpandBMP){
			case 1:
			case 4: {
					Uint8 pixel = 0;
					int   shift = (8-ExpandBMP);
					for (i=0; i<surface->w; ++i){
						if (i%(8/ExpandBMP) == 0){
							if (!imageRead(log, &ctx, &pixel, 1, 1)){
								log_error(log, "[%s:%d]\t: (imageLoadBMP)\t: Error reading from file\n", __FILE__, __LINE__);
								was_error = true;
								goto done;
							}
						}
						*(bits+i) = (pixel>>shift);
						pixel <<= ExpandBMP;
					}
				}
				break;

			default:
				if (imageRead(log, &ctx, bits, 1, surface->pitch) != surface->pitch){
					log_error(log, "[%s:%d]\t: (imageLoadBMP)\t: File read error\n", __FILE__, __LINE__);
					was_error = true;
					goto done;
				}
// Motorola 68k swap endian-ness
#ifdef USE_BIG_ENDIAN
			log_debug(log, "[%s:%d]\t: (imageLoadBMP)\t: Big-endian byte-swap\n", __FILE__, __LINE__);
			// Byte-swap the pixels if needed. Note that the 24bpp
			// case has already been taken care of above.
			switch(biBitCount) {
				case 15:
				case 16: {
					Uint16 *pix = (Uint16 *)bits;
					for(i = 0; i < surface->w; i++)
						pix[i] = imageSwap16(pix[i]);
					break;
				}

				case 32: {
					Uint32 *pix = (Uint32 *)bits;
					for(i = 0; i < surface->w; i++)
						pix[i] = imageSwap32(pix[i]);
					break;
				}
			}
#endif
			break;
		}
		// Skip padding bytes, ugh
		if (pad){
			Uint8 padbyte;
			for (i=0; i<pad; ++i){
				imageRead(log, &ctx, &padbyte, 1, 1);
			}
		}
		if (topDown){
			bits += surface->pitch;
		} else {
			bits -= surface->pitch;
		}
	}
done:
	if (was_error){
		if (ctx.fp){
			//imageSeek(log, &ctx, fp_offset, RW_SEEK_SET);
		}
		//if ( surface ) {
		//	SDL_FreeSurface(surface);
		//}
		//surface = NULL;
	} else {
		fclose(ctx.fp);	
	}
	if (freesrc && src){
		imageClose(log, &ctx);
	}
	//return(surface);
	return 1;
}