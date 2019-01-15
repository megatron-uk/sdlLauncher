#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <SDL/SDL.h>

#include "errors.h"
#include "menu.h"
#include "bmp2text.h"
#include "gamedata.h"

// Global variable to hold SDL events
SDL_Event event;

// Global variable to hold text being blitted to the 
// screen - rather than one buffer per function
char text_buffer[256];
char text_buffer_alt[256];

// Load SDL library
int menu_sdl_init(FILE *log){
	
	SDL_version sdl_compiled;	// Show compiled-time version of SDL 
								// (this will be the runtime version for a static binary)
	int r;						// return codes
	
	// Load SDL display library
	r = SDL_Init(SDL_INIT_VIDEO);
	if (r != 0){
		fprintf(log, "menu_sdl_init SDL Init Error: %s\n", SDL_GetError());
		fclose(log);
		sleep(2);
		exit(-1);
	} else {
		SDL_VERSION(&sdl_compiled);
		fprintf(log, "menu_sdl_init: SDL v%d.%d.%d\n", sdl_compiled.major, sdl_compiled.minor, sdl_compiled.patch);
		//fflush(log);
		return r;
	}
}

// Draw a bordered box at pos x,y, of height h and width w, and of border px thickness
int menu_borders(SDL_Surface *display, FILE *log, int x, int y, int w, int h, int px){
	
	SDL_Rect box;	// A bounding box for the borders we want to draw
	int r;		// return codes
	
	box.x = x;
	box.y = y;
	box.w = w;
	box.h = h;
	// Outer box in white
	r = SDL_FillRect(display, &box, SDL_MapRGB(display->format, 255, 255, 255));
	if ( r != 0){
		fprintf(log, "menu_borders Outer SDL Fill Error: %s\n", SDL_GetError());
		SDL_Quit();
		fclose(log);
		sleep(2);
		exit(-1);
	} else {
		//fprintf(log, "menu_borders: Outer border at (%d,%d) w:%d x h:%d %dpx border\n", x, y, w, h, px);
		//fflush(log);
	}
	
	box.x = x + px;
	box.y = y + px;
	box.w = w - (px + 1);
	box.h = h - (px + 1);
	// Inner box in black
	r = SDL_FillRect(display, &box, SDL_MapRGB(display->format, 0, 0, 0));
	if ( r != 0){
		fprintf(log, "menu_borders Inner SDL Fill Error: %s\n", SDL_GetError());
		fflush(log);
		SDL_Quit();
		fclose(log);
		sleep(2);
		exit(-1);
	} else {
		//fprintf(log, "menu_borders: Inner border at (%d,%d) w:%d x h:%d\n", box.x, box.y, box.w, box.h);
		//fflush(log);	
	}
	return r;
	
}

// Print a message from the menu programme in the bottom status box
int menu_infobox_print(SDL_Surface *display, struct WINDOW_STATE *window_state, FILE *log, char *text){
	
	// Iterate over the string, printing each line seperated by \n on a
	COORDS coords = INFO_COORDS();
	int max_chars = (coords.w / FONT_W) - 1;			// NUmber of characters on a row
	char *text_line = malloc(max_chars * sizeof(char));	// Allocate a buffer of the size of characters we can print on a row
	int src_pos = 0;
	int dest_pos = 0;
	int line_number = 0;
	int i;
	int x;
	int y;
	
	for (src_pos = 0; src_pos < strlen(text); src_pos++){		
		// We read a normal character
		if (text[src_pos] != '\n'){
			// Copy the character to the line buffer
			text_line[dest_pos] = text[src_pos];
			dest_pos++;
		}
			
		// We reach a carriage return or end of string
		if ((text[src_pos] == '\n') | ((src_pos + 1) == strlen(text))){
			// Bitmap position
			x = coords.x + 2;
			y = coords.y + 2 + (line_number * 8);
			
			// Carriage return, print line buffer
			//fprintf(log, "Displaying text at line %d (x:%d,y:%d) [%s]\n", line_number, x, y, text_line);
			
			// Inset by 2px from left hand border and 4px + (font height * line number) from top border
			text2surface(display, window_state->font_normal, window_state->font_reverse, log, text_line, x, y, 0);
			
			// Reset line buffer for next pass
			for (i = 0; i < max_chars; i++){
				text_line[i] =  '\0';
			}
			dest_pos = 0;
			line_number++;
		}
	}
	free(text_line);
	return 0;
}

