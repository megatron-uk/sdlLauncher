// Global variable to hold text being blitted to the 
// screen - rather than one buffer per function
// Any function using these arrays is responsible to
// initialise it first.
char text_buffer[256];
char text_buffer_alt[256];

// Draw a bordered box at pos x,y, of height h and width w, and of border px thickness
int menu_borders(agnostic_bitmap *display, FILE *log, int x, int y, int w, int h, int border_px, int shadow_px){
	
	agnostic_colours border;	// Colour spec
	agnostic_colours fill;		// Colour spec
	int r;					// return codes
	
	border.r = 255;
	border.g = 255;
	border.b= 255;
	fill.r = 0;
	fill.g = 0;
	fill.b= 0;
	r = gfxDrawBox(log, display, x, y, w, h, &border, &fill, border_px, shadow_px);
	if ( r != 0){
		log_error(log, "[menu_borders]\t: Drawing Error\n");
		gfxQuit(log);
		fclose(log);
		exit(-1);
	}	
	return r;
	
}

// Draw the blank game cover/bitmap window
int menu_gamecover_init(agnostic_bitmap *display, FILE *log){
	
	// redraw our box borders and blank any previous bitmap
	log_debug(log, "[%s:%d]\t: (menu_gamecover_init)\t: Redrawing window\n", __FILE__, __LINE__);
	COORDS coords = GAMECOVER_COORDS();
	menu_borders(display, log, coords.x, coords.y, coords.w, coords.h, 1, 0);	
	return 0;
}

// Draw the blank browser window
int menu_browser_init(agnostic_bitmap *display, FILE *log){

	// Draw an outlined box at 0,137 that is 320x63 pixels for our game list window
	log_debug(log, "[%s:%d]\t: (menu_browser_init)\t: Redrawing window\n", __FILE__, __LINE__);
	COORDS coords = BROWSER_COORDS();
	menu_borders(display, log, coords.x, coords.y, coords.w, coords.h, 1, 0);
	return 0;
}

// Draw the alphabet menu window
int menu_category_init(agnostic_bitmap *display, FILE *log){
	
	log_debug(log, "[%s:%d]\t: (menu_category_init)\t: Redrawing window\n", __FILE__, __LINE__);
	COORDS coords = ALPHABET_COORDS();
	menu_borders(display, log, coords.x, coords.y, coords.w, coords.h, 1, 0);
	return 0;
}

// Draw the blank info window
int menu_info_init(agnostic_bitmap *display, FILE *log){

	// Draw an outlined box at 0,0 that is 138x137 pixels for our info window
	log_debug(log, "[%s:%d]\t: (menu_info_init)\t: Redrawing window\n", __FILE__, __LINE__);
	COORDS coords = INFO_COORDS();
	menu_borders(display, log, coords.x, coords.y, coords.w, coords.h, 1, 0);
	return 0;
}

// Draw the blank on-screen text reader box
int menu_textreader_init(agnostic_bitmap *display, FILE *log){
	
	log_debug(log, "[%s:%d]\t: (menu_textreader_init)\t: Redrawing window\n", __FILE__, __LINE__);
	COORDS coords = READER_COORDS();
	menu_borders(display, log, coords.x, coords.y, coords.w, coords.h, 1, 4);
	return 0;
}

// Draw the config menu window
int menu_config_init(agnostic_bitmap *display, FILE *log){
	
	log_debug(log, "[%s:%d]\t: (menu_config_init)\t: Redrawing window\n", __FILE__, __LINE__);
	COORDS coords = CONFIG_COORDS();
	menu_borders(display, log, coords.x, coords.y, coords.w, coords.h, 1, 4);
	return 0;
}

// Print a message from the menu programme in the bottom status box
int menu_infobox_print(agnostic_bitmap *display, struct WINDOW_STATE *window_state, FILE *log, char *text){
	
	// Iterate over the string, printing each line seperated by \n on a
	COORDS coords = INFO_COORDS();
	int max_chars = (coords.w / FONT_W) - 1;	// NUmber of characters on a row
	char text_line[max_chars + 1];				// Allocate a buffer of the size of characters we can print on a row
	int src_pos = 0;
	int dest_pos = 0;
	int line_number = 0;
	int x;
	int y;

	// This is a generic error/warning/info function to use
	// the info window, so blank anything that was there previously.
	menu_info_init(display, log);

	// Blank the local text buffer
	log_debug(log, "[%s:%d]\t: (menu_config_init)\t: Clearing text buffer\n", __FILE__, __LINE__);
	memset(text_line, '\0', sizeof(text_line));
	for (src_pos = 0; src_pos < strlen(text); src_pos++){		
		// We read a normal character
		if (text[src_pos] != '\n'){
			// Copy the character to the line buffer
			text_line[dest_pos] = text[src_pos];
			dest_pos++;
		}
			
		// We reach a carriage return or end of string
		if ((text[src_pos] == '\n') || ((src_pos + 1) == strlen(text))){
			// Bitmap position
			x = coords.x + 2;
			y = coords.y + 2 + (line_number * 8);
			
			// Carriage return, print line buffer
			log_debug(log, "[%s:%d]\t: (menu_config_init)\t: Writing line %d\n", __FILE__, __LINE__, line_number);
			log_debug(log, "[%s:%d]\t: (menu_config_init)\t: text [%s]\n", __FILE__, __LINE__, text);
			text2BMP(display, window_state->font_normal, window_state->font_reverse, log, text_line, x, y, 0);
			
			// Reset line buffer for next pass
			memset(text_line, '\0', sizeof(text_line));
			dest_pos = 0;
			line_number++;
		}
	}
	return 0;
}

