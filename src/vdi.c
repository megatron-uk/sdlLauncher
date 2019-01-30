#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <mint/osbind.h>
#include <gem.h>

#include "logging.h"

static struct {
	int w, h;
	int multitos;
} screen;

static short vdi_handle, win_handle;

/* how many bits for fixed point calculations /
 * how many steps for acceleration/decleration.
 */
#define DIGITS		5

#define SPEED_INIT	4			/* in pixels */
#define SPEED_MIN	(1 << (DIGITS-3))	/* in fixed point */
#define SPEED_DECR	2			/* in fixed point */

/* TEXT style bits */
#define TEXT_BOLD	1
#define TEXT_LIGHT	2
#define TEXT_ITALIC	4
#define TEXT_UNDERLINE	8
#define TEXT_OUTLINE	16

/* text size is based on screen width, in pt */
#define SIZE_NAME	(screen.w/20)
#define SIZE_HELP	(screen.w/32)
#define SIZE_SCORE	(screen.w/32)
#define SIZE_GAMEOVER	(screen.w/10)

/* settings for draw type widths */
#define LINE_WIDTH	1

/* draw filled rectangle */
static void draw_fbox(int x1, int y1, int x2, int y2)
{
	short pxyarray[4];
	pxyarray[0] = x1;
	pxyarray[1] = y1;
	pxyarray[2] = x2;
	pxyarray[3] = y2;
	vr_recfl(vdi_handle, pxyarray);
}

static void clip_enable(int enabled)
{
	short pxyarray[4];
	pxyarray[0] = 0;
	pxyarray[1] = 0;
	pxyarray[2] = screen.w;
	pxyarray[3] = screen.h;
	vs_clip(vdi_handle, enabled, pxyarray);
}

static void work_clear(void){
	short pxyarray[4];
	short attrib[5];

	/* this makes it bg colored */
	vqf_attributes(vdi_handle, attrib);
	vswr_mode(vdi_handle, MD_REPLACE);
	vsf_interior(vdi_handle, 0);

	pxyarray[0] = 0;
	pxyarray[1] = 0;
	pxyarray[2] = screen.w;
	pxyarray[3] = screen.h;
	vr_recfl(vdi_handle, pxyarray);

	/* restore interior fill style */
	vsf_interior(vdi_handle, attrib[0]);
	vswr_mode(vdi_handle, attrib[3]);
}

static void work_open(void){
	static short work_in[12], work_out[57];
	short app_id;
	int i;

	app_id = appl_init();
	screen.multitos = (aes_global[1] == -1);

	/* open virtual screen workstation (screen) */
	for(i = 0; i < 10; work_in[i++] = 1);
	work_in[10] = 2;
	v_opnvwk(work_in, &vdi_handle, work_out);

	/* screen info */
	screen.w = work_out[0];
	screen.h = work_out[1];

	if (screen.multitos) {
		short x, y, w, h;
		/* create window, otherwise cannot get all events */
		x = 0; y = 0; w = screen.w; h = screen.h;
		win_handle = wind_create(0, x, y, w, h);
		if (win_handle > 0) {
			wind_open(win_handle, x, y, w, h);
		}
	} else {
		/* reserve screen area (in multitasking AES = a no-op),
		 * is really needed only for ACCs as on non-multitos AES
		 * only those can come on top of other applications.
		 */
		form_dial(FMD_START, screen.w>>1, screen.h>>1, 0, 0, 0, 0, screen.w, screen.h);
	}

	/* hide mouse, stop updates */
	graf_mouse(M_OFF, NULL);
	wind_update(BEG_UPDATE);

	/* enable clipping & clear screen */
	clip_enable(1);
	work_clear();
}

static void work_close(void){
	clip_enable(0);

	wind_update(END_UPDATE);
	graf_mouse(M_ON, NULL);

	/* restore screen area (in multitasking AES = cause WM_REDRAW) */
	form_dial(FMD_FINISH, screen.w>>1, screen.h>>1, 0, 0, 0, 0, screen.w, screen.h);

	/* VDI & AES close */
	v_clsvwk(vdi_handle);
	appl_exit();
}

static void style_bg(void)
{
	short d;
	vsf_interior(vdi_handle, 0);
	vsf_perimeter(vdi_handle, 0);
	vsl_width(vdi_handle, LINE_WIDTH);
	vswr_mode(vdi_handle, MD_REPLACE);
	/* text alignment: horizontal = center, vertical = half line */
	vst_alignment(vdi_handle, 1, 1, &d, &d);
}

/* draw horizontal line, with dashes (restores solid lines) */
static void draw_hline(int x1, int x2, int y)
{
	short pxyarray[4];

	pxyarray[0] = x1;
	pxyarray[1] = y;
	pxyarray[2] = x2;
	pxyarray[3] = y;
	v_pline(vdi_handle, 2, pxyarray);
}