// Draw the blank game cover/bitmap window
int menu_gamecover_init(SDL_Surface *display, FILE *log){
	
	// redraw our box borders and blank any previous bitmap
	COORDS coords = GAMECOVER_COORDS();
	menu_borders(display, log, coords.x, coords.y, coords.w, coords.h, 1);	
	return 0;
}

// Draw the blank browser window
int menu_browser_init(SDL_Surface *display, FILE *log){

	// Draw an outlined box at 0,137 that is 320x63 pixels for our game list window
	COORDS coords = BROWSER_COORDS();
	menu_borders(display, log, coords.x, coords.y, coords.w, coords.h, 1);
	return 0;
}

// Draw the blank info window
int menu_info_init(SDL_Surface *display, FILE *log){

	// Draw an outlined box at 0,0 that is 138x137 pixels for our info window
	COORDS coords = INFO_COORDS();
	menu_borders(display, log, coords.x, coords.y, coords.w, coords.h, 1);
	return 0;
}

// Draw the blank on-screen text reader box
int menu_textreader_init(SDL_Surface *display, FILE *log){
	
	COORDS coords = READER_COORDS();
	menu_borders(display, log, coords.x, coords.y, coords.w, coords.h, 1);
	return 0;
}

// Load and display the game cover for the currently selected game	
int menu_gamecover_load(SDL_Surface *display, struct WINDOW_STATE *window_state, FILE *log, char *fname){
	
	COORDS coords = GAMECOVER_COORDS();
	SDL_Surface *bmp = NULL;	// Raw bitmap as loaded from disk
	SDL_Rect src, dest;			// Cropping surfaces for displaying bitmaps
	int r;						// return codes

	//fprintf(log, "menu_gamecover_load: Loading [%s]\n", fname);
	// Load splash bitmap
	bmp = SDL_LoadBMP(fname); 
	if (bmp == NULL){
		fprintf(log, "menu_gamecover_load SDL Load Error: %s\n", SDL_GetError());
		//fflush(log);
		SDL_FreeSurface(bmp);
		strcpy(text_buffer, ERROR_BITMAP_OPEN);
		strcat(text_buffer, "\nFilename: ");
		strcat(text_buffer, fname);
		menu_infobox_print(display, window_state, log, text_buffer);
		return r;
	} else {
		//fprintf(log, "menu_gamecover_load: Loaded %dx%dx%dbpp\n", bmp->w, bmp->h, bmp->format->BitsPerPixel);
		//fflush(log);
		// Ensure bitmap is converted to colour mode of display
		//image = SDL_DisplayFormat(bmp);
	}
	
	// Source
	src.x = 0;		// Top left corner of source bitmap
	src.y = 0;		// Top left corner of source bitmap
	src.w = BMP_W;	// Crop to our sizes defined in menu.h
	src.h = BMP_H;	// Crop to our sizes defined in menu.h

	// Destination
	dest.x = coords.x + 1; //(SCREEN_W - BMP_W); 	// Load the bitmap into the bordered window          
	dest.y = coords.y + 1;	// Load the bitmap into the bordered window 
	dest.w = coords.w - 2;  // Crop to our sizes defined in menu.h
	dest.h = coords.h - 2;	// Crop to our sizes defined in menu.h
	
	// Write bitmap to display
	//r = SDL_BlitSurface(image, &src, display, &dest);
	r = SDL_BlitSurface(bmp, &src, display, &dest);
	if ( r != 0){
		fprintf(log, "menu_gamecover_load SDL Blit Error: %s\n", SDL_GetError());
		SDL_Quit();
		fclose(log);
		sleep(2);
		exit(-1);
	}
	SDL_FreeSurface(bmp);
	return r;
}