// Adjust start and end position in game list based on category selected
int menu_refilter_browser(FILE *log, struct GAME_DATA *game_data, struct WINDOW_STATE *window_state){
	
	int i;
	int new_start_pos = -1;
	int new_end_pos = -1;
	char cat_letter;
	
	log_debug(log, "============================\n");
	log_debug(log, "menu_refilter_browser: Category ID [%d]\n", window_state->category_window.cat_selected);
	if (window_state->category_window.cat_selected == CATEGORY_ALL){
		// Show all items
		log_debug(log, "menu_refilter_browser: Show all entries\n");	
		window_state->browser_window.select_pos = 0;
		window_state->browser_window.start_pos = 0;
		window_state->browser_window.start_pos_filtered = 0;		
		window_state->browser_window.end_pos = game_data->items;
		window_state->browser_window.end_pos_filtered = game_data->items;
		
	} else if (window_state->category_window.cat_selected == CATEGORY_FAV){
		// Show only favourite items
		log_debug(log, "menu_refilter_browser: Show favourites\n");	
		
	} else if (window_state->category_window.cat_selected == CATEGORY_NUM){
		// Show games starting with 0-9
		log_debug(log, "menu_refilter_browser: Show games with numeric titles\n");	
		
	} else if (window_state->category_window.cat_selected >= CATEGORY_ALPHA){
		// Show games starting with A-Z, that letter only
		cat_letter = ALPHABET_CATS[window_state->category_window.cat_selected];
		log_debug(log, "menu_refilter_browser: Show games with titles [%c]\n", cat_letter);
		// Find the start position that matches our new selected category
		for (i = 0; i<game_data->items; i++){
			if (game_data->game_data_items[i].name[0] == cat_letter){
				new_start_pos = i;
				log_debug(log, "menu_refilter_browser: New start position [%d]\n", new_start_pos);	
				break;
			}
		}
		// Find the end position that matches our new selected category
		for (i = game_data->items; i >= 0; i--){
			if (game_data->game_data_items[i].name[0] == cat_letter){
				new_end_pos = i;
				log_debug(log, "menu_refilter_browser: New end position [%d]\n", new_end_pos);	
				break;
			}
		}
		// Update select position to be the start position
		window_state->browser_window.select_pos = new_start_pos;
		window_state->browser_window.start_pos = new_start_pos;
		window_state->browser_window.start_pos_filtered = new_start_pos;
		window_state->browser_window.end_pos = new_start_pos + window_state->browser_window.max_lines;
		window_state->browser_window.end_pos_filtered = new_end_pos;
	}
	return 0;
}


// Load and display the game cover for the currently selected game	
int menu_gamecover_load(agnostic_bitmap *display, struct WINDOW_STATE *window_state, FILE *log, char *fname){
	
	COORDS coords = GAMECOVER_COORDS();
	agnostic_bitmap bmp;			// Raw bitmap as loaded from disk
	agnostic_window src, dest;		// Cropping surfaces for displaying bitmaps
	int r = 0;						// return codes

	log_debug(log, "============================\n");
	log_debug(log, "menu_gamecover_load: Loading [%s]\n", fname);
	// Load splash bitmap
	r = gfxLoadBMP(log, fname, &bmp); 
	if (r != 0){
		log_error(log, "menu_gamecover_load Load Error\n");
		gfxFreeBMP(log, &bmp);
		strcpy(text_buffer, ERROR_BITMAP_OPEN);
		strcat(text_buffer, "\nFilename: ");
		strcat(text_buffer, fname);
		menu_infobox_print(display, window_state, log, text_buffer);
		return r;
	} else {
		log_debug(log, "menu_gamecover_load: Loaded %dx%dx%dbpp\n", bmp.bmp->w, bmp.bmp->h, bmp.bmp->format->BitsPerPixel);
	}
	
	// Source
	src.window.x = 0;		// Top left corner of source bitmap
	src.window.y = 0;		// Top left corner of source bitmap
	src.window.w = BMP_W;	// Crop to our sizes defined in menu.h
	src.window.h = BMP_H;	// Crop to our sizes defined in menu.h

	// Destination
	dest.window.x = coords.x + 1; //(SCREEN_W - BMP_W); 	// Load the bitmap into the bordered window          
	dest.window.y = coords.y + 1;	// Load the bitmap into the bordered window 
	dest.window.w = coords.w - 2;  // Crop to our sizes defined in menu.h
	dest.window.h = coords.h - 2;	// Crop to our sizes defined in menu.h
	
	// Write bitmap to display
	r = gfxBlitBMP(log, &bmp, &src, display, &dest);
	if ( r != 0){
		log_error(log, "menu_gamecover_load Blit Error\n");
		gfxQuit(log);
		fclose(log);
		sleep(2);
		exit(-1);
	}
	gfxFreeBMP(log, &bmp);
	return r;
}

