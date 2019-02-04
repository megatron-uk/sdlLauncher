/* Virtual screen (X)IMG-viewer (C) 1994 by Eero Tamminen. */
/* No palette conversion. */
/* Using Sozobon C 2.0x (short ints) you can compile this with: */
/* cc -e -O -o img.prg img.c xaesfast.a xvdifast.a */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef __AHCC__
# include <tos.h>
# include <gemf.h>
#  define OBJ_INFO	long
#  define lalloc	malloc
#  define max(x,y)       (((x)>(y))?(x):(y))
#  define min(x,y)       (((x)<(y))?(x):(y))
# else
# include <osbind.h>
# include <macros.h>
# ifdef __GNUC__
#  include <memory.h>
#  define __TCC_COMPAT__
#  include <aesbind.h>
#  include <vdibind.h>
#  define OBJ_INFO	TEDINFO *
#  define lalloc	malloc
# endif
#endif

#ifndef FALSE
#define FALSE 0
#define TRUE  !FALSE
#endif

/* ------------------------------------------- */
/* error codes */
#define ERR_HEADER      1
#define ERR_ALLOC       2
#define ERR_FILE        3
#define ERR_DEPACK      4
#define ERR_COLOR       5
#define ERR_WIDTH	6

/* how many bytes one scanline can be */
#define MAX_WIDTH	2048
#define MAX_PLANES	8
static char buf[MAX_PLANES * MAX_WIDTH / 8];

/* some vdi arrays and global variables (work-area) etc. */
static short vdi_handle, work_in[11], work_out[57],
      screen_planes, screen_colors, screen_w, screen_h,
      normal_palette[256*3], wg_x, wg_y, wg_w, wg_h;

/* VDI <-> Device palette order conversion matrixes:
 */

/* Four-plane vdi-device */
static const short vdi2dev4[] = {0,15,1,2,4,6,3,5,7,8,9,10,12,14,11,13};
/* Two-plane vdi-device */
static const short vdi2dev2[] = {0,3,1,2};

/* ------------------------------------------- */
#define XIMG      0x58494D47

typedef struct IMG_HEADER
{                 /* Header of GEM Image Files   */
  short version;  /* Img file format version (1) */
  short length;   /* Header length in words  (8) */
  short planes;   /* Number of bit-planes    (1) */
  short pat_len;  /* length of Patterns      (2) */
  short pix_w;    /* Pixel width in 1/1000 mmm  (372)    */
  short pix_h;    /* Pixel height in 1/1000 mmm (372)    */
  short img_w;    /* Pixels per line (=(x+7)/8 Bytes)    */
  short img_h;    /* Total number of lines               */
  long  magic;    /* Contains "XIMG" if standard color   */
  short paltype;  /* palette type (0 = RGB (short each)) */
  short *palette; /* palette etc.                        */
  char *addr;     /* Address for the depacked bit-planes */
} IMG_header;

/* ------------------------------------------- */
/* IMG_RSC (for phase announcement) definition */

#define DIALOG    0 /* form/dialog */
#define NOTICE    1 /* BOXTEXT in tree DIALOG_TREE */

static char notice[64] = "";

static TEDINFO rs_tdi = {notice, "", "", 3, 6, 0, 0x1180, 0x0, -1, 20, 1};

static OBJECT rs_ob[] =
{{-1, 1, 1, G_BOX, NONE, OUTLINED|SHADOWED, (OBJ_INFO)0xFE113CL, 2,1, 23,3},
 { 0,-1,-1, G_BOXTEXT, LASTOB, OUTLINED, (OBJ_INFO)&rs_tdi, 2,1, 19,1}};

/* number of objects to fix */
#define NUM_OBS     2

/* ------------------------------------------- */
/* Function prototypes. */
static char    *pathcut (char *path);
static void    fix_rsc(void);
static void    open_work(void);
static void    close_work(void);
static void    show_notice(int flag);
static void    show_error(int error);
static char   *file_name(char *path);
static int     depack_img(char *name, IMG_header *info);
static void    show_img(IMG_header *info);
static void    set_colors(short *palette, int colors);
static int     transform_img(MFDB *image, short *palette);
static void    dither(unsigned short *addr, long size, unsigned short width, unsigned short planes, short *palette);
static int     convert(MFDB *image, long size);