int menu_gamecover_populate(SDL_Surface *display, FILE *log, struct GAME_DATA *game_data, struct WINDOW_STATE *window_state){

	// Full path and filename of game bitmap
	char fullpath[(GAME_PATH_LEN + GAME_FILE_LEN)];
	
	// Index of selected game
	int selected = window_state->browser_window.select_pos;
	
	// Blank and redraw window borders
	menu_gamecover_init(display, log);
		
	// Show cover bitmap
	if (game_data->game_data_items[selected].has_bitmap){
		strcpy(fullpath, game_data->game_data_items[selected].path);
		strcat(fullpath, DIRSEP);
		strcat(fullpath, game_data->game_data_items[selected].bitmap);
		menu_gamecover_load(display, window_state, log, fullpath);	
	}
	
	return 0;
}

// The main scrollable game browser window
int menu_browser_populate(SDL_Surface *display, FILE *log, struct GAME_DATA *game_data, struct WINDOW_STATE *window_state){
	
	int i;
	bool selected = 0;
	
	// Maximum number of rows we can show
	
	// Current start pos
	// Current end pos

	// Blank and redraw window borders
	menu_browser_init(display, log);
	
	fprintf(log, "menu_browser_populate: start_pos: %d\n", window_state->browser_window.start_pos);
	fprintf(log, "menu_browser_populate: end_pos: %d\n", window_state->browser_window.end_pos);
	fprintf(log, "menu_browser_populate: select_pos: %d\n", window_state->browser_window.select_pos);
	fprintf(log, "menu_browser_populate: last_pos: %d\n", window_state->browser_window.last_pos);
	
	// Any games?
	if (game_data->items > 0){
		// Is our game list longer than max number of rows?
		/*if (game_data->items > window_state->browser_window.max_lines){
			// More games than lines available - we can only show some of them
			
			// Is select position > end of visible list?
			if (window_state->browser_window.select_pos > window_state->browser_window.end_pos){
				window_state->browser_window.start_pos++;
				window_state->browser_window.end_pos++;
			} else if (window_state->browser_window.select_pos < window_state->browser_window.start_pos){
				window_state->browser_window.start_pos--;
				window_state->browser_window.end_pos--;
			}
			for (i = window_state->browser_window.start_pos; i < window_state->browser_window.max_lines; i++){
				if (i == window_state->browser_window.select_pos){
					selected = 1;
				} else {
					selected = 0;	
				}
				text2surface(display, window_state->font_normal, window_state->font_reverse, log, game_data->game_data_items[i].directory, 2, 3 + (i * FONT_H), selected);
			}
			
		} else {*/
			// Less games than lines available - print them all			
			for (i = 0; i < game_data->items; i++){
				if (i == window_state->browser_window.select_pos){
					selected = 1;
				} else {
					selected = 0;	
				}
				text2surface(display, window_state->font_normal, window_state->font_reverse, log, game_data->game_data_items[i].directory, 2, 3 + (i * FONT_H), selected);
			}
	/*	}*/
	} else {
		// No games found :(
		text2surface(display, window_state->font_normal, window_state->font_reverse, log, "No entries", 2, 3, 0);
	}
	//fflush(log);
	return 0;
}

// Open/close the textreader file handler
int menu_textreader_file(FILE *log, struct WINDOW_STATE *window_state, struct GAME_DATA *game_data, bool open_file){
	
	int pos = window_state->browser_window.select_pos;
	
	if (open_file == 1){
		// Open file pointer
		strcpy(window_state->text_window.buffer, game_data->game_data_items[pos].path);
		strcat(window_state->text_window.buffer, DIRSEP);
		if (window_state->info_window.readme_selected == 1){
			strcat(window_state->text_window.buffer, game_data->game_data_items[pos].readme_1);
		}
		if (window_state->info_window.readme_selected == 2){
			strcat(window_state->text_window.buffer, game_data->game_data_items[pos].readme_2);
		}
		if (window_state->info_window.readme_selected == 3){
			strcat(window_state->text_window.buffer, game_data->game_data_items[pos].readme_3);
		}
		fprintf(log, "menu_textreader_file: Open file [%s]\n", window_state->text_window.buffer);
		window_state->text_window.readme = fopen(window_state->text_window.buffer, "r");
		memset(window_state->text_window.buffer, '\0', sizeof(window_state->text_window.buffer));
		if (window_state->text_window.readme != NULL){
			return 0;
		} else {
			return -1;
		}
	} else {
		// Close file pointer
		if (window_state->text_window.readme != NULL){
			fclose(window_state->text_window.readme);
			memset(window_state->text_window.buffer, '\0', sizeof(window_state->text_window.buffer));
			fprintf(log, "menu_textreader_file: Closed file\n");
			return 0;
		} else {
			fprintf(log, "menu_textreader_file: Attempt to close a file not already open!\n");
			return 0;
		}
	}
}