// Draw the category chooser (e.g. all, favourite, 0-9, a, b, c, d etc.)
// and highlight the current chosen category
int menu_category_populate(agnostic_bitmap *display, FILE *log, struct GAME_DATA *game_data, struct WINDOW_STATE *window_state){
	
	COORDS coords = ALPHABET_COORDS();	// Coordinates of window
	//int r = 0;							// return codes
	int i;								// loop counter
	int rev = 0;							// reverse video selector
	char c[2];
	c[1] = '\0';
	
	// X-offset of each category element
	int cat_all_x_offset = (FONT_W * 9) + 2;
	int cat_fav_x_offset = cat_all_x_offset + (FONT_W * 3) + 6;
	int cat_num_x_offset = cat_fav_x_offset + (FONT_W * 3) + 6;
	int cat_alpha_x_offset = cat_num_x_offset + (FONT_W * 3) + 6;
	
	log_debug(log, "============================\n");
	log_debug(log, "menu_category_populate: Elements at %d, %d, %d, %d\n", cat_all_x_offset, cat_fav_x_offset, cat_num_x_offset, cat_alpha_x_offset);	
	
	// Blank and redraw window
	menu_category_init(display, log);
	
	// Category text
	text2BMP(display, window_state->font_normal, window_state->font_reverse, log, "Category", (coords.x + 1), (coords.y + 1), 1);

	// Game category ALL
	if (window_state->category_window.cat_selected == -3){
		rev = 1;
	} else {
		rev = 0;
	}
	text2BMP(display, window_state->font_normal, window_state->font_reverse, log, "All", (coords.x + cat_all_x_offset), coords.y + 2, rev);

	// Game category Favourites
	if (window_state->category_window.cat_selected == -2){
		rev = 1;
	} else {
		rev = 0;
	}
	text2BMP(display, window_state->font_normal, window_state->font_reverse, log, "Fav", (coords.x + cat_fav_x_offset), coords.y + 2, rev);
	
	// Game category 0-9
	if (window_state->category_window.cat_selected == -1){
		rev = 1;
	} else {
		rev = 0;
	}
	text2BMP(display, window_state->font_normal, window_state->font_reverse, log, "0-9", (coords.x + cat_num_x_offset), coords.y + 2, rev);
	
	// Game category by first letter
	for(i = 0; i <= CATEGORY_MAX_CAT; i++){
		if (i == window_state->category_window.cat_selected){
			rev = 1;
		} else {
			rev = 0;
		}
		c[0] = ALPHABET_CATS[i];
		c[1] = '\0';
		text2BMP(display, window_state->font_normal, window_state->font_reverse, log, c, (coords.x + cat_alpha_x_offset + (i * (FONT_W + 1))), coords.y + 2, rev);
	}
	
	return 0;
	
}

// Draw the contents of the config/options window and process any input
// commands that were sent to it
int menu_config_populate(agnostic_bitmap *display, FILE *log, struct GAME_DATA *game_data, struct WINDOW_STATE *window_state){
	
	COORDS coords = CONFIG_COORDS();	// Coordinates of window
	FILE *csv;							// File handler for csv import/export
	int r;								// return codes
	
	// Blank and redraw window
	menu_config_init(display, log);
		
	// List config options
	text2BMP(display, window_state->font_normal, window_state->font_reverse, log, "Config Menu", 			(coords.x + 45), (coords.y + 3), 1);
	text2BMP(display, window_state->font_normal, window_state->font_reverse, log, " Esc - Close menu", 		(coords.x + 2), (coords.y + 3 + (2 * FONT_H)), 0);
	text2BMP(display, window_state->font_normal, window_state->font_reverse, log, " F1  - Export CSV", 		(coords.x + 2), (coords.y + 3 + (3 * FONT_H)), 0);
	text2BMP(display, window_state->font_normal, window_state->font_reverse, log, " F2  - Import CSV", 		(coords.x + 2), (coords.y + 3 + (4 * FONT_H)), 0);
	text2BMP(display, window_state->font_normal, window_state->font_reverse, log, " F3  - Rescan folders", 	(coords.x + 2), (coords.y + 3 + (5 * FONT_H)), 0);
	//text2BMP(display, window_state->font_normal, window_state->font_reverse, log, " F4  -", 				(coords.x + 2), (coords.y + 3 + (6 * FONT_H)), 0);
	//text2BMP(display, window_state->font_normal, window_state->font_reverse, log, " F5  -", 				(coords.x + 2), (coords.y + 3 + (7 * FONT_H)), 0);

	//==========================================
	// rescan folders
	if (window_state->config_window.config_option_selected == OPTION_RESCAN){		
		text2BMP(display, window_state->font_normal, window_state->font_reverse, log, " Rescan all folders...", (coords.x + 2), (coords.y + 3 + (10 * FONT_H)), 0);
		text2BMP(display, window_state->font_normal, window_state->font_reverse, log, " - Scanning, please wait", (coords.x + 2), (coords.y + 3 + (11 * FONT_H)), 0);
		gfxFlip(log, display);
		r = scangames(log, GAMEDIR, game_data);
		if (r > 0){
			sprintf(text_buffer, " - %d games found", r);
			text2BMP(display, window_state->font_normal, window_state->font_reverse, log, text_buffer , (coords.x + 2), (coords.y + 3 + (12 * FONT_H)), 0);
			text2BMP(display, window_state->font_normal, window_state->font_reverse, log, " - Sorting, please wait" , (coords.x + 2), (coords.y + 3 + (13 * FONT_H)), 0);
			gfxFlip(log, display);
			sortgames(game_data);
			menu_refilter_browser(log, game_data, window_state);
			// Select game list item 0
			window_state->browser_window.select_pos = 0;
		} else {
			text2BMP(display, window_state->font_normal, window_state->font_reverse, log, " - No games found" , (coords.x + 2), (coords.y + 3 + (12 * FONT_H)), 0);
		}
		text2BMP(display, window_state->font_normal, window_state->font_reverse, log, " Completed", (coords.x + 2), (coords.y + 3 + (14 * FONT_H)), 0);
		text2BMP(display, window_state->font_normal, window_state->font_reverse, log, " Press Esc to reload", (coords.x + 2), (coords.y + 3 + (18 * FONT_H)), 0);
		// If successful or not, cancel export option
		window_state->config_window.config_option_selected = OPTION_NONE;
	}
	
	//==========================================
	// CSV export
	if (window_state->config_window.config_option_selected == OPTION_CSV_EXPORT){
		text2BMP(display, window_state->font_normal, window_state->font_reverse, log, " Export data to CSV...", (coords.x + 2), (coords.y + 3 + (10 * FONT_H)), 0);
		// Export current game data
		// Open file
		csv = fopen(CSVFILE, "w");
		if (csv != NULL){
			text2BMP(display, window_state->font_normal, window_state->font_reverse, log, " - Writing, please wait", (coords.x + 2), (coords.y + 3 + (11 * FONT_H)), 0);
			gfxFlip(log, display);
			r = csv_writer(game_data, log, csv);
			if (r > 0){
				sprintf(text_buffer, " - %d records written", r);
				text2BMP(display, window_state->font_normal, window_state->font_reverse, log, text_buffer , (coords.x + 2), (coords.y + 3 + (12 * FONT_H)), 0);
			} else {
				text2BMP(display, window_state->font_normal, window_state->font_reverse, log, " - Error writing records" , (coords.x + 2), (coords.y + 3 + (12 * FONT_H)), 0);
			}
			// Close file
			fclose(csv);
			text2BMP(display, window_state->font_normal, window_state->font_reverse, log, " - Closed CSV file", (coords.x + 2), (coords.y + 3 + (13 * FONT_H)), 0);
			text2BMP(display, window_state->font_normal, window_state->font_reverse, log, " Completed", (coords.x + 2), (coords.y + 3 + (14 * FONT_H)), 0);
			
		} else {
			text2BMP(display, window_state->font_normal, window_state->font_reverse, log, " - Error opening CSV file", (coords.x + 2), (coords.y + 3 + (11 * FONT_H)), 0);
			text2BMP(display, window_state->font_normal, window_state->font_reverse, log, " Completed (error)", (coords.x + 2), (coords.y + 3 + (12 * FONT_H)), 0);
			text2BMP(display, window_state->font_normal, window_state->font_reverse, log, " CSV file may be corrupt", (coords.x + 2), (coords.y + 3 + (18 * FONT_H)), 0);
		}
		// If successful or not, cancel export option
		window_state->config_window.config_option_selected = OPTION_NONE;
	}
	
	//==========================================
	// CSV import
	if (window_state->config_window.config_option_selected == OPTION_CSV_IMPORT){
		text2BMP(display, window_state->font_normal, window_state->font_reverse, log, " Import data from CSV...", (coords.x + 2), (coords.y + 3 + (10 * FONT_H)), 0);
		// Import game data
		// Open file
		csv = fopen(CSVFILE, "r");
		if (csv != NULL){
			text2BMP(display, window_state->font_normal, window_state->font_reverse, log, " - Reading, please wait", (coords.x + 2), (coords.y + 3 + (11 * FONT_H)), 0);
			gfxFlip(log, display);
			r = csv_reader(game_data, log, csv);
			if (r > 0){
				sprintf(text_buffer, " - %d records read", r);
				text2BMP(display, window_state->font_normal, window_state->font_reverse, log, text_buffer , (coords.x + 2), (coords.y + 3 + (12 * FONT_H)), 0);
				// Select game list item 0
				window_state->browser_window.select_pos = 0;
				menu_refilter_browser(log, game_data, window_state);
			} else {
				text2BMP(display, window_state->font_normal, window_state->font_reverse, log, " - Error reading records" , (coords.x + 2), (coords.y + 3 + (12 * FONT_H)), 0);
			}
			// Close file
			fclose(csv);
			text2BMP(display, window_state->font_normal, window_state->font_reverse, log, " - Closed CSV file", (coords.x + 2), (coords.y + 3 + (13 * FONT_H)), 0);
			text2BMP(display, window_state->font_normal, window_state->font_reverse, log, " Completed", (coords.x + 2), (coords.y + 3 + (14 * FONT_H)), 0);
			text2BMP(display, window_state->font_normal, window_state->font_reverse, log, " Press Esc to reload", (coords.x + 2), (coords.y + 3 + (18 * FONT_H)), 0);
		
		} else {
			text2BMP(display, window_state->font_normal, window_state->font_reverse, log, " - Error opening CSV file", (coords.x + 2), (coords.y + 3 + (11 * FONT_H)), 0);
			text2BMP(display, window_state->font_normal, window_state->font_reverse, log, " Completed", (coords.x + 2), (coords.y + 3 + (12 * FONT_H)), 0);
			text2BMP(display, window_state->font_normal, window_state->font_reverse, log, " Game list NOT updated", (coords.x + 2), (coords.y + 3 + (18 * FONT_H)), 0);
		}		
		// If successful or not, cancel import option
		window_state->config_window.config_option_selected = OPTION_NONE;
	}	
	return 0;
}

