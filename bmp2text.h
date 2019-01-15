#include <stdbool.h>

// Definition of the loadfont function
SDL_Surface* loadfont(FILE *log, bool inverse);

// Definition of the text2surface function
int text2surface(SDL_Surface *display, SDL_Surface *font_normal, SDL_Surface *font_reverse, FILE *log, char *text, int x, int y, bool inverse);