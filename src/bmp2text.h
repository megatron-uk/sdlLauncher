#pragma once

#include <stdbool.h>

// Definition of the loadfont function
int loadfont(FILE *log, struct agnostic_bitmap *bmp, bool inverse);

// Definition of the text2surface function
int text2BMP(struct agnostic_bitmap *display, struct agnostic_bitmap *font_normal, struct agnostic_bitmap *font_reverse, FILE *log, char *text, int x, int y, bool inverse);