// Update the text reader window
int menu_textreader_populate(SDL_Surface *display, FILE *log, struct GAME_DATA *game_data, struct WINDOW_STATE *window_state){
	
	int game_id = window_state->browser_window.select_pos;				// Current game_id
	struct GAME_DATA_ITEM game = game_data->game_data_items[game_id];	// Current game data
	struct COORDS coords = READER_COORDS();								// Geometry of reader window
	int total_max_chars;
	int i;
	int f_size;
	char c;
	
	window_state->text_window.max_lines = (coords.h / FONT_H) - 1;
	window_state->text_window.max_chars = (coords.w / FONT_W) - 1;
	total_max_chars = window_state->text_window.max_lines * window_state->text_window.max_chars;
		
	fprintf(log, "menu_textreader_populate: Reading file\n");
	fseek(window_state->text_window.readme, 0L, SEEK_END);
	f_size = ftell(window_state->text_window.readme);
	fprintf(log, "menu_textreader_populate: File is %d bytes\n", f_size);
	fflush(log);
	if (f_size < sizeof(window_state->text_window.buffer)){
		fprintf(log, "menu_textreader_populate: Read entire file\n");
		fflush(log);
		fread(*window_state->text_window.buffer, 1, f_size, window_state->text_window.readme);
		printf("%s", window_state->text_window.buffer);
	} else {
		for (i = 0; i < total_max_chars; i++){
			c = fgetc(window_state->text_window.readme);
			printf("%c", c);
			if (c == EOF){
				fprintf(log, "menu_textreader_populate: %d bytes read\n", i);
				fflush(log);
				break;
			} else {
				strcat(window_state->text_window.buffer, c);
			}
		}
	}
	//printf(window_state->text_window.buffer);
	
	return 0;
}