/* ------------------------------------------- */
int main(int argc, char *argv[])
{
  IMG_header img;
  static char options[96], filename[33] = "\0";
  int error = 1, i = 1, button = FALSE, type;
  char *path;

  img.addr = NULL;
  path = buf;
  appl_init();
  open_work();
  fix_rsc();
  graf_mouse(2, 0);		/* busybee */

  while(i < argc || (argc < 2 && (strcpy(path, "*.IMG"),
    fsel_exinput(path, filename, &button, "(X)IMG-Viewer v1.4")) && button))
  {
    if(button)
      strcat(pathcut(path), filename);
    else
      path = argv[i];

    type = strlen(path) - 4;
    if(type >= 0 && (strcmp(&path[type], ".img") == 0 ||
       strcmp(&path[type], ".IMG") == 0))
    {
      sprintf(notice, " Loading %s... ", file_name(path));
      show_notice(TRUE);

      error = depack_img(path, &img);
      show_notice(FALSE);

      if(error)
      {
	show_error(error);
	i = argc;		/* exit */
      }
      else
      {
        /* save screen below menu */
        form_dial(FMD_START, wg_x, wg_y, wg_w, wg_h, wg_x, wg_y, wg_w, wg_h);
        /* disable mouse */
        graf_mouse(M_OFF, 0);
	/* exit at right button, next pic at left button */
	show_img(&img);
        /* enable mouse */
        graf_mouse(M_ON, 0);

	/* not through fileselector */
	if(!button)
	{
          sprintf(options, "[1][%s |(%d/%d)|Size: %d x %d |Bitplanes: %d |][ Prev |Quit|Next]",
                  file_name(path), i, argc - 1, img.img_w, img.img_h, img.planes);
          switch(form_alert(2, options))
          {
            case 1:
              i = max(1, i - 1);
              break;
            case 2:
              i = argc;
              break;
            case 3:
              i = min(argc - 1, i + 1);
              break;
          }
        }
        /* redraw screen below menu */
        form_dial(FMD_FINISH, wg_x, wg_y, wg_w, wg_h, wg_x, wg_y, wg_w, wg_h);
      }
    }
  }
  graf_mouse(0, 0);		/* normal pointer */
  free(img.palette);
  free(img.addr);

  /* restore screen and close virtual... */
  close_work();
  appl_exit();
  return error;
}

/* cut a filename or -mask away from the filepath */
static char *pathcut (char *path)
{
  int i = 0;

  while(path[i ++] != '\0');            /* search end of string      */
  while(path[-- i] != '\\' && i >= 0);  /* search path separator     */
  path[++ i] = '\0';                    /* cut path from name / mask */

  return path;
}

/* fix resource co-ordinates etc. */
static void fix_rsc(void)
{
  int i ;
  for(i = 0; i < NUM_OBS; rsrc_obfix(rs_ob, i++));
}

/* open screen, set some vars, hide mouse and disable other screen update */
static void open_work(void)
{
  short i, idx;

  /* open virtual screen workstation (screen) */
  for(i = 0; i < 10; work_in[i ++] = 1);
  work_in[10] = 2;
  vdi_handle = graf_handle(&i, &i, &i, &i);
  v_opnvwk(work_in, &vdi_handle, work_out);
  screen_colors = work_out[13];
  screen_w = work_out[0];
  screen_h = work_out[1];

  /* get the number of bitplanes on screen */
  vq_extnd(vdi_handle, 1, work_out);
  screen_planes = work_out[4];

  /* a resolution with a palette */
  if(screen_planes > 0 && screen_planes < 9)
    /* get current color palette */
    for(i = 0; i < screen_colors; i ++)
    {
      /* device->vdi->device palette order */
      switch(screen_planes)
      {
	case 2:
	  idx = vdi2dev2[i];
	  break;
	case 4:
	  idx = vdi2dev4[i];
	  break;
	default:
	  idx = i;
      }
      vq_color(vdi_handle, i, 0, normal_palette + idx * 3);
    }

  /* Desktop work area. */
  wind_get(0, WF_WORKXYWH, &wg_x, &wg_y, &wg_w, &wg_h);

  /* suspend other screen activity */
  wind_update(BEG_UPDATE);
}