int menu_gamecover_populate(agnostic_bitmap *display, FILE *log, struct GAME_DATA *game_data, struct WINDOW_STATE *window_state){

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
int menu_browser_populate(agnostic_bitmap *display, FILE *log, struct GAME_DATA *game_data, struct WINDOW_STATE *window_state){
	
	COORDS coords = BROWSER_COORDS();
	int i;
	int row = 0;
	int i_offset = 1;
	int select_i;
	int total_items;
	bool selected = 0;
	
	// Blank and redraw window borders
	menu_browser_init(display, log);
	
	text2BMP(display, window_state->font_normal, window_state->font_reverse, log, "        Browser        ", (coords.x), (coords.y + 1), 1);
	// Any games?
	
	log_debug(log, "============================\n");
	log_debug(log, "menu_browser_populate: select_pos %d\n", window_state->browser_window.select_pos);
	log_debug(log, "menu_browser_populate: start_pos %d\n", window_state->browser_window.start_pos);
	log_debug(log, "menu_browser_populate: end_pos %d\n", window_state->browser_window.end_pos);
	log_debug(log, "menu_browser_populate: start_pos_filtered %d\n", window_state->browser_window.start_pos_filtered);
	log_debug(log, "menu_browser_populate: end_pos_filtered %d\n", window_state->browser_window.end_pos_filtered);
	
	if (game_data->items > 0){

		if (window_state->category_window.cat_selected == CATEGORY_ALL){
			// We're in the category 'ALL'
			log_debug(log, "menu_browser_populate: Category All\n");
			
			total_items = game_data->items;
			// Is our game list longer than max number of rows?
			if (total_items >= (window_state->browser_window.max_lines - 1)){
				// More games than lines available - we can only show some of them
				
				if (window_state->browser_window.select_pos >= (window_state->browser_window.max_lines - 1)){
					log_debug(log, "more lines than printable. we're startin at %d [0-indexed]\n", window_state->browser_window.start_pos);
					select_i = window_state->browser_window.start_pos;
					for (i = 0; i < window_state->browser_window.max_lines; i++){
						if (select_i == window_state->browser_window.select_pos){
							log_debug(log, "menu_browser_populate: row: %d select_i: %d <- selected\n", i, select_i);
							selected = 1;
						} else {
							log_debug(log, "menu_browser_populate: row: %d select_i: %d\n", i, select_i);
							selected = 0;	
						}
						text2BMP(display, window_state->font_normal, window_state->font_reverse, log, game_data->game_data_items[select_i].name, (coords.x + 2), (coords.y + 3 + ((i + i_offset) * FONT_H)), selected);
						select_i++;
					}
					
					// Is select position outside of visible list?
					if (window_state->browser_window.select_pos >= (window_state->browser_window.end_pos - 1)){
						// Scroll down
						window_state->browser_window.start_pos++;
						window_state->browser_window.end_pos = window_state->browser_window.start_pos + window_state->browser_window.max_lines;
					} else if (window_state->browser_window.select_pos < window_state->browser_window.start_pos){
						// Scroll up
						window_state->browser_window.start_pos--;
						window_state->browser_window.end_pos--;
					}
				} else {
					log_debug(log, "menu_browser_populate: All selected lines can be shown - we're starting at 0\n");
					window_state->browser_window.start_pos = 0;
					window_state->browser_window.end_pos = window_state->browser_window.max_lines;
					for (i = 0; i < window_state->browser_window.max_lines; i++){
						if (i == window_state->browser_window.select_pos){
							selected = 1;
						} else {
							selected = 0;	
						}
						text2BMP(display, window_state->font_normal, window_state->font_reverse, log, game_data->game_data_items[i].name, (coords.x + 2), (coords.y + 3 + ((i + i_offset) * FONT_H)), selected);
					}
				}
				
			} else {
				// Less games than lines available - print them all			
				for (i = window_state->browser_window.start_pos; i <= window_state->browser_window.end_pos; i++){
					if (i == window_state->browser_window.select_pos){
						selected = 1;
					} else {
						selected = 0;	
					}
					text2BMP(display, window_state->font_normal, window_state->font_reverse, log, game_data->game_data_items[i].name, (coords.x + 2), (coords.y + 3 + ((i + i_offset) * FONT_H)), selected);
				}
			}
		} else {
			// We're in a filtered category and not 'ALL'
			log_debug(log, "menu_browser_populate: Filtered category\n");
			total_items = window_state->browser_window.end_pos_filtered - window_state->browser_window.start_pos_filtered;
			
			if (total_items >= (window_state->browser_window.max_lines - 1)){
				// We have more filtered items than can fit in the window
				
				if ((window_state->browser_window.end_pos_filtered  - window_state->browser_window.start_pos) >= (window_state->browser_window.max_lines - 1)){
					log_debug(log, "menu_browser_populate: Filtered category only %d lines can be shown, starting from item %d\n", window_state->browser_window.max_lines, window_state->browser_window.start_pos);
					select_i = window_state->browser_window.start_pos;
					for (i = 0; i < window_state->browser_window.max_lines; i++){
						if (select_i == window_state->browser_window.select_pos){
							log_debug(log, "menu_browser_populate: row: %d select_i: %d <- selected\n", i, select_i);
							selected = 1;
						} else {
							log_debug(log, "menu_browser_populate: row: %d select_i: %d\n", i, select_i);
							selected = 0;	
						}
						text2BMP(display, window_state->font_normal, window_state->font_reverse, log, game_data->game_data_items[select_i].name, (coords.x + 2), (coords.y + 3 + ((row + i_offset) * FONT_H)), selected);
						select_i++;
						row++;
					}
					
					// Is select position now outside of end of visible list?
					if (window_state->browser_window.select_pos >= (window_state->browser_window.end_pos - 1)){
						// Scroll down next time
						if (window_state->browser_window.end_pos < window_state->browser_window.end_pos_filtered){
							window_state->browser_window.start_pos++;
							window_state->browser_window.end_pos++;
						}
					} else if (window_state->browser_window.select_pos > window_state->browser_window.start_pos_filtered){
						// Scroll up next time
						if (window_state->browser_window.start_pos > window_state->browser_window.start_pos_filtered){
							window_state->browser_window.start_pos--;
							window_state->browser_window.end_pos--;
						}
					} else {
						// wha?	
					}
				} else {
					log_debug(log, "menu_browser_populate: All selected lines can be shown - we're starting at 0\n");
					window_state->browser_window.start_pos = 0;
					window_state->browser_window.end_pos = window_state->browser_window.max_lines;
					for (i = 0; i < window_state->browser_window.max_lines; i++){
						if (i == window_state->browser_window.select_pos){
							selected = 1;
						} else {
							selected = 0;	
						}
						text2BMP(display, window_state->font_normal, window_state->font_reverse, log, game_data->game_data_items[i].name, (coords.x + 2), (coords.y + 3 + ((row + i_offset) * FONT_H)), selected);
						row++;
					}
				}
				
			} else {
				// We can show all filtered items in the window
				log_debug(log, "menu_browser_populate: Filtered category all lines can be shown\n");
				if (window_state->browser_window.start_pos_filtered == -1){
					text2BMP(display, window_state->font_normal, window_state->font_reverse, log, "No entries", (coords.x + 2), (coords.y + 3 + (i_offset * FONT_H)), 0);	
				} else {
					for (i = window_state->browser_window.start_pos_filtered; i <= window_state->browser_window.end_pos_filtered; i++){
						if (i == window_state->browser_window.select_pos){
							selected = 1;
						} else {
							selected = 0;	
						}
						text2BMP(display, window_state->font_normal, window_state->font_reverse, log, game_data->game_data_items[i].name, (coords.x + 2), (coords.y + 3 + ((row + i_offset) * FONT_H)), selected);
						row++;
					}
				}
			}
		}
	} else {
		// No games found :(
		text2BMP(display, window_state->font_normal, window_state->font_reverse, log, "No entries", (coords.x + 2), (coords.y + 3 + (i_offset * FONT_H)), 0);
	}
	return 0;
}

// Open/close the textreader file handler
int menu_textreader_file(FILE *log, struct WINDOW_STATE *window_state, struct GAME_DATA *game_data, bool open_file){
	
	int pos = window_state->browser_window.select_pos;
	
	log_debug(log, "============================\n");
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
		log_debug(log, "menu_textreader_file: Open file [%s]\n", window_state->text_window.buffer);
		window_state->text_window.readme = fopen(window_state->text_window.buffer, "r");
		memset(window_state->text_window.buffer, '\0', sizeof(window_state->text_window.buffer));
		if (window_state->text_window.readme != NULL){
			log_debug(log, "menu_textreader_file: Opened\n");
			window_state->text_window.f_pos = 0;
			return 0;
		} else {
			log_debug(log, "menu_textreader_file: Error, file not open\n");
			return -1;
		}
	} else {
		// Close file pointer
		if (window_state->text_window.readme != NULL){
			fclose(window_state->text_window.readme);
			memset(window_state->text_window.buffer, '\0', sizeof(window_state->text_window.buffer));
			window_state->text_window.f_pos = 0;
			log_debug(log, "menu_textreader_file: Closed file\n");
			return 0;
		} else {
			log_debug(log, "menu_textreader_file: Attempt to close a file not already open!\n");
			return 0;
		}
	}
}