// The bottom info window - update it to reflect the current selected
// game, plus any additional details for the game.
int menu_info_populate(SDL_Surface *display, FILE *log, struct GAME_DATA *game_data, struct WINDOW_STATE *window_state){
	
	COORDS coords = INFO_COORDS();
	int game_id = window_state->browser_window.select_pos;		// short hand
	int last_game_id = window_state->browser_window.last_pos;	// short hand
	int rev = 0;												// Reverse video
	int x_offset = coords.x + 30;											// All text offset this min amount from lhs
	int binary_y_pos = coords.y + 2;
	int readme_y_pos = coords.y + 2 + (2 * FONT_H);
	int gamedata_y_pos = coords.y + 2 + (4 * FONT_H);
	int gamedata_y_pos2 = coords.y + 2 + (6 * FONT_H);
	
	//fprintf(log, "game_id: %d\n", game_id);
	//fprintf(log, "last_game_id: %d\n", last_game_id);
	
	// Blank and redraw window borders
	menu_info_init(display, log);
	
	// Is this the same game chosen as the last time?
	if (game_id != last_game_id){
		// Different game to last time, reset all choices
		window_state->info_window.binary_selected = 0;
		window_state->info_window.readme_selected = 0;
		window_state->info_window.info_mode_selected = M_BINARY;
	}
	
	//==========================================================
	// Draw all possible binaries
	// Is the 'choose binary' row currently selected?
	if (window_state->info_window.info_mode_selected == M_BINARY){
		text2surface(display, window_state->font_normal, window_state->font_reverse, log, ">", 4, binary_y_pos, 0);
	}
	// binary_1
	if (game_data->game_data_items[game_id].binary_1[0] != '\0'){
		if (window_state->info_window.binary_selected == 1){
			rev = 1;
		} else {
			rev = 0;	
		}
		text2surface(display, window_state->font_normal, window_state->font_reverse, log, game_data->game_data_items[game_id].binary_1, x_offset, binary_y_pos, rev);
	}
	// binary_2
	if (game_data->game_data_items[game_id].binary_2[0] != '\0'){
		if (window_state->info_window.binary_selected == 2){
			rev = 1;
		} else {
			rev = 0;	
		}
		text2surface(display, window_state->font_normal, window_state->font_reverse, log, game_data->game_data_items[game_id].binary_2, (x_offset + ((16 * FONT_W) * 1)), binary_y_pos, rev);
	}
	// binary_3
	if (game_data->game_data_items[game_id].binary_3[0] != '\0'){
		if (window_state->info_window.binary_selected == 3){
			rev = 1;
		} else {
			rev = 0;	
		}
		text2surface(display, window_state->font_normal, window_state->font_reverse, log, game_data->game_data_items[game_id].binary_3, (x_offset + ((16 * FONT_W) * 2)), binary_y_pos, rev);
	}
	
	//==========================================================
	// Draw all possible readme files
	// Is the 'choose binary' row currently selected?
	if (window_state->info_window.info_mode_selected == M_README){
		text2surface(display, window_state->font_normal, window_state->font_reverse, log, ">", 4, readme_y_pos, 0);
	}
	// readme_1
	if (game_data->game_data_items[game_id].readme_1[0] != '\0'){
		if (window_state->info_window.readme_selected == 1){
			rev = 1;
		} else {
			rev = 0;	
		}
		text2surface(display, window_state->font_normal, window_state->font_reverse, log, game_data->game_data_items[game_id].readme_1, x_offset, readme_y_pos, rev);
	}
	// readme_2
	if (game_data->game_data_items[game_id].readme_2[0] != '\0'){
		if (window_state->info_window.readme_selected == 2){
			rev = 1;
		} else {
			rev = 0;	
		}
		text2surface(display, window_state->font_normal, window_state->font_reverse, log, game_data->game_data_items[game_id].readme_2, (x_offset + ((16 * FONT_W) * 1)), readme_y_pos, rev);
	}
	// readme_3
	if (game_data->game_data_items[game_id].readme_3[0] != '\0'){
		if (window_state->info_window.readme_selected == 3){
			rev = 1;
		} else {
			rev = 0;	
		}
		text2surface(display, window_state->font_normal, window_state->font_reverse, log, game_data->game_data_items[game_id].readme_3, (x_offset + ((16 * FONT_W) * 2)), readme_y_pos, rev);
	}
	
	//====================================
	// game data - show the path to the game
	strcpy(text_buffer, "Path: ");
	strcat(text_buffer, game_data->game_data_items[game_id].path);
	text2surface(display, window_state->font_normal, window_state->font_reverse, log, text_buffer, x_offset, gamedata_y_pos, 0);
	
	// Display counter of game_id selected from total number available
	// e.g. 13/254
	strcpy(text_buffer, "Game: ");
	sprintf(text_buffer_alt, "%d", (window_state->browser_window.select_pos + 1));
	strcat(text_buffer, text_buffer_alt);
	strcat(text_buffer, "/");
	sprintf(text_buffer_alt, "%d", game_data->items);
	strcat(text_buffer, text_buffer_alt);
	text2surface(display, window_state->font_normal, window_state->font_reverse, log, text_buffer, x_offset, gamedata_y_pos2, 0);
	
	// Update last selected game to this one
	window_state->browser_window.last_pos = game_id;
	
	return 0;
}

// Toggle between selected windows
int menu_toggle_window(FILE *log, struct WINDOW_STATE *window_state){
	
	if (window_state->selected_window == W_BROWSER){
		window_state->selected_window = W_INFO;
		//fprintf(log, "menu_toggle_window: Info panel now selected\n");
		return 0;
	} else if (window_state->selected_window == W_INFO){
		window_state->selected_window = W_BROWSER;
		//fprintf(log, "menu_toggle_window: Browser now selected\n");
		return 0;
	} else {
		//fprintf(log, "menu_toggle_window: Error unknown window mode\n");
		return -1;
	}
}