/* enable updates, redraw screen, show mouse and close screen */
static void close_work(void)
{
  /* enable other screen activities */
  wind_update(END_UPDATE);
  v_clsvwk(vdi_handle);
}

/* show and remove a notice dialog */
static void show_notice(int flag)
{
  short extent[8];
  static short x, y, w, h;

  if(flag)
  {
    vqt_extent (vdi_handle, notice, extent);
    rs_ob[DIALOG].ob_width = rs_ob[NOTICE].ob_x * 2 + extent[2];
    rs_ob[NOTICE].ob_width = extent[2];
    form_center(&rs_ob[DIALOG], &x, &y, &w, &h);
    form_dial(FMD_START, x, y, w, h, x, y, w, h);
    objc_draw(&rs_ob[DIALOG], 0, 7, x, y, w, h);
  }
  else
  {
    form_dial(FMD_FINISH, x, y, w, h, x, y, w, h);
  }
}

/* show error alert */
static void show_error(int error)
{
  char message[64];

  strcpy(message, "[1][");

  switch(error)
  {
    case ERR_FILE:
      strcat(message, "File error.");
      break;
    case ERR_HEADER:
      strcat(message, "Invalid IMG-header.");
      break;
    case ERR_WIDTH:
      strcat(message, "Image too wide.");
    case ERR_ALLOC:
      strcat(message, "Not enough memory.");
      break;
    case ERR_DEPACK:
      strcat(message, "Depacking error.");
      break;
    case ERR_COLOR:
      strcat(message, "Strange palette.");
      break;
  }
  strcat(message, " ][  OK  ]");

  form_alert(1, message);
}

/* get whole path, return the address of the plain name */
static char *file_name(char *path)
{
  char *name;

  name = path + strlen(path);
  while(*name != '\\' && *name != '/' && name >= path)
    name --;

  return (++name);
}

/* Loads & depacks IMG (0 if succeded, else error). */
/* Bitplanes are one after another in address IMG_HEADER.addr. */
static int depack_img(char *name, IMG_header *pic)
{
  int   a, b, line, plane, width, word_aligned, opcode, patt_len, pal_size,
	byte_repeat, patt_repeat, scan_repeat, error = FALSE;
  char  pattern[16], *address, *to, *endline;
  long  size;
  FILE *fp;

  if((fp = fopen(name, "rb")) == NULL)
    return ERR_FILE;

  /* read header info (bw & ximg) into image structure */
  fread((char *)&(pic->version), 2, 8 + 3, fp);

  /* only 2-256 color imgs */
  if(pic->planes < 1 || pic->planes > MAX_PLANES)
  {
    error = ERR_COLOR;
    goto end_depack;
  }
  /* not too wide for the buffer */
  if(pic->img_w > MAX_WIDTH)
  {
    error = ERR_WIDTH;
    goto end_depack;
  }

  /* if XIMG, read info */
  if(pic->magic == XIMG && pic->paltype == 0)
  {
    pal_size = (1 << pic->planes) * 3 * 2;
    if((pic->palette = (short *)malloc(pal_size)))
    {
      fread((char *)pic->palette, 1, pal_size, fp);
    }
  }
  else
  {
    pic->palette = NULL;
  }

  /* width in bytes word aliged */
  word_aligned = (pic->img_w + 15) >> 4;
  word_aligned <<= 1;

  /* width byte aligned */
  width = (pic->img_w + 7) >> 3;

  /* allocate memory for the picture */
  free(pic->addr);
  size = (long) word_aligned * pic->img_h * pic->planes;

  /* check for header validity & malloc long... */
  if (pic->length > 7 && pic->img_w > 0 && pic->img_h > 0)
    pic->addr = (char *) lalloc(size);
  else
  {
    error = ERR_HEADER;
    goto end_depack;
  }

  /* if allocation succeded, proceed with depacking */
  if(pic->addr == NULL)
  {
    error = ERR_ALLOC;
    goto end_depack;
  }
  else
  {
    patt_len = pic->pat_len;
    endline = buf + width;

    /* jump over the header and possible (XIMG) info */
    fseek(fp, (long) pic->length * 2, SEEK_SET);

    /* depack whole img */
    for(line = 0; line < pic->img_h; line += scan_repeat)
    {
      scan_repeat = 1;

      /* depack one scan line */
      for(plane = 0; plane < pic->planes; plane ++)
      {
	to = buf;

	/* depack one line in one bitplane */
	do
	{
	  opcode = fgetc(fp);
	  switch(opcode)
	  {
	    /* pattern or scan repeat */
	    case 0:

	      patt_repeat = fgetc(fp);
	      /* repeat a pattern */
	      if(patt_repeat)
	      {
		/* read pattern */
		for(b = 0; b < patt_len; b ++)
		  pattern[b] = fgetc(fp);

		/* copy pattern */
		for(a = 0; a < patt_repeat; a ++)
		{
		  /* in case it's odd amount... */
		  for(b = 0; b < patt_len; b ++)
		    *(to ++) = pattern[b];
		}
	      }

	      /* repeat a line */
	      else
	      {
		if(fgetc(fp) == 0xFF)
		  scan_repeat = fgetc(fp);
		else
		{
		  error = ERR_DEPACK;
		  goto end_depack;
		}
	      }
	      break;

	    /* repeat 'as is' */
	    case 0x80:
	      byte_repeat = fgetc(fp);
	      for(; byte_repeat > 0; byte_repeat --)
		*(to ++) = fgetc(fp);
	      break;

	    /* repeat black or white */
	    default:
	      byte_repeat = opcode & 0x7F;
	      if(opcode & 0x80)
		opcode = 0xFF;
	      else
		opcode = 0x00;
	      for(; byte_repeat > 0; byte_repeat --)
		*(to ++) = opcode;
	  }
	}
	while(to < endline);

	if(to == endline)
	{
	  /* ensure that lines aren't repeated past the end of the img */
	  if(line + scan_repeat > pic->img_h)
	    scan_repeat = pic->img_h - line;

	  /* calculate address of a current line in a current bitplane */
	  address = pic->addr  +
		  (long) line  * word_aligned +
		  (long) plane * word_aligned * pic->img_h;

	  /* copy line to image buffer */
	  for(a = 0; a < scan_repeat; a ++)
	  {
	    memcpy(address, buf, width);
	    address += word_aligned;
	  }
	}
	else
	{
	  error = ERR_DEPACK;
	  goto end_depack;
	}
      }
    }
  }

  end_depack:
  fclose(fp);
  return error;
}

