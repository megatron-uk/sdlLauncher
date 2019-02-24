#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../menu.h"
#include "../misc/logging.h"
#include "config.h"
#include "ini.h"

static int config_handler(void* user, const char* section, const char* name, const char* value){
	config_object* pconfig = (config_object*)user;
	
	int i = 0;
	char delim[] = ",";
	char buf[INI_MAX_LINE];
	
	#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
	
	// Path settings
	if (MATCH("paths", "csv")) {
		// CSV filename
		pconfig->csv_filename = strdup(value);
	}
	
	if (MATCH("paths", "gamedirs")) {
		
		// Split gamedirs string on comma...
		// c:\games1,c:\adventure,d:\fps
		strncpy(buf, value, strlen(value));
		char *ptr = strtok(buf, delim);
		while(ptr != NULL){
			pconfig->gamedirs[i] = strdup(ptr);
			ptr = strtok(NULL, delim);
			i++;
		}
		pconfig->gamedir_count = i;
	}
	
	// Video settings
	if (MATCH("video", "mode")){
		
		// Check video mode (certain platforms only)
		if (strcmp(value, "low") == 0){
			pconfig->menu_screen_w = MENU_SCREEN_W;
			pconfig->menu_screen_h = MENU_SCREEN_H;
		} else if (strcmp(value, "med") == 0){
			pconfig->menu_screen_w = MENU_SCREEN_W_MED;
			pconfig->menu_screen_h = MENU_SCREEN_H_MED;
		} else if (strcmp(value, "high") == 0){
			pconfig->menu_screen_w = MENU_SCREEN_W_HI;
			pconfig->menu_screen_h = MENU_SCREEN_H_HI;
		} else {
			pconfig->menu_screen_w = MENU_SCREEN_W;
			pconfig->menu_screen_h = MENU_SCREEN_H;
		}
		
		// Recalculate bitmap sizes
		pconfig->bmp_w = (pconfig->menu_screen_w / 5) * 3;
		pconfig->bmp_h = (pconfig->menu_screen_h / 4) * 3;
	}
	
	return 1;
}

// Load default config values
int config_defaults(FILE* log, struct config_object *cfg){
	
	// Paths gamedirs
	cfg->gamedirs[0] = GAMEDIR;
	cfg->gamedir_count = 1;
	
	// Paths csv
	cfg->csv_filename = CSV_FILE;
	
	// Video mode
	cfg->menu_screen_w = MENU_SCREEN_W;
	cfg->menu_screen_h = MENU_SCREEN_H;
	
	// Recalculate bitmap sizes
	cfg->bmp_w = (cfg->menu_screen_w / 5) * 3;
	cfg->bmp_h = (cfg->menu_screen_h / 4) * 3;
}

// Parse config data from ini file, or load from defaults
int config(FILE *log, struct config_object *cfg){

	int r = 0;
	
	// Load defaults
	config_defaults(log, cfg);
	
	// Load values from config file
	r = ini_parse(INI_FILE, config_handler, cfg);
	if (r < 0){
		log_warn(log, "[%s:%d]\t: (config)\t: Configuration file not found - using defaults\n", __FILE__, __LINE__);
		return r;
	} else {	
		log_info(log, "[%s:%d]\t: (config)\t: Configuration file loaded\n", __FILE__, __LINE__);
		return 0;
	}
}