// Update the text reader window
int menu_textreader_populate(agnostic_bitmap *display, FILE *log, struct GAME_DATA *game_data, struct WINDOW_STATE *window_state){
	
	struct COORDS coords = READER_COORDS();								// Geometry of reader window
	int total_max_chars;		// Total number of printable chars (chars per line * lines)
	int last_i,i;				// Loop counter
	int f_size = 0;				// Size of the text file
	char c;						// Hold each char from the text buffer
	int total_printed_chars = 0;	// Total count of all chars printed so far. Must be < total_max_chars
	int printed_chars = 0;		// Count of chars printed so far for this line. Must be < window_state->text_window.max_chars
	int printed_lines = 0;		// Count of lines printed so far. Must be < window_state->text_window.max_lines
	char token_cr = '\n';		// Carriage return
	char token_lf = '\r';		// Carriage return + linefeed
	
	log_debug(log, "============================\n");
	window_state->text_window.max_lines = (coords.h / FONT_H) - 2;
	window_state->text_window.max_chars = (coords.w / FONT_W) - 2;
	total_max_chars = window_state->text_window.max_lines * window_state->text_window.max_chars;
	
	log_debug(log, "menu_textreader_populate: Reading file\n");
	fseek(window_state->text_window.readme, 0L, SEEK_END);
	f_size = ftell(window_state->text_window.readme);
	fseek(window_state->text_window.readme, 0L, SEEK_SET);
	fseek(window_state->text_window.readme, window_state->text_window.f_pos, SEEK_SET);
	log_debug(log, "menu_textreader_populate: File is %d bytes\n", f_size);
	log_debug(log, "menu_textreader_populate: Reading from byte %d\n", window_state->text_window.f_pos);
	
	if (f_size < sizeof(window_state->text_window.buffer)){
		// File is smaller than the available buffer, so read it all
		log_debug(log, "menu_textreader_populate: Reading entire file\n");
		i = fread(window_state->text_window.buffer, 1, f_size, window_state->text_window.readme);
		log_debug(log, "menu_textreader_populate: Read %d bytes\n", i);
	} else {
		// File is larger than the available buffer, so read a maximum of total_max_chars
		log_debug(log, "menu_textreader_populate: Reading partial file\n");
		for (i = 0; i < total_max_chars; i++){
			c = fgetc(window_state->text_window.readme);
			window_state->text_window.f_pos++;
			if (c == EOF){
				log_debug(log, "menu_textreader_populate: %d bytes read before EOF\n", i);
				break;
			} else {
				window_state->text_window.buffer[i] = c;
			}
		}
	}
	
	// If we're here, we either have the entire file in the buffer, or we've read, at most, total_max_chars.
	// We now need to split the buffer by end-of-line characters and ensure that we don't have more than
	// window_state->text_window.max_lines.
	
	// For up to max_chars in a line...
	printed_lines = 0;
	printed_chars = 0;
	last_i = 0;
	memset(text_buffer, '\0', 256);
	
	while (printed_lines <= window_state->text_window.max_lines){
		log_debug(log, "menu_textreader_populate: Parsing line %d, starting at pos %d\n", printed_lines, last_i);
		for (i = 0; i <= window_state->text_window.max_chars; i++){
			last_i ++;
			// Read next char
			c = window_state->text_window.buffer[last_i];
			if (c == token_cr){
				// newline
				// Print buffer and start new line
				// Print line buffer up to this point
				text2BMP(display, window_state->font_normal, window_state->font_reverse, log, text_buffer, coords.x + 2, (coords.y + 2 + (printed_lines * FONT_H)), 0);
				memset(text_buffer, '\0', 256);
				printed_lines++;
				break;
			} else if (c == token_lf){
				// linefeed - always comes after newline, so ignore
			} else {
				// plain character, add to buffer
				printed_chars++;
				total_printed_chars++;
				text_buffer[i] = c;
			}
			
			// Total number of visible lines reached - break inner loop
			if (printed_lines == window_state->text_window.max_lines){
				log_debug(log, "menu_textreader_populate: [Inner] Reached limit of %d lines\n", window_state->text_window.max_lines);
				// Save position and exit
				break;
			}
		}
		
		// Total number of visible lines reached - break outer loop
		if (printed_lines >= window_state->text_window.max_lines){
			log_debug(log, "menu_textreader_populate: [Before extra chars] Reached limit of %d lines\n", window_state->text_window.max_lines);
			// Save position and exit
			break;
		} else if (printed_lines < window_state->text_window.max_lines){
			log_debug(log, "menu_textreader_populate: Reached line %d chars limit\n", printed_lines);
			if (strlen(text_buffer) > 0){
				log_debug(log, "menu_textreader_populate: %zu characters left to print on line %d\n", strlen(text_buffer), printed_lines);
				text2BMP(display, window_state->font_normal, window_state->font_reverse, log, text_buffer, coords.x + 2, (coords.y + 2 + (printed_lines * FONT_H)), 0);
				memset(text_buffer, '\0', 256);
				printed_lines++;
			}
		}
		
		// Total number of visible lines reached - break outer loop
		if (printed_lines == window_state->text_window.max_lines){
			log_debug(log, "menu_textreader_populate: [After extra chars] Reached limit of %d lines\n", window_state->text_window.max_lines);
			// Save position and exit
			break;
		}
	}
	log_debug(log, "menu_textreader_populate: Final position @ byte %u\n", last_i);
	// Save state of file pointer so we know where to load next time around
	window_state->text_window.f_pos = last_i;
	return 0;
}