/* show img on screen */
static void show_img(IMG_header *pic)
{
  short buttons = 0, key, x1 = 0, y1 = 0, x2 = 0, y2 = 0,
	mx, my, xx, yy, cen_x, cen_y, redraw = TRUE, pxyarray[8], colors;
  MFDB image, screen;

  /* Screen and Image VDI Memory Form Definitions. */
  screen.fd_addr   = 0;
  image.fd_addr    = pic->addr;                 /* address      */
  image.fd_w       = pic->img_w;                /* width        */
  image.fd_wdwidth = (pic->img_w + 15) >> 4;    /* (words)      */
  image.fd_h       = pic->img_h;                /* height       */
  image.fd_stand   = 0;                         /* raster format = device */
  image.fd_nplanes = pic->planes;               /* bitplanes    */

  /* pic size on screen */
  xx = min(wg_w, pic->img_w);
  yy = min(wg_h, pic->img_h);

  /* for centering pic on screen */
  cen_x = ((wg_w - xx) >> 1) + wg_x;
  cen_y = ((wg_h - yy) >> 1) + wg_y;

  /* convert image to the current screen format if necessary */
  if(!(transform_img(&image, pic->palette)))
    return;

  /* if a color image with a RGB palette and color resolution */
  colors = (pic->palette != NULL && screen_planes > 1 && screen_planes < 9);
  if(colors)
    set_colors(pic->palette, min(screen_colors, pic->planes));

  do
  {
    /* get mouse position */
    graf_mkstate(&mx, &my, &buttons, &key);

    /* calculate new image place */
    x1 = (long) mx * (image.fd_w - xx) / screen_w;
    y1 = (long) my * (image.fd_h - yy) / screen_h;

    /* fit co-ordinates onto screen */
    x1 = min(x1, pic->img_w - xx);
    y1 = min(y1, pic->img_h - yy);
    x1 = max(0, x1);
    y1 = max(0, y1);

    /* draw image if necessary */
    if (redraw || x1 != x2 || y1 != y2)
    {
      /* put values into the co-ordinate array */
      pxyarray[0] = x1;
      pxyarray[1] = y1;
      pxyarray[2] = x1 + xx - 1;
      pxyarray[3] = y1 + yy - 1;
      pxyarray[4] = cen_x;
      pxyarray[5] = cen_y;
      pxyarray[6] = cen_x + xx - 1;
      pxyarray[7] = cen_y + yy - 1;

      /* throw onto screen */
      vro_cpyfm(vdi_handle, S_ONLY, pxyarray, &image, &screen);

      x2 = x1; y2 = y1;
      redraw = FALSE;
    }
  /* exit with right button */
  }
  while(!buttons);

  if(colors)
    /* restore normal palette (already converted) */
    set_colors(normal_palette, screen_planes);

  /* if image was converted to device format, free allocated memory */
  if(image.fd_addr != pic->addr)
    free(image.fd_addr);

  /* assure mouse button(s) off */
  while(buttons)
    graf_mkstate(&mx, &my, &buttons, &key);
}

