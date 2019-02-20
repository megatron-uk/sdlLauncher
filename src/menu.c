#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "menu.h"

// Standard application functions
#include "misc/logging.h"
#include "misc/errors.h"
#include "gfx/gfx.h"
#include "input/input.h"
#include "image/bmp2text.h"
#include "data/gamedata.h"
#include "data/csvlib.h"
#include "data/config.h"
#include "menulib.h"

// Global namespace configuration object
struct config_object menu_cfg;

// Main menu runtime
int main(int argc, char* argv[]){
	
	struct agnostic_event event;			// Input event data
	struct agnostic_bitmap display;			// Our display screen bitmap
	struct GAME_DATA game_data;			// Global game data list
	struct WINDOW_STATE window_state;	// Global window state tracking
	
	int r = 0;					// return codes
	int r_tmp;
	int i;
	bool quit = 0;				// exit
	FILE *log;					// Log file
	
	log = fopen("MENU.LOG","w");

	log_info(log, "[%s:%d]\t: (main)\t: Menu tool running\n", __FILE__, __LINE__);
	log_info(log, "[%s:%d]\t: (main)\t: -----------------\n", __FILE__, __LINE__);
	
	// Load config file data
	config(log, &menu_cfg);
	
	// Load driver and set up video mode
	gfxInit(log);
	
	// Init game data
	menuInitGamedata(log, &game_data);
	
	// Init window data
	menuInitWindowstate(log, &window_state);
	
	// Set screen.bmp mode for the screen
	r = gfxSetMode(log, &display, MENU_SCREEN_W, MENU_SCREEN_H, MENU_SCREEN_BPP);
	if (r != 0){
		log_error(log, "[%s:%d]\t: (main)\t: Unable to initialise video driver!\n");
		log_error(log, "[%s:%d]\t: (main)\t: Application closing\n", __FILE__, __LINE__);
		gfxQuit(log);
		fclose(log);
		exit(-1);
	}
	
	// Init all windows
	menuInfoInit(&display, log);
	menuCategoryInit(&display, log);
	menuGamecoverInit(&display, log);
	menuBrowserInit(&display, log);
	menuInfoboxPrint(&display, &window_state, log, INFO_GAMEDIR_WAIT);
	gfxFlip(log, &display);
	
	// This is where we need to decide if we are going to
	// scan the games directory, load in a pre-built CSV file
	// or just allow the user to select those options themselves.
	
	// Option 1. Get initial list of games in a directory (slow)
	for (i=0; i<menu_cfg.gamedir_count; i++){
		r_tmp = scangames(log, menu_cfg.gamedirs[i], &game_data);
		r += r_tmp;
	}
	// Sort list
	if (r > 0){
		sortgames(&game_data);
	}
	
	// Option 2. Load CSV data
	// csv = fopen();
	//r = csv_reader(&game_data, log, csv);
	//window_state.browser_window.select_pos
	// fclose(csv);
	
	// Option 3. Do nothing
	//window_state.browser_window.select_pos = -1;
	
	if (r < 0){
		window_state.browser_window.select_pos = -1;
		menuInfoboxPrint(&display, &window_state, log, ERROR_GAMEDIR_OPEN);	
	} else {
		window_state.browser_window.select_pos = 0;
		menuInfoboxPrint(&display, &window_state, log, INFO_GAMEDIR_SUCCESS);	
	}
	gfxFlip(log, &display);
	
	// Fill the browser window once
	menuBrowserPopulate(&display, log, &game_data, &window_state);
	menuGamecoverPopulate(&display, log, &game_data, &window_state);
	menuCategoryPopulate(&display, log, &game_data, &window_state);
	menuInfoPopulate(&display, log, &game_data, &window_state);
	menuHelpTextPopulate(log, &display, &window_state);
	gfxFlip(log, &display);
	
	// Main loop looking for user input
	log_debug(log, "[%s:%d]\t: (main)\t: Entering event loop\n", __FILE__, __LINE__);
	quit = false;
	while (quit != true){
		
		// Get an input event
		inputPollEvent(&event);
		
		//If the user has Xed out the window
		//if (inputEventCheck(&event, EVENT_QUIT)){
			//Quit the program
			//log_debug(log, "Quit selected\n");
			//quit = true;
		//} 
		
		// Check for keypresses
		if (inputEventCheck(&event, EVENT_KEYDOWN)){
			log_debug(log, "[%s:%d]\t: (main)\t: Processing key\n", __FILE__, __LINE__);
			
			// Check the actual key pressed
			switch(inputEventKeypress(&event)){
				case KEY_q:
					// Close textreader if open
					if (window_state.selected_window == W_TEXT){
						menuTextreaderFile(log, &window_state, &game_data, 0);
					}
					quit = true;
					break;
				case KEY_c:
					if (window_state.selected_window != W_CONFIG){
						// Open config window
						menuToggleConfigWindowMode(&display, log, &game_data, &window_state, &event);
						menuConfigPopulate(&display, log, &game_data, &window_state);
						window_state.selected_window = W_CONFIG;
					}
					break;
				case KEY_F1:
					if (window_state.selected_window == W_CONFIG){
						// Send F1 to config window
						menuToggleConfigWindowMode(&display, log, &game_data, &window_state, &event);
						menuConfigPopulate(&display, log, &game_data, &window_state);
					}
					break;
				case KEY_F2:
					if (window_state.selected_window == W_CONFIG){
						// Send F2 to config window
						menuToggleConfigWindowMode(&display, log, &game_data, &window_state, &event);
						menuConfigPopulate(&display, log, &game_data, &window_state);
					}
					break;
				case KEY_F3:
					if (window_state.selected_window == W_CONFIG){
						// Send F3 to config window
						menuToggleConfigWindowMode(&display, log, &game_data, &window_state, &event);
						menuConfigPopulate(&display, log, &game_data, &window_state);
					}
					break;
				case KEY_F4:
					if (window_state.selected_window == W_CONFIG){
						// Send F4 to config window
						menuToggleConfigWindowMode(&display, log, &game_data, &window_state, &event);
						menuConfigPopulate(&display, log, &game_data, &window_state);
					}
					break;
				case KEY_F5:
					if (window_state.selected_window == W_CONFIG){
						// Send F5 to config window
						menuToggleConfigWindowMode(&display, log, &game_data, &window_state, &event);
						menuConfigPopulate(&display, log, &game_data, &window_state);
					}
					break;
				case KEY_UP: 
					if (window_state.selected_window == W_BROWSER){
						// scroll up through browser list
						menuToggleBrowserWindow(log, &game_data, &window_state, &event);
						menuBrowserPopulate(&display, log, &game_data, &window_state);
						menuInfoPopulate(&display, log, &game_data, &window_state);
						menuGamecoverPopulate(&display, log, &game_data, &window_state);
					} else if (window_state.selected_window == W_INFO){
						// scroll up between binary/readme lines
						menuToggleInfoWindowMode(&display, log, &game_data, &window_state, &event);
						menuInfoPopulate(&display, log, &game_data, &window_state);
					} else if (window_state.selected_window == W_TEXT){
						// scroll up text window
					} else {
						// no-nop	
					}
					break;
				case KEY_DOWN: 
					if (window_state.selected_window == W_BROWSER){
						// scroll down through browser list
						menuToggleBrowserWindow(log, &game_data, &window_state, &event);
						menuBrowserPopulate(&display, log, &game_data, &window_state);
						menuInfoPopulate(&display, log, &game_data, &window_state);
						menuGamecoverPopulate(&display, log, &game_data, &window_state); 
					} else if (window_state.selected_window == W_INFO){
						// scroll down between binary/readme lines
						menuToggleInfoWindowMode(&display, log, &game_data, &window_state, &event);
						menuInfoPopulate(&display, log, &game_data, &window_state);
					} else if (window_state.selected_window == W_TEXT){
						// scroll down text window
					} else {
						// no-nop	
					}
					break;
				case KEY_LEFT: 
					if (window_state.selected_window == W_BROWSER){
						// Toggle between game categories
						menuToggleCategory(log, &game_data, &window_state, &event);
						menuCategoryPopulate(&display, log, &game_data, &window_state);
						menuBrowserPopulate(&display, log, &game_data, &window_state);
						menuInfoPopulate(&display, log, &game_data, &window_state);
						menuGamecoverPopulate(&display, log, &game_data, &window_state);
						
					} else if (window_state.selected_window == W_INFO){
						// Scroll left between binary/readme lines
						menuToggleInfoWindowMode(&display, log, &game_data, &window_state, &event);
						menuInfoPopulate(&display, log, &game_data, &window_state);
					}
					break;
				case KEY_RIGHT:
					if (window_state.selected_window == W_BROWSER){
						// Toggle between game categories
						menuToggleCategory(log, &game_data, &window_state, &event);
						menuCategoryPopulate(&display, log, &game_data, &window_state);
						menuBrowserPopulate(&display, log, &game_data, &window_state);
						menuInfoPopulate(&display, log, &game_data, &window_state);
						menuGamecoverPopulate(&display, log, &game_data, &window_state);
						
					} else if (window_state.selected_window == W_INFO){
						// Scroll right between binary/readme lines
						menuToggleInfoWindowMode(&display, log, &game_data, &window_state, &event);
						menuInfoPopulate(&display, log, &game_data, &window_state);
					}
					break;
				case KEY_TAB:
					// Swap between browser and info windows
					if ((window_state.selected_window == W_INFO) || (window_state.selected_window == W_BROWSER)){
						menuToggleWindow(log, &window_state);
					}
					break;
				case KEY_RETURN:
					// Choose highlighted option of info window
					if (window_state.selected_window == W_INFO){
						menuToggleInfoWindowMode(&display, log, &game_data, &window_state, &event);
					}
					break;
				case KEY_ESCAPE:
					// Close text reader from info window
					if (window_state.selected_window == W_TEXT){
						menuBrowserPopulate(&display, log, &game_data, &window_state);
						menuCategoryPopulate(&display, log, &game_data, &window_state);
						menuInfoPopulate(&display, log, &game_data, &window_state);
						menuGamecoverPopulate(&display, log, &game_data, &window_state);
						window_state.selected_window = W_INFO;
					}
					
					// Close config window
					if (window_state.selected_window == W_CONFIG){
						// Disable any config mode
						window_state.config_window.config_option_selected = OPTION_NONE;
						menuBrowserPopulate(&display, log, &game_data, &window_state);
						menuCategoryPopulate(&display, log, &game_data, &window_state);
						menuInfoPopulate(&display, log, &game_data, &window_state);
						menuGamecoverPopulate(&display, log, &game_data, &window_state);
						window_state.selected_window = W_BROWSER;
					}
					break;
				default:
					break;
			}
			// Refresh all windows
			menuHelpTextPopulate(log, &display, &window_state);
			gfxFlip(log, &display);			
			inputSleep();
			
			// Flush any log message
#ifdef USE_LOGGING
			//fflush(log);
#endif
		}
	}	
	// Tidy up gfx driver before closing
	gfxFreeBMP(log, &display);
	gfxQuit(log);
	
	log_info(log, "[%s:%d]\t: (main)\t: Application closing\n", __FILE__, __LINE__);
	fclose(log);
	return 0;
}

#ifdef USE_ALLEGRO
END_OF_MAIN()
#endif