// The bottom info window - update it to reflect the current selected
// game, plus any additional details for the game.
int menu_info_populate(agnostic_bitmap *display, FILE *log, struct GAME_DATA *game_data, struct WINDOW_STATE *window_state){
	
	COORDS coords = INFO_COORDS();
	int game_id = window_state->browser_window.select_pos;		// short hand
	int last_game_id = window_state->browser_window.last_pos;	// short hand
	int rev = 0;												// Reverse video
	int x_offset = coords.x + 30;											// All text offset this min amount from lhs
	int binary_y_pos = coords.y + 2;
	int readme_y_pos = coords.y + 2 + (2 * FONT_H);
	int gamedata_y_pos = coords.y + 2 + (3 * FONT_H);
	int gamedata_y_pos2 = coords.y + 2 + (4 * FONT_H);
	
	// Blank and redraw window borders
	menu_info_init(display, log);
	
	if (game_id >= 0){
	
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
			text2BMP(display, window_state->font_normal, window_state->font_reverse, log, ">", 4, binary_y_pos, 0);
		}
		// binary_1
		if (game_data->game_data_items[game_id].binary_1[0] != '\0'){
			if (window_state->info_window.binary_selected == 1){
				rev = 1;
			} else {
				rev = 0;	
			}
			text2BMP(display, window_state->font_normal, window_state->font_reverse, log, game_data->game_data_items[game_id].binary_1, x_offset, binary_y_pos, rev);
		}
		// binary_2
		if (game_data->game_data_items[game_id].binary_2[0] != '\0'){
			if (window_state->info_window.binary_selected == 2){
				rev = 1;
			} else {
				rev = 0;	
			}
			text2BMP(display, window_state->font_normal, window_state->font_reverse, log, game_data->game_data_items[game_id].binary_2, (x_offset + ((16 * FONT_W) * 1)), binary_y_pos, rev);
		}
		// binary_3
		if (game_data->game_data_items[game_id].binary_3[0] != '\0'){
			if (window_state->info_window.binary_selected == 3){
				rev = 1;
			} else {
				rev = 0;	
			}
			text2BMP(display, window_state->font_normal, window_state->font_reverse, log, game_data->game_data_items[game_id].binary_3, (x_offset + ((16 * FONT_W) * 2)), binary_y_pos, rev);
		}
		
		//==========================================================
		// Draw all possible readme files
		// Is the 'choose binary' row currently selected?
		if (window_state->info_window.info_mode_selected == M_README){
			text2BMP(display, window_state->font_normal, window_state->font_reverse, log, ">", 4, readme_y_pos, 0);
		}
		// readme_1
		if (game_data->game_data_items[game_id].readme_1[0] != '\0'){
			if (window_state->info_window.readme_selected == 1){
				rev = 1;
			} else {
				rev = 0;	
			}
			text2BMP(display, window_state->font_normal, window_state->font_reverse, log, game_data->game_data_items[game_id].readme_1, x_offset, readme_y_pos, rev);
		}
		// readme_2
		if (game_data->game_data_items[game_id].readme_2[0] != '\0'){
			if (window_state->info_window.readme_selected == 2){
				rev = 1;
			} else {
				rev = 0;	
			}
			text2BMP(display, window_state->font_normal, window_state->font_reverse, log, game_data->game_data_items[game_id].readme_2, (x_offset + ((16 * FONT_W) * 1)), readme_y_pos, rev);
		}
		// readme_3
		if (game_data->game_data_items[game_id].readme_3[0] != '\0'){
			if (window_state->info_window.readme_selected == 3){
				rev = 1;
			} else {
				rev = 0;	
			}
			text2BMP(display, window_state->font_normal, window_state->font_reverse, log, game_data->game_data_items[game_id].readme_3, (x_offset + ((16 * FONT_W) * 2)), readme_y_pos, rev);
		}
		
		//====================================
		// game data - show the path to the game
		strcpy(text_buffer, "Path: ");
		strcat(text_buffer, game_data->game_data_items[game_id].path);
		text2BMP(display, window_state->font_normal, window_state->font_reverse, log, text_buffer, x_offset, gamedata_y_pos, 0);
		
		// Display counter of game_id selected from total number available
		// e.g. 13/254
		strcpy(text_buffer, "Game: ");
		sprintf(text_buffer_alt, "%d", (window_state->browser_window.select_pos + 1));
		strcat(text_buffer, text_buffer_alt);
		strcat(text_buffer, "/");
		sprintf(text_buffer_alt, "%d", game_data->items);
		strcat(text_buffer, text_buffer_alt);
		text2BMP(display, window_state->font_normal, window_state->font_reverse, log, text_buffer, x_offset, gamedata_y_pos2, 0);
		
		// Update last selected game to this one
		window_state->browser_window.last_pos = game_id;
	} else {
		menu_infobox_print(display, window_state, log, INFO_GAME_LIST_EMPTY);	
	}
	
	return 0;
}