static void set_colors(short *palette, int colors)
{
  int i, idx;

  /* set color palette */
  colors = 1 << colors;
  for(i = 0; i < colors; i++)
  {
    switch(screen_planes)
    {
      case 2:
	idx = vdi2dev2[i];
	break;
      case 4:
	idx = vdi2dev4[i];
	break;
      default:
	idx = i;
    }
    vs_color(vdi_handle, i, palette + idx * 3);
  }
}

/* return FALSE if transformation was unsuccesful */
static int transform_img(MFDB *image, short *palette)
{
  int success;
  long size;

  size = (long)(image->fd_wdwidth * image->fd_h);

  if(screen_planes == 1)
  {
    if(image->fd_nplanes > 1)
    {
      sprintf(notice, " Dithering %d bitplanes... ", image->fd_nplanes);
      show_notice(TRUE);
      dither((unsigned short *)image->fd_addr, size,
      	(unsigned short)image->fd_wdwidth,
	(unsigned short)image->fd_nplanes, palette);
      show_notice(FALSE);
      image->fd_nplanes = 1;
    }
    return TRUE;

  }
  else
  {
    sprintf(notice, " Converting %d bitplanes... ", image->fd_nplanes);
    show_notice(TRUE);

    /* Use vr_trfm(), which needs quite a lot memory. */
    success = convert(image, size);
    show_notice(FALSE);
    if(success)
      return TRUE;
    else
      show_error(ERR_ALLOC);
  }
  return FALSE;
}

