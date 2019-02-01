#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "menu.h"

// Standard application functions
#include "logging.h"
#include "errors.h"
#include "gfx.h"
#include "input.h"
#include "bmp2text.h"
#include "gamedata.h"
#include "csvlib.h"
#include "menulib.h"

// Main menu runtime
int main(int argc, char* argv[]){
	
	struct agnostic_event event;			// Input event data
	struct agnostic_bitmap screen;			// Our display screen bitmap
	struct GAME_DATA game_data;			// Global game data list
	struct WINDOW_STATE window_state;	// Global window state tracking
	
	int r = 0;					// return codes
	bool quit = 0;				// exit
	FILE *log;					// Log file
	
	log = fopen("MENU.LOG","w");

	log_info(log, "Menu tool running\n");
	log_info(log, "-----------------\n");

	// Init game data
	menu_init_gamedata(log, &game_data);
	
	// Init window data
	menu_init_windowstate(log, &window_state);
	
	// Load driver and set up video mode
	gfxInit(log);
	
	// Set screen.bmp mode for the screen
	r = gfxSetMode(log, &screen, SCREEN_W, SCREEN_H, SCREEN_BPP);
	if (r != 0){
		log_error(log, "Unable to initialise video driver!\n");
		gfxQuit(log);
		fclose(log);
		exit(-1);
	}
	
	// Init all windows
	menu_info_init(&screen, log);
	menu_category_init(&screen, log);
	menu_gamecover_init(&screen, log);
	menu_browser_init(&screen, log);
	menu_infobox_print(&screen, &window_state, log, INFO_GAMEDIR_WAIT);
	gfxFlip(log, &screen);
	
	// This is where we need to decide if we are going to
	// scan the games directory, load in a pre-built CSV file
	// or just allow the user to select those options themselves.
	
	// Option 1. Get initial list of games in a directory (slow)
	//r = scangames(log, GAMEDIR, &game_data);
	//window_state.browser_window.select_pos = 0;
	
	// Option 2. Load CSV data
	// csv = fopen();
	//r = csv_reader(&game_data, log, csv);
	//window_state.browser_window.select_pos
	// fclose(csv);
	
	// Option 3. Do nothing
	window_state.browser_window.select_pos = -1;
	r = 0;
	if (r < 0){
		menu_infobox_print(&screen, &window_state, log, ERROR_GAMEDIR_OPEN);	
	} else {
		menu_infobox_print(&screen, &window_state, log, INFO_GAMEDIR_SUCCESS);	
	}
	gfxFlip(log, &screen);
	
	// Fill the browser window once
	menu_browser_populate(&screen, log, &game_data, &window_state);
	//menu_gamecover_populate(&screen, log, &game_data, &window_state); // Disable until all backend methods can show graphics
	menu_category_populate(&screen, log, &game_data, &window_state);
	menu_info_populate(&screen, log, &game_data, &window_state);
	gfxFlip(log, &screen);
	
	// Main loop looking for user input
	log_debug(log, "[%s:%d]\t: (menu.c)\t: Entering event loop\n", __FILE__, __LINE__);
	while (quit == false){
		//While there's an event to handle
		while (inputPollEvent(&event)){
			//If the user has Xed out the window
			if (inputEventCheck(&event, EVENT_QUIT)){
				//Quit the program
				log_debug(log, "Quit selected\n");
			} 
			// Check for keypresses
			if (inputEventCheck(&event, EVENT_KEYDOWN)){
				log_debug(log, "[%s:%d]\t: (menu.c)\t: Processing key\n", __FILE__, __LINE__);
				switch(inputEventKeypress(&event)){
					case KEY_q:
						// Close textreader if open
						if (window_state.selected_window == W_TEXT){
							menu_textreader_file(log, &window_state, &game_data, 0);
						}
						quit = true;
						break;
					case KEY_c:
						if (window_state.selected_window != W_CONFIG){
							// Open config window
							menu_toggle_config_window_mode(&screen, log, &game_data, &window_state, &event);
							menu_config_populate(&screen, log, &game_data, &window_state);
							window_state.selected_window = W_CONFIG;
						}
						break;
					case KEY_F1:
						if (window_state.selected_window == W_CONFIG){
							// Send F1 to config window
							menu_toggle_config_window_mode(&screen, log, &game_data, &window_state, &event);
							menu_config_populate(&screen, log, &game_data, &window_state);
						}
						break;
					case KEY_F2:
						if (window_state.selected_window == W_CONFIG){
							// Send F2 to config window
							menu_toggle_config_window_mode(&screen, log, &game_data, &window_state, &event);
							menu_config_populate(&screen, log, &game_data, &window_state);
						}
						break;
					case KEY_F3:
						if (window_state.selected_window == W_CONFIG){
							// Send F3 to config window
							menu_toggle_config_window_mode(&screen, log, &game_data, &window_state, &event);
							menu_config_populate(&screen, log, &game_data, &window_state);
						}
						break;
					case KEY_F4:
						if (window_state.selected_window == W_CONFIG){
							// Send F4 to config window
							menu_toggle_config_window_mode(&screen, log, &game_data, &window_state, &event);
							menu_config_populate(&screen, log, &game_data, &window_state);
						}
						break;
					case KEY_F5:
						if (window_state.selected_window == W_CONFIG){
							// Send F5 to config window
							menu_toggle_config_window_mode(&screen, log, &game_data, &window_state, &event);
							menu_config_populate(&screen, log, &game_data, &window_state);
						}
						break;
					case KEY_UP: 
						if (window_state.selected_window == W_BROWSER){
							// scroll up through browser list
							menu_toggle_browser_window(log, &game_data, &window_state, &event);
							menu_browser_populate(&screen, log, &game_data, &window_state);
							menu_info_populate(&screen, log, &game_data, &window_state);
							menu_gamecover_populate(&screen, log, &game_data, &window_state);
						} else if (window_state.selected_window == W_INFO){
							// scroll up between binary/readme lines
							menu_toggle_info_window_mode(&screen, log, &game_data, &window_state, &event);
							menu_info_populate(&screen, log, &game_data, &window_state);
						} else if (window_state.selected_window == W_TEXT){
							// scroll up text window
						} else {
							// no-nop	
						}
						break;
					case KEY_DOWN: 
						if (window_state.selected_window == W_BROWSER){
							// scroll down through browser list
							menu_toggle_browser_window(log, &game_data, &window_state, &event);
							menu_browser_populate(&screen, log, &game_data, &window_state);
							menu_info_populate(&screen, log, &game_data, &window_state);
							menu_gamecover_populate(&screen, log, &game_data, &window_state); 
						} else if (window_state.selected_window == W_INFO){
							// scroll down between binary/readme lines
							menu_toggle_info_window_mode(&screen, log, &game_data, &window_state, &event);
							menu_info_populate(&screen, log, &game_data, &window_state);
						} else if (window_state.selected_window == W_TEXT){
							// scroll down text window
						} else {
							// no-nop	
						}
						break;
					case KEY_LEFT: 
						if (window_state.selected_window == W_BROWSER){
							// Toggle between game categories
							menu_toggle_category(log, &game_data, &window_state, &event);
							menu_category_populate(&screen, log, &game_data, &window_state);
							menu_browser_populate(&screen, log, &game_data, &window_state);
							//menu_gamecover_populate(&screen, log, &game_data, &window_state);
							
						} else if (window_state.selected_window == W_INFO){
							// Scroll left between binary/readme lines
							menu_toggle_info_window_mode(&screen, log, &game_data, &window_state, &event);
							menu_info_populate(&screen, log, &game_data, &window_state);
						}
						break;
					case KEY_RIGHT:
						if (window_state.selected_window == W_BROWSER){
							// Toggle between game categories
							menu_toggle_category(log, &game_data, &window_state, &event);
							menu_category_populate(&screen, log, &game_data, &window_state);
							menu_browser_populate(&screen, log, &game_data, &window_state);
							//menu_gamecover_populate(&screen, log, &game_data, &window_state);
							
						} else if (window_state.selected_window == W_INFO){
							// Scroll right between binary/readme lines
							menu_toggle_info_window_mode(&screen, log, &game_data, &window_state, &event);
							menu_info_populate(&screen, log, &game_data, &window_state);
						}
						break;
					case KEY_TAB:
						// Swap between browser and info windows
						if ((window_state.selected_window == W_INFO) || (window_state.selected_window == W_BROWSER)){
							menu_toggle_window(log, &window_state);
						}
						break;
					case KEY_RETURN:
						// Choose highlighted option of info window
						if (window_state.selected_window == W_INFO){
							menu_toggle_info_window_mode(&screen, log, &game_data, &window_state, &event);
						}
						break;
					case KEY_ESCAPE:
						// Close text reader from info window
						if (window_state.selected_window == W_TEXT){
							menu_browser_populate(&screen, log, &game_data, &window_state);
							//menu_gamecover_populate(&screen, log, &game_data, &window_state);
							menu_category_populate(&screen, log, &game_data, &window_state);
							menu_info_populate(&screen, log, &game_data, &window_state);
							window_state.selected_window = W_INFO;
						}
						
						// Close config window
						if (window_state.selected_window == W_CONFIG){
							// Disable any config mode
							window_state.config_window.config_option_selected = OPTION_NONE;
							menu_browser_populate(&screen, log, &game_data, &window_state);
							//menu_gamecover_populate(&screen, log, &game_data, &window_state);
							menu_category_populate(&screen, log, &game_data, &window_state);
							menu_info_populate(&screen, log, &game_data, &window_state);
							window_state.selected_window = W_BROWSER;
						}
						break;
					default:
						break;
				}
				// Refresh all windows				
				gfxFlip(log, &screen);
				
				// Flush any log message
#ifdef USE_LOGGING
				fflush(log);
#endif
			}
		}
	}	
	// Tidy up gfx driver before closing
	gfxFreeBMP(log, &screen);
	gfxQuit(log);
	
	log_info(log, "Exiting\n");
	fclose(log);
	return 0;
}