/* draw vertical line, with dashes (restores solid lines) */
static void draw_vline(int y1, int y2, int x)
{
	short pxyarray[4];

	pxyarray[0] = x;
	pxyarray[1] = y1;
	pxyarray[2] = x;
	pxyarray[3] = y2;
	v_pline(vdi_handle, 2, pxyarray);
}

// Draw a rectangular box and fill it
static void draw_box(int x1, int y1, int x2, int y2, int fill){
	
	short pxyarray[4];
	
	pxyarray[0] = x1;
	pxyarray[1] = y1;
	pxyarray[2] = x2;
	pxyarray[3] = y2;
	if (fill){
		vsf_interior(vdi_handle, FIS_SOLID);
	} else {
		vsf_interior(vdi_handle, FIS_HOLLOW);
	}
	vsf_perimeter(vdi_handle, 1);
	v_bar(vdi_handle, pxyarray);
}

static void draw_hline_dashed(int x1, int x2, int y)
{
	short attrib[6];
	vql_attributes(vdi_handle, attrib);

	vsl_type(vdi_handle, LONGDASH);/* sometimes also named as LDASHED */
	draw_hline(x1, x2, y);
	/* restore line settings */
	vsl_type(vdi_handle, attrib[0]);
}

static void blankScreen(FILE *log){

	short pxyarray[4];	// Coordinates of window
	int c;	
	
	pxyarray[0] = 0;
	pxyarray[1] = 0;
	pxyarray[2] = screen.w;
	pxyarray[3] = screen.h;
	
	log_info(log, "[blankScreen]\t: %dx%d\n", pxyarray[2], pxyarray[3]);
	vsf_interior(vdi_handle, FIS_SOLID);
	vswr_mode(vdi_handle, MD_REPLACE);
	vr_recfl(vdi_handle, pxyarray);
	
}

#define MAX_PENS 16

// Save the values of the original pen colours
int savePens(FILE *log, short old_rgb[MAX_PENS][3]){
	short c;
	
	log_info(log, "[savePens]\t: Saving current colour settings\n");
	for (c=0; c<MAX_PENS; c++){
		vq_color(vdi_handle,c,1,old_rgb[c]);
		log_info(log, "[savePens]\t: Save pen %d [%d,%d,%d]\n", c, old_rgb[c][0], old_rgb[c][1], old_rgb[c][2]);
	}
	return 0;
}

int loadPens(FILE *log, short old_rgb[MAX_PENS][3]){
	short c;
	
	log_info(log, "[loadPens]\t: Loading previous colour settings\n");
	for (c=0; c<MAX_PENS; c++){
		vs_color(vdi_handle,c,old_rgb[c]);
		log_info(log, "[loadPens]\t: Load pen %d [%d,%d,%d]\n", c, old_rgb[c][0], old_rgb[c][1], old_rgb[c][2]);
	}
	return 0;
}

// Set a pen colour
int setPen(FILE *log, short pen, short rgb[3]){

	log_info(log, "[setPen]\t: Set pen %d [%d,%d,%d]\n", pen, rgb[0], rgb[1], rgb[2]);
	vs_color(vdi_handle,pen,rgb);
	return 0;
}

int setLine(FILE *log, short rgb[3]){

	return setPen(log, 1, rgb);
}

int setFill(FILE *log, short rgb[3]){

	return setPen(log, 0, rgb);
}

int main(void)
{
	FILE *log;
	short old_rgb[MAX_PENS][3];// Save old color values
	short rgb[3];
	short attrib[5];
	
	log = fopen("MENU.LOG","w");
	
	log_info(log, "[main]\t: Starting demo\n");
	fflush(log);
	
	work_open();
	
	// Save attributes
	vqf_attributes(vdi_handle, attrib);
	
	savePens(log, old_rgb);
	
	// Draw a horizontal line from x1 to x2, at y
	
	rgb[0] = 1000;
	rgb[1] = 1000;
	rgb[2] = 1000;
	setLine(log, rgb);
	
	rgb[0] = 0;
	rgb[1] = 0;
	rgb[2] = 0;
	setFill(log, rgb);
	
	
	//draw_hline(0, 319, 60);
	
	// Draw a box
	//draw_fbox(0, 0, 40, 100);
	vsf_perimeter(vdi_handle, 1);
	//vsl_color(vdi_handle, 0);
	//vsf_color(vdi_handle, 1);
	draw_box(10, 10, 80, 40, 0);
	draw_box(50, 70, 160, 140, 1);
	
	sleep(5);
	
	loadPens(log, old_rgb);
	vsf_interior(vdi_handle, attrib[0]);
	vswr_mode(vdi_handle, attrib[3]);
	
	work_close();
	log_info(log, "[main]\t: Stopping demo\n");
	fclose(log);
	return 0;
}