/* FS-dither bitplanes into an bw one 'on place' */
static void dither(unsigned short *addr, long size, unsigned short width,
		   unsigned short planes, short *palette)
{
  register short cur = 0;
  register unsigned short i, bit, color, *plane;
  unsigned short result, col_max = 0, col_min = 255,
    first_plane[MAX_PLANES], *last, *idx, *end, *intensity;
  long offset;
  short count;

  /* cur = current error or pixel value */
  short belowerr = 0;		/* error for pixel below cur */
  short bpreverr = 0;		/* error for below/prev col */
  short bnexterr;		/* error for below/next col */
  short delta;
  short *errorptr;		/* => fserrors at column before current */
  static short fserrors[MAX_WIDTH + 2];

  end = addr + size;		/* destination pointer */
  errorptr = fserrors;
  intensity = (unsigned short *)buf;
  last = first_plane + planes - 1;
  planes = (1 << planes);

  /* scan palette */
  if(planes <= 256)
  {
    for(i = 0; i < planes; i ++)
    {
      intensity[i] = 0;
      /* add register values together to get the grey level */
      for(bit = 0; bit < 3; bit ++)
      {
	intensity[i] += *(palette + 3 * i + bit);
      }
      col_min = min(col_min, intensity[i]);
      col_max = max(col_max, intensity[i]);
    }
  }
  /* scale mapped grayscale to range 0-255 */
  for(i = 0; i < planes; i++)
    intensity[i] = ((long)intensity[i] - col_min) * 255 / (col_max - col_min);

  /* remake first bitplane */
  planes >>= 1;
  count = 0;
  for(idx = addr; idx < end; idx++)
  {
    if(--count <= 0)
    {
      count = width;
      /* Post-loop cleanup: we must unload the final error value into the
       * final fserrors entry.  Note we need not unload belowerr because
       * it is for the dummy column before or after the actual array.
       */
      errorptr[0] = bpreverr; /* unload prev err into array */
      /* => entry before first column */
      errorptr = fserrors;
      /* no error propagated to first pixel from left and to row below yet */
      cur = belowerr = bpreverr = 0;
    }

    /* go through all bitplanes */
    offset = 0;
    plane = first_plane;
    for(i = planes; i > 0; i >>= 1)
    {
      /* get one word from a bitplane */
      *(plane++) = *(idx + offset);
      offset += size;
    }
    result = 0;

    /* go through one word on all bitplanes */
    for(bit = 0x8000; bit > 0; bit >>= 1)
    {
      color = 0;
      plane = last;

      /* planar->chunky */
      for(i = planes; i > 0; i >>= 1)
        if((*(plane--) & bit))
          color |= i;

      /* cur holds the error propagated from the previous pixel on the
       * current line.  Add the error propagated from the previous line
       * to form the complete error correction term for this pixel, and
       * round the error term (which is expressed * 16) to an integer.
       * RIGHT_SHIFT rounds towards minus infinity, so adding 8 is correct
       * for either sign of the error value.
       * Note: errorptr points to *previous* column's array entry.
       */
      cur = (cur + errorptr[1] + 8) >> 4;

      /* form pixel value + error, and range-limit to col_min...col_max. */
      cur += intensity[color];
      if(cur < 0)
        cur = 0;
      else
        if(cur > 255)
          cur = 255;

      /* set bit in resulting bitplane if needed */
      /* cur <= the actual representation error at this pixel */
      if(cur < 127)
        result |= bit;
      else
        cur -= 255;

      /* Compute error fractions to be propagated to adjacent pixels.
       * Add these into the running sums, and simultaneously shift the
       * next-line error sums left by 1 column.
       */
      bnexterr = cur;
      delta = cur << 1;
      cur += delta;			/* form error * 3 */
      *errorptr = bpreverr + cur;
      cur += delta;			/* form error * 5 */
      bpreverr = belowerr + cur;
      belowerr = bnexterr;
      cur += delta;			/* form error * 7 */
      errorptr++;		/* advance errorptr to current column */
    }
    *idx = result;		/* output the word of dithered pixels */
  }
}

/* convert image using the standard vr_trfm() */
static int convert(MFDB *image, long size)
{
  int i, plane, planes, line_size, width;
  char *line_addr, *buf_addr, *new_addr;
  char *image_addr, *screen_addr;
  MFDB  screen, tmp;

  /* convert size from words to bytes */
  size <<= 1;

  /* memory for the device raster */
  if((new_addr = (char *) lalloc(size * screen_planes)) == NULL)
    return FALSE;

  /* initialize MFDBs */
  tmp = *image;
  tmp.fd_h = 1;
  tmp.fd_nplanes = screen_planes;
  tmp.fd_addr = buf;
  tmp.fd_stand = 1;             /* standard format */
  screen = tmp;
  screen_addr = new_addr;
  screen.fd_stand = 0;          /* device format */
  image_addr = (char *)image->fd_addr;

  /* initialize some variables and zero temp. line buffer */
  planes = min(image->fd_nplanes, screen_planes);
  width = (tmp.fd_wdwidth << 1);
  line_size = screen_planes * width;
  for(i = 0; i < line_size; i ++)
    buf[i] = 0;

  /* convert image */
  for(i = 0; i < image->fd_h; i ++)
  {
    line_addr = image_addr + width * i;
    buf_addr = buf;
    if(planes > 1)
    {
      /* cut / pad color planes into temp buf */
      for(plane = 0; plane < planes; plane ++)
      {
        memcpy(buf_addr, line_addr, width);
        line_addr += size;
        buf_addr += width;
      }
    }
    else
    {
      /* fill temp line bitplanes with a b&w line */
      for(plane = 0; plane < screen_planes; plane ++)
      {
        memcpy(buf_addr, line_addr, width);
        buf_addr += width;
      }
    }
    /* convert image line in temp into current device raster format */
    screen.fd_addr = screen_addr;
    vr_trnfm(vdi_handle, &tmp, &screen);
    screen_addr += line_size;
  }
  /* change image description */
  image->fd_stand = 0;          /* device format */
  image->fd_addr = new_addr;
  image->fd_nplanes = screen_planes;
  return TRUE;
}