// Toggle rows and selected item in info window
int menu_toggle_info_window_mode(SDL_Surface *display, FILE *log, struct GAME_DATA *game_data, struct WINDOW_STATE *window_state, SDL_Event event){
	
	// Scroll down to readme line
	if (event.key.keysym.sym == SDLK_DOWN){
		if (window_state->info_window.info_mode_selected == M_BINARY){
			window_state->info_window.info_mode_selected = M_README;
			return 0;
		}
	}
	
	// Scroll up to binary line
	if (event.key.keysym.sym == SDLK_UP){
		if (window_state->info_window.info_mode_selected == M_README){
			window_state->info_window.info_mode_selected = M_BINARY;
			return 0;
		}
	}
	
	// Scroll left through readme files
	if ((event.key.keysym.sym == SDLK_LEFT) && (window_state->info_window.info_mode_selected == M_README)){
		if (window_state->info_window.readme_selected > 0){
			window_state->info_window.readme_selected--;
			return 0;
		}
	}
	
	// Scroll right through readme files
	if ((event.key.keysym.sym == SDLK_RIGHT) && (window_state->info_window.info_mode_selected == M_README)){
		
		if ((window_state->info_window.readme_selected == 0) && (game_data->game_data_items[window_state->browser_window.select_pos].readme_1[0] != '\0')){
			window_state->info_window.readme_selected = 1;
			return 0;
		}
		
		if ((window_state->info_window.readme_selected == 1) && (game_data->game_data_items[window_state->browser_window.select_pos].readme_2[0] != '\0')){
			window_state->info_window.readme_selected = 2;
			return 0;
		}
		
		if ((window_state->info_window.readme_selected == 2) && (game_data->game_data_items[window_state->browser_window.select_pos].readme_3[0] != '\0')){
			window_state->info_window.readme_selected = 3;
			return 0;
		}
	}
	
	// Scroll left through binary files
	if ((event.key.keysym.sym == SDLK_LEFT) && (window_state->info_window.info_mode_selected == M_BINARY)){
		if (window_state->info_window.binary_selected > 0){
			window_state->info_window.binary_selected--;
			return 0;
		}
	}
	
	// Scroll right through binary files
	if ((event.key.keysym.sym == SDLK_RIGHT) && (window_state->info_window.info_mode_selected == M_BINARY)){
		
		if ((window_state->info_window.binary_selected == 0) && (game_data->game_data_items[window_state->browser_window.select_pos].binary_1[0] != '\0')){
			window_state->info_window.binary_selected = 1;
			return 0;
		}
		
		if ((window_state->info_window.binary_selected == 1) && (game_data->game_data_items[window_state->browser_window.select_pos].binary_2[0] != '\0')){
			window_state->info_window.binary_selected = 2;
			return 0;
		}
		
		if ((window_state->info_window.binary_selected == 2) && (game_data->game_data_items[window_state->browser_window.select_pos].binary_3[0] != '\0')){
			window_state->info_window.binary_selected = 3;
			return 0;
		}
	}
	
	// Run selected item
	if (event.key.keysym.sym == SDLK_RETURN){
		if (window_state->info_window.info_mode_selected == M_BINARY){
			// Start the selected binary
		} else if (window_state->info_window.info_mode_selected == M_README){
			// View a readme file
			if (window_state->info_window.readme_selected > 0){
				window_state->selected_window = W_TEXT;
				fprintf(log, "menu_toggle_info_window_mode: Open document %d\n", window_state->info_window.readme_selected);
				fflush(log);
				// Open reader window
				menu_textreader_init(display, log);
				// Open file handler
				menu_textreader_file(log, window_state, game_data, 1);
				// Draw initial contents
				menu_textreader_populate(display, log, game_data, window_state);
			}
		}
	} 
	
	// No-op
	return 0;
}

// Move selection down rows in browser window
int menu_toggle_browser_window(FILE *log, struct GAME_DATA *game_data, struct WINDOW_STATE *window_state, SDL_Event event){

	if (event.key.keysym.sym == SDLK_DOWN){
		if (window_state->browser_window.select_pos < (game_data->items - 1)){
			// Move up list
			window_state->browser_window.select_pos++;
		}
	}
	
	if (event.key.keysym.sym == SDLK_UP){
		if (window_state->browser_window.select_pos > 0){
			// Move down list
			window_state->browser_window.select_pos--;
		}
	}
	return 0;
}

