#include "SDL_config.h"
#include "SDL_video.h"
#include "SDL_endian.h"

/* Compression encodings for BMP files */
#ifndef BI_RGB
#define BI_RGB		0
#define BI_RLE8		1
#define BI_RLE4		2
#define BI_BITFIELDS	3
#endif

// __m68k__
static __inline__ Uint16 gfxSwap16(Uint16 x){
	__asm__("rorw #8,%0" : "=d" (x) :  "0" (x) : "cc");
	return x;
}

// __m68k__
static __inline__ Uint32 gfxSwap32(Uint32 x){
	__asm__("rorw #8,%0\n\tswap %0\n\trorw #8,%0" : "=d" (x) :  "0" (x) : "cc");
	return x;
}

static __inline__ Uint64 gfxSwap64(Uint64 x){
	Uint32 hi, lo;

	/* Separate into high and low 32-bit values and swap them */
	lo = SDL_static_cast(Uint32, x & 0xFFFFFFFF);
	x >>= 32;
	hi = SDL_static_cast(Uint32, x & 0xFFFFFFFF);
	x = SDL_Swap32(lo);
	x <<= 32;
	x |= SDL_Swap32(hi);
	return (x);
}

SDL_Surface * loadBMP(FILE *log, FILE *src, int freesrc){
	SDL_bool was_error;
	long fp_offset = 0;
	int bmpPitch;
	int i, pad;
	SDL_Surface *surface;
	Uint32 Rmask;
	Uint32 Gmask;
	Uint32 Bmask;
	SDL_Palette *palette;
	Uint8 *bits;
	Uint8 *top, *end;
	SDL_bool topDown;
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

	/* Make sure we are passed a valid data source */
	surface = NULL;
	was_error = SDL_FALSE;
	if ( src == NULL ) {
		was_error = SDL_TRUE;
		goto done;
	}

	/* Read in the BMP file header */
	fp_offset = SDL_RWtell(src);
	SDL_ClearError();
	if ( SDL_RWread(src, magic, 1, 2) != 2 ) {
		SDL_Error(SDL_EFREAD);
		was_error = SDL_TRUE;
		goto done;
	}
	if ( SDL_strncmp(magic, "BM", 2) != 0 ) {
		SDL_SetError("File is not a Windows BMP file");
		was_error = SDL_TRUE;
		goto done;
	}
	bfSize		= SDL_ReadLE32(src);
	bfReserved1	= SDL_ReadLE16(src);
	bfReserved2	= SDL_ReadLE16(src);
	bfOffBits	= SDL_ReadLE32(src);

	/* Read the Win32 BITMAPINFOHEADER */
	biSize		= SDL_ReadLE32(src);
	if ( biSize == 12 ) {
		biWidth		= (Uint32)SDL_ReadLE16(src);
		biHeight	= (Uint32)SDL_ReadLE16(src);
		biPlanes	= SDL_ReadLE16(src);
		biBitCount	= SDL_ReadLE16(src);
		biCompression	= BI_RGB;
		biSizeImage	= 0;
		biXPelsPerMeter	= 0;
		biYPelsPerMeter	= 0;
		biClrUsed	= 0;
		biClrImportant	= 0;
	} else {
		biWidth		= SDL_ReadLE32(src);
		biHeight	= SDL_ReadLE32(src);
		biPlanes	= SDL_ReadLE16(src);
		biBitCount	= SDL_ReadLE16(src);
		biCompression	= SDL_ReadLE32(src);
		biSizeImage	= SDL_ReadLE32(src);
		biXPelsPerMeter	= SDL_ReadLE32(src);
		biYPelsPerMeter	= SDL_ReadLE32(src);
		biClrUsed	= SDL_ReadLE32(src);
		biClrImportant	= SDL_ReadLE32(src);
	}

	/* stop some compiler warnings. */
	(void) bfSize;
	(void) bfReserved1;
	(void) bfReserved2;
	(void) biPlanes;
	(void) biSizeImage;
	(void) biXPelsPerMeter;
	(void) biYPelsPerMeter;
	(void) biClrImportant;

	if (biHeight < 0) {
		topDown = SDL_TRUE;
		biHeight = -biHeight;
	} else {
		topDown = SDL_FALSE;
	}

	/* Check for read error */
	if ( SDL_strcmp(SDL_GetError(), "") != 0 ) {
		was_error = SDL_TRUE;
		goto done;
	}

	/* Expand 1 and 4 bit bitmaps to 8 bits per pixel */
	switch (biBitCount) {
		case 1:
		case 4:
			ExpandBMP = biBitCount;
			biBitCount = 8;
			break;
		default:
			ExpandBMP = 0;
			break;
	}

	/* We don't support any BMP compression right now */
	Rmask = Gmask = Bmask = 0;
	switch (biCompression) {
		case BI_RGB:
			/* If there are no masks, use the defaults */
			if ( bfOffBits == (14+biSize) ) {
				/* Default values for the BMP format */
				switch (biBitCount) {
					case 15:
					case 16:
						Rmask = 0x7C00;
						Gmask = 0x03E0;
						Bmask = 0x001F;
						break;
					case 24:
// On 68k swap endian-ness
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
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
			/* Fall through -- read the RGB masks */

		case BI_BITFIELDS:
			switch (biBitCount) {
				case 15:
				case 16:
				case 32:
					Rmask = SDL_ReadLE32(src);
					Gmask = SDL_ReadLE32(src);
					Bmask = SDL_ReadLE32(src);
					break;
				default:
					break;
			}
			break;
		default:
			SDL_SetError("Compressed BMP files not supported");
			was_error = SDL_TRUE;
			goto done;
	}

	/* Create a compatible surface, note that the colors are RGB ordered */
	surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
			biWidth, biHeight, biBitCount, Rmask, Gmask, Bmask, 0);
	if ( surface == NULL ) {
		was_error = SDL_TRUE;
		goto done;
	}

	/* Load the palette, if any */
	palette = (surface->format)->palette;
	if ( palette ) {
		if ( biClrUsed == 0 ) {
			biClrUsed = 1 << biBitCount;
		}
		if ( biSize == 12 ) {
			for ( i = 0; i < (int)biClrUsed; ++i ) {
				SDL_RWread(src, &palette->colors[i].b, 1, 1);
				SDL_RWread(src, &palette->colors[i].g, 1, 1);
				SDL_RWread(src, &palette->colors[i].r, 1, 1);
				palette->colors[i].unused = 0;
			}	
		} else {
			for ( i = 0; i < (int)biClrUsed; ++i ) {
				SDL_RWread(src, &palette->colors[i].b, 1, 1);
				SDL_RWread(src, &palette->colors[i].g, 1, 1);
				SDL_RWread(src, &palette->colors[i].r, 1, 1);
				SDL_RWread(src, &palette->colors[i].unused, 1, 1);
			}	
		}
		palette->ncolors = biClrUsed;
	}

	/* Read the surface pixels.  Note that the bmp image is upside down */
	if ( SDL_RWseek(src, fp_offset+bfOffBits, RW_SEEK_SET) < 0 ) {
		SDL_Error(SDL_EFSEEK);
		was_error = SDL_TRUE;
		goto done;
	}
	top = (Uint8 *)surface->pixels;
	end = (Uint8 *)surface->pixels+(surface->h*surface->pitch);
	switch (ExpandBMP) {
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
	if ( topDown ) {
		bits = top;
	} else {
		bits = end - surface->pitch;
	}
	while ( bits >= top && bits < end ) {
		switch (ExpandBMP) {
			case 1:
			case 4: {
			Uint8 pixel = 0;
			int   shift = (8-ExpandBMP);
			for ( i=0; i<surface->w; ++i ) {
				if ( i%(8/ExpandBMP) == 0 ) {
					if ( !SDL_RWread(src, &pixel, 1, 1) ) {
						SDL_SetError(
					"Error reading from BMP");
						was_error = SDL_TRUE;
						goto done;
					}
				}
				*(bits+i) = (pixel>>shift);
				pixel <<= ExpandBMP;
			} }
			break;

			default:
			if ( SDL_RWread(src, bits, 1, surface->pitch)
							 != surface->pitch ) {
				SDL_Error(SDL_EFREAD);
				was_error = SDL_TRUE;
				goto done;
			}
// Motorola 68k swap endian-ness
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			/* Byte-swap the pixels if needed. Note that the 24bpp
			   case has already been taken care of above. */
			switch(biBitCount) {
				case 15:
				case 16: {
				        Uint16 *pix = (Uint16 *)bits;
					for(i = 0; i < surface->w; i++)
					        pix[i] = SDL_Swap16(pix[i]);
					break;
				}

				case 32: {
				        Uint32 *pix = (Uint32 *)bits;
					for(i = 0; i < surface->w; i++)
					        pix[i] = SDL_Swap32(pix[i]);
					break;
				}
			}
#endif
			break;
		}
		/* Skip padding bytes, ugh */
		if ( pad ) {
			Uint8 padbyte;
			for ( i=0; i<pad; ++i ) {
				SDL_RWread(src, &padbyte, 1, 1);
			}
		}
		if ( topDown ) {
			bits += surface->pitch;
		} else {
			bits -= surface->pitch;
		}
	}
done:
	if ( was_error ) {
		if ( src ) {
			SDL_RWseek(src, fp_offset, RW_SEEK_SET);
		}
		if ( surface ) {
			SDL_FreeSurface(surface);
		}
		surface = NULL;
	}
	if ( freesrc && src ) {
		SDL_RWclose(src);
	}
	return(surface);
}