// Toggle between selected windows
int menu_toggle_window(FILE *log, struct WINDOW_STATE *window_state){
	
	log_debug(log, "============================\n");
	if (window_state->selected_window == W_BROWSER){
		window_state->selected_window = W_INFO;
		log_debug(log, "menu_toggle_window: Info panel now selected\n");
		return 0;
	} else if (window_state->selected_window == W_INFO){
		window_state->selected_window = W_BROWSER;
		log_debug(log, "menu_toggle_window: Browser now selected\n");
		return 0;
	} else {
		log_debug(log, "menu_toggle_window: Error unknown window mode\n");
		return -1;
	}
}

// Send events to the config window
int menu_toggle_config_window_mode(agnostic_bitmap *display, FILE *log, struct GAME_DATA *game_data, struct WINDOW_STATE *window_state, SDL_Event event){
	
	log_debug(log, "============================\n");
	if (event.key.keysym.sym == SDLK_F1){
		// Export gamedata to CSV
		log_debug(log, "menu_toggle_config_window_mode: Export to CSV activated\n");
		window_state->config_window.config_option_selected = OPTION_CSV_EXPORT;
		return 0;	
	} else if (event.key.keysym.sym == SDLK_F2){
		// Import gamedata from CSV
		log_debug(log, "menu_toggle_config_window_mode: Import from  CSV activated\n");
		window_state->config_window.config_option_selected = OPTION_CSV_IMPORT;
		return 0;	
	} else if (event.key.keysym.sym == SDLK_F3){
		log_debug(log, "menu_toggle_config_window_mode: Rescan all game folders\n");
		window_state->config_window.config_option_selected = OPTION_RESCAN;
		return 0;
	} else {
		log_debug(log, "menu_toggle_config_window_mode: Default to no option selected\n");
		window_state->config_window.config_option_selected = OPTION_NONE;
		return 0;
	}
}