// Save last selected game
int menu_save_last_game(struct WINDOW_STATE *window_state){

	// Save last selected game
	window_state->browser_window.last_pos = window_state->browser_window.select_pos;
	
	return 0;		
}

// Initialise game data structures
int menu_init_gamedata(FILE *log, struct GAME_DATA *game_data){
	
	// Set initial list position and number of items to zero
	game_data->pos = 0;
	game_data->items = 0;
	
	return 0;
}

// Initialise window data structures
int menu_init_windowstate(FILE *log, struct WINDOW_STATE *window_state){
	
	// Default to having the browser selected
	window_state->selected_window = W_BROWSER;
	
	// Default to first item in the browser list selected
	window_state->browser_window.start_pos = 0;
	window_state->browser_window.end_pos = 0;
	window_state->browser_window.select_pos = 0;
	window_state->browser_window.last_pos = -1;
	
	// Maximum number of lines we can show in browser
	window_state->browser_window.max_lines = (BMP_H - 3) / FONT_H;
	
	// Unselect all info window choices
	window_state->info_window.binary_selected = 0;
	window_state->info_window.readme_selected = 0;
	
	// Maximum number of lines we can show in text reader
	// Maximum number of characters in width we can show in text reader
	
	// Load fonts
	window_state->font_normal = loadfont(log, 0);
	window_state->font_reverse = loadfont(log, 1);
	return 0;
}

