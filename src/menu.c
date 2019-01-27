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
#include "csvlib.h"
#include "menulib.h"

// Global variable to hold SDL events
SDL_Event event;

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
		if (LOGGING){
			fprintf(log, "SDL Driver %s\n", SDL_VideoDriverName(vdriver, 32));
			fprintf(log, "SDL Driver mode %dx%dx%dbpp\n", display->w, display->h, display->format->BitsPerPixel);
			fprintf(log, "SDL Driver has HW Surfaces? (%d)\n", SDL_GetVideoSurface()->flags & SDL_HWSURFACE);
			fflush(log);
		}
	}
	
	// Init all windows
	menu_info_init(display, log);
	menu_category_init(display, log);
	menu_gamecover_init(display, log);
	menu_browser_init(display, log);
	menu_infobox_print(display, &window_state, log, INFO_GAMEDIR_WAIT);
	SDL_Flip(display);
	
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
		menu_infobox_print(display, &window_state, log, ERROR_GAMEDIR_OPEN);	
	} else {
		menu_infobox_print(display, &window_state, log, INFO_GAMEDIR_SUCCESS);	
	}
	SDL_Flip(display);
	
	// Fill the browser window once
	menu_browser_populate(display, log, &game_data, &window_state);
	menu_gamecover_populate(display, log, &game_data, &window_state);
	menu_category_populate(display, log, &game_data, &window_state);
	menu_info_populate(display, log, &game_data, &window_state);
	SDL_Flip(display);
		
	// Main loop looking for user input
	while ( quit == false ){
		//While there's an event to handle
		while (SDL_PollEvent(&event)){
			//If the user has Xed out the window
			if (event.type == SDL_QUIT){
				//Quit the program
				if (LOGGING){
					fprintf(log, "Quit selected\n");
					fflush(log);
				}
			} 
			// Check for keypresses
			if (event.type == SDL_KEYDOWN){
				switch(event.key.keysym.sym){
					case SDLK_q:
						// Close textreader if open
						if (window_state.selected_window == W_TEXT){
							menu_textreader_file(log, &window_state, &game_data, 0);
						}
						quit = 1;
						break;
					case SDLK_c:
						if (window_state.selected_window != W_CONFIG){
							// Open config window
							menu_toggle_config_window_mode(display, log, &game_data, &window_state, event);
							menu_config_populate(display, log, &game_data, &window_state);
							window_state.selected_window = W_CONFIG;
						}
						break;
					case SDLK_F1:
						if (window_state.selected_window == W_CONFIG){
							// Send F1 to config window
							menu_toggle_config_window_mode(display, log, &game_data, &window_state, event);
							menu_config_populate(display, log, &game_data, &window_state);
						}
						break;
					case SDLK_F2:
						if (window_state.selected_window == W_CONFIG){
							// Send F2 to config window
							menu_toggle_config_window_mode(display, log, &game_data, &window_state, event);
							menu_config_populate(display, log, &game_data, &window_state);
						}
						break;
					case SDLK_F3:
						if (window_state.selected_window == W_CONFIG){
							// Send F3 to config window
							menu_toggle_config_window_mode(display, log, &game_data, &window_state, event);
							menu_config_populate(display, log, &game_data, &window_state);
						}
						break;
					case SDLK_F4:
						if (window_state.selected_window == W_CONFIG){
							// Send F4 to config window
							menu_toggle_config_window_mode(display, log, &game_data, &window_state, event);
							menu_config_populate(display, log, &game_data, &window_state);
						}
						break;
					case SDLK_F5:
						if (window_state.selected_window == W_CONFIG){
							// Send F5 to config window
							menu_toggle_config_window_mode(display, log, &game_data, &window_state, event);
							menu_config_populate(display, log, &game_data, &window_state);
						}
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
						if (window_state.selected_window == W_BROWSER){
							// Toggle between game categories
							menu_toggle_category(log, &game_data, &window_state, event);
							menu_category_populate(display, log, &game_data, &window_state);
							menu_browser_populate(display, log, &game_data, &window_state);
							menu_gamecover_populate(display, log, &game_data, &window_state);
							
						} else if (window_state.selected_window == W_INFO){
							// Scroll left between binary/readme lines
							menu_toggle_info_window_mode(display, log, &game_data, &window_state, event);
							menu_info_populate(display, log, &game_data, &window_state);
						}
						break;
					case SDLK_RIGHT:
						if (window_state.selected_window == W_BROWSER){
							// Toggle between game categories
							menu_toggle_category(log, &game_data, &window_state, event);
							menu_category_populate(display, log, &game_data, &window_state);
							menu_browser_populate(display, log, &game_data, &window_state);
							menu_gamecover_populate(display, log, &game_data, &window_state);
							
						} else if (window_state.selected_window == W_INFO){
							// Scroll right between binary/readme lines
							menu_toggle_info_window_mode(display, log, &game_data, &window_state, event);
							menu_info_populate(display, log, &game_data, &window_state);
						}
						break;
					case SDLK_TAB:
						// Swap between browser and info windows
						if ((window_state.selected_window == W_INFO) || (window_state.selected_window == W_BROWSER)){
							menu_toggle_window(log, &window_state);
						}
						break;
					case SDLK_RETURN:
						// Choose highlighted option of info window
						if (window_state.selected_window == W_INFO){
							menu_toggle_info_window_mode(display, log, &game_data, &window_state, event);
						}
						break;
					case SDLK_ESCAPE:
						// Close text reader from info window
						if (window_state.selected_window == W_TEXT){
							menu_browser_populate(display, log, &game_data, &window_state);
							menu_gamecover_populate(display, log, &game_data, &window_state);
							menu_category_populate(display, log, &game_data, &window_state);
							menu_info_populate(display, log, &game_data, &window_state);
							window_state.selected_window = W_INFO;
						}
						
						// Close config window
						if (window_state.selected_window == W_CONFIG){
							// Disable any config mode
							window_state.config_window.config_option_selected = OPTION_NONE;
							menu_browser_populate(display, log, &game_data, &window_state);
							menu_gamecover_populate(display, log, &game_data, &window_state);
							menu_category_populate(display, log, &game_data, &window_state);
							menu_info_populate(display, log, &game_data, &window_state);
							window_state.selected_window = W_BROWSER;
						}
						break;
					default:
						break;
				}
				// Refresh all windows				
				SDL_Flip(display);
				
				// Flush any log message
				if (LOGGING){
					fflush(log);
				}
			}
		}
		SDL_Delay(100);
	}	
	// Tidy up SDL before closing
	SDL_Delay(500);
	SDL_FreeSurface(display);
	SDL_Quit();
	
	if (LOGGING){
		fprintf(log, "Exiting\n");
	}
	fclose(log);
	return 0;
}