// Toggle rows and selected item in info window
int menu_toggle_info_window_mode(agnostic_bitmap *display, FILE *log, struct GAME_DATA *game_data, struct WINDOW_STATE *window_state, SDL_Event event){
	
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
				log_debug(log, "menu_toggle_info_window_mode: Open document %d\n", window_state->info_window.readme_selected);
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

// Scroll through category window
int menu_toggle_category(FILE *log, struct GAME_DATA *game_data, struct WINDOW_STATE *window_state, SDL_Event event){

	int changed;
	changed = 0;
	
	if (event.key.keysym.sym == SDLK_RIGHT){
		if (window_state->category_window.cat_selected < CATEGORY_MAX_CAT){
			window_state->category_window.cat_selected++;
			changed = 1;
		}
	}
	
	if (event.key.keysym.sym == SDLK_LEFT){
		if (window_state->category_window.cat_selected > CATEGORY_MIN_CAT){
			window_state->category_window.cat_selected--;
			changed = 1;			
		}
	}
	
	if (changed == 1){
		menu_refilter_browser(log, game_data, window_state);
	}
	return 0;
}

// Move selection down rows in browser window
int menu_toggle_browser_window(FILE *log, struct GAME_DATA *game_data, struct WINDOW_STATE *window_state, SDL_Event event){

	if (event.key.keysym.sym == SDLK_DOWN){
		if ((window_state->browser_window.select_pos < (game_data->items - 1)) && (window_state->browser_window.select_pos < window_state->browser_window.end_pos_filtered)){
			// Move down list
			window_state->browser_window.select_pos++;
		}
	}
	
	if (event.key.keysym.sym == SDLK_UP){
		if ((window_state->browser_window.select_pos > 0) && (window_state->browser_window.select_pos > window_state->browser_window.start_pos_filtered)){
			// Move up list
			window_state->browser_window.select_pos--;
			if (window_state->browser_window.select_pos <= window_state->browser_window.start_pos){
				window_state->browser_window.select_pos = window_state->browser_window.start_pos;	
			}
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
	log_debug(log, "============================\n");
	log_debug(log, "menu_init_gamedata: Current game pos: [%d]\n", game_data->pos);
	log_debug(log, "menu_init_gamedata: Current game items: [%d]\n", game_data->items);
	game_data->pos = 0;
	game_data->items = 0;
	log_debug(log, "menu_init_gamedata: Current game pos: [%d]\n", game_data->pos);
	log_debug(log, "menu_init_gamedata: Current game items: [%d]\n", game_data->items);
	
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
	window_state->browser_window.max_lines = ((BMP_H - 3) / FONT_H) - 1;
	
	// Unselect all info window choices
	window_state->info_window.binary_selected = 0;
	window_state->info_window.readme_selected = 0;
	
	// Set config option window choices
	window_state->config_window.config_option_selected = OPTION_NONE;
	
	// Category window 
	window_state->category_window.cat_selected = CATEGORY_DEFAULT_CAT;
	window_state->category_window.min_cat = CATEGORY_MIN_CAT;
	window_state->category_window.max_cat = CATEGORY_MAX_CAT;
	
	// Maximum number of lines we can show in text reader
	// Maximum number of characters in width we can show in text reader
	
	// Load fonts
	loadfont(log, (agnostic_bitmap *)&window_state->font_normal, 0);
	loadfont(log, (agnostic_bitmap *)&window_state->font_reverse, 1);
	return 0;
}