// Main menu runtime
int main(int argc, char* argv[]){
	
	SDL_Surface *display = NULL; 		// Main display surface as initialised by menu_sdl_init
	char vdriver[32];					// Name of video driver
	struct GAME_DATA game_data;			// Global game data list
	struct WINDOW_STATE window_state;	// Global window state tracking
	
	int r;						// return codes
	bool quit = 0;				// exit
	FILE *log;					// Log file
	
	log = fopen("MENU.LOG","w");

	fprintf(log, "Menu tool running\n");
	fprintf(log, "-----------------\n");
	fflush(log);

	// Init game data
	menu_init_gamedata(log, &game_data);
	
	// Init window data
	menu_init_windowstate(log, &window_state);
	
	// Load SDL and set up video mode
	menu_sdl_init(log);
	
	// Set display mode for the SDL screen
	display = SDL_SetVideoMode(SCREEN_W, SCREEN_H, SCREEN_BPP, SDL_HWSURFACE);
	if (display == NULL){
		fprintf(log, "SDL Driver Error: %s\n", SDL_GetError());
		SDL_Quit();
		fclose(log);
		sleep(2);
		exit(-1);
	} else {
		fprintf(log, "SDL Driver %s\n", SDL_VideoDriverName(vdriver, 32));
		fprintf(log, "SDL Driver mode %dx%dx%dbpp\n", display->w, display->h, display->format->BitsPerPixel);
		fprintf(log, "SDL Driver has HW Surfaces? (%d)\n", SDL_GetVideoSurface()->flags & SDL_HWSURFACE);
		fflush(log);
	}
	
	// Init all windows
	menu_info_init(display, log);
	menu_gamecover_init(display, log);
	menu_browser_init(display, log);
	menu_infobox_print(display, &window_state, log, INFO_GAMEDIR_WAIT);
	SDL_Flip(display);
	
	// Get initial list of games in a directory
	r = scangames(log, GAMEDIR, &game_data);
	if (r < 0){
		menu_infobox_print(display, &window_state, log, ERROR_GAMEDIR_OPEN);	
	} else {
		menu_infobox_print(display, &window_state, log, INFO_GAMEDIR_SUCCESS);	
	}
	SDL_Flip(display);
	
	// Fill the browser window once
	menu_browser_populate(display, log, &game_data, &window_state);
	menu_gamecover_populate(display, log, &game_data, &window_state);
	menu_info_populate(display, log, &game_data, &window_state);
	SDL_Flip(display);
	
	fflush(log);
	
	// Main loop looking for user input
    while ( quit == false ){
		//While there's an event to handle
		while (SDL_PollEvent(&event)){
			//If the user has Xed out the window
            if (event.type == SDL_QUIT){
				//Quit the program
				fprintf(log, "Quit selected\n");
				fflush(log);
			} 
			// Check for keypresses
			if (event.type == SDL_KEYDOWN){
				switch(event.key.keysym.sym){
					case SDLK_q:
						fprintf(log, "Quit selected\n");
						fflush(log);
						// Close textreader if open
						if (window_state.selected_window == W_TEXT){
							menu_textreader_file(log, &window_state, &game_data, 0);
						}
						quit = 1;
						break;
                    case SDLK_UP: 
                    	if (window_state.selected_window == W_BROWSER){
                    		// scroll up through browser list
                    		menu_toggle_browser_window(log, &game_data, &window_state, event);
                    		menu_browser_populate(display, log, &game_data, &window_state);
                    		menu_info_populate(display, log, &game_data, &window_state);
                   			menu_gamecover_populate(display, log, &game_data, &window_state);
                    	} else if (window_state.selected_window == W_INFO){
                    		// scroll up between binary/readme lines
                    		menu_toggle_info_window_mode(display, log, &game_data, &window_state, event);
                    		menu_info_populate(display, log, &game_data, &window_state);
                    	} else if (window_state.selected_window == W_TEXT){
                    		// scroll up text window
                    	} else {
                    		// no-nop	
                    	}
                    	break;
                    case SDLK_DOWN: 
                    	if (window_state.selected_window == W_BROWSER){
                    		// scroll down through browser list
                    		menu_toggle_browser_window(log, &game_data, &window_state, event);
                    		menu_browser_populate(display, log, &game_data, &window_state);
                    		menu_info_populate(display, log, &game_data, &window_state);
                    		menu_gamecover_populate(display, log, &game_data, &window_state); 
                    	} else if (window_state.selected_window == W_INFO){
                    		// scroll down between binary/readme lines
                    		menu_toggle_info_window_mode(display, log, &game_data, &window_state, event);
                    		menu_info_populate(display, log, &game_data, &window_state);
                    	} else if (window_state.selected_window == W_TEXT){
                    		// scroll down text window
                    	} else {
                    		// no-nop	
                    	}
                    	break;
                    case SDLK_LEFT: 
                    	// Scroll left between binary/readme lines
                    	if (window_state.selected_window == W_INFO){
                    		menu_toggle_info_window_mode(display, log, &game_data, &window_state, event);
                    		menu_info_populate(display, log, &game_data, &window_state);
                    	}
                    	break;
                    case SDLK_RIGHT:
                    	// Scroll right between binary/readme lines
                    	if (window_state.selected_window == W_INFO){
                    		menu_toggle_info_window_mode(display, log, &game_data, &window_state, event);
                    		menu_info_populate(display, log, &game_data, &window_state);
                    	}
                    	break;
                    case SDLK_TAB:
                    	// Swap between browser and info windows
                    	menu_toggle_window(log, &window_state);
                    	break;
                    case SDLK_RETURN:
                    	// Choose highlighted option
                    	if (window_state.selected_window == W_INFO){
                    		menu_toggle_info_window_mode(display, log, &game_data, &window_state, event);
                    	}
                    	break;
                    case SDLK_ESCAPE:
                    	// Close text reader from info window
                    	if (window_state.selected_window == W_TEXT){
                    		menu_textreader_file(log, &window_state, &game_data, 0);
                    		menu_browser_populate(display, log, &game_data, &window_state);
                    		menu_info_populate(display, log, &game_data, &window_state);
                    		menu_gamecover_populate(display, log, &game_data, &window_state);
                    		window_state.selected_window = W_INFO;
                    	}
                    	break;
                    default:
                    	break;
                }
                // Refresh all windows
				
				
				SDL_Flip(display);
				
				// Flush any log message
				fflush(log);
			}
		}
		//SDL_Delay(100);
    }	
	// Tidy up SDL before closing
	SDL_Delay(1500);
	SDL_FreeSurface(display);
	SDL_Quit();
	
	fprintf(log, "Exiting\n");
	fclose(log);
	return 0;
}