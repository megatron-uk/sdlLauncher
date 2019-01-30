#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <mint/osbind.h>
#include <gem.h>
#include <unistd.h>

#include "gfx.h"
#include "logging.h"

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
#define SIZE_NAME	(gem_screen.w/20)
#define SIZE_HELP	(gem_screen.w/32)
#define SIZE_SCORE	(gem_screen.w/32)
#define SIZE_GAMEOVER	(gem_screen.w/10)

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
	vr_recfl(gem_vdi_handle, pxyarray);
}

/* draw horizontal line, with dashes (restores solid lines) */
static void draw_hline(int x1, int x2, int y)
{
	short pxyarray[4];

	pxyarray[0] = x1;
	pxyarray[1] = y;
	pxyarray[2] = x2;
	pxyarray[3] = y;
	v_pline(gem_vdi_handle, 2, pxyarray);
}

/* draw vertical line, with dashes (restores solid lines) */
static void draw_vline(int y1, int y2, int x)
{
	short pxyarray[4];

	pxyarray[0] = x;
	pxyarray[1] = y1;
	pxyarray[2] = x;
	pxyarray[3] = y2;
	v_pline(gem_vdi_handle, 2, pxyarray);
}

// Draw a rectangular box and fill it
static void draw_box(FILE *log, int x1, int y1, int x2, int y2, int fill){
	
	short pxyarray[4];
	
	log_debug(log, "[%s:%d]\t: (draw_box)\t: Drawing\n", __FILE__, __LINE__);
	pxyarray[0] = x1;
	pxyarray[1] = y1;
	pxyarray[2] = x2;
	pxyarray[3] = y2;
	if (fill){
		log_debug(log, "[%s:%d]\t: (draw_box)\t: Drawing SOLID\n", __FILE__, __LINE__);
		vsf_interior(gem_vdi_handle, FIS_SOLID);
	} else {
		log_debug(log, "[%s:%d]\t: (draw_box)\t: Drawing HOLLOW\n", __FILE__, __LINE__);
		vsf_interior(gem_vdi_handle, FIS_HOLLOW);
	}
	vsf_perimeter(gem_vdi_handle, 1);
	v_bar(gem_vdi_handle, pxyarray);
	log_debug(log, "[%s:%d]\t: (draw_box)\t: Drawn\n", __FILE__, __LINE__);
}

static void draw_hline_dashed(int x1, int x2, int y)
{
	short gem_attrib[6];
	vql_attributes(gem_vdi_handle, gem_attrib);

	vsl_type(gem_vdi_handle, LONGDASH);/* sometimes also named as LDASHED */
	draw_hline(x1, x2, y);
	/* restore line settings */
	vsl_type(gem_vdi_handle, gem_attrib[0]);
}

int main(void)
{
	FILE *log;
	short rgb[3];
	agnostic_bitmap screen;
	agnostic_colours colours;
	int r;
	
	log = fopen("MENU.LOG","w");
	
	log_info(log, "[main]\t: Starting demo\n");
	
	//gfxInitLocal(log);
	
	gfxInit(log);
	r = gfxSetMode(log, &screen, 320, 200, 4);
	r = 0;
	if (r != 0){
		gfxQuit(log);
		log_error(log, "[main]\t: Stopping demo\n");
		fclose(log);
		return 0;
	}

	gfxDrawBox(log, &screen, 10, 10, 70, 20, &colours, &colours, 3, 2);
	gfxDrawBox(log, &screen, 100, 90, 200, 95, &colours, &colours, 3, 2);
	
	sleep(3);
	
	//work_close();
	gfxQuit(log);
	log_info(log, "[main]\t: Stopping demo\n");
	fclose(log);
	return 0;
}