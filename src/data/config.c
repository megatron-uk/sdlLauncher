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
	
	if (MATCH("paths", "csv")) {
		// CSV filename
		pconfig->csv_filename = strdup(value);
	} else if (MATCH("paths", "gamedirs")) {
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
		
	} else {
		return 0;  // unknown section/name, error
	}
	return 1;
}

// Parse config data from ini file, or load from defaults
int config(FILE *log, struct config_object *cfg){

	int r = 0;
	
	r = ini_parse(INI_FILE, config_handler, cfg);
	if (r < 0){
		log_warn(log, "[%s:%d]\t: (config)\t: Configuration file not found - using defaults\n", __FILE__, __LINE__);
	
		// CSV file
		cfg->csv_filename = CSV_FILE;
	
		// Game dirs
		cfg->gamedirs[0] = GAMEDIR;	
		cfg->gamedir_count = 1;
		return r;
	} else {	
		log_info(log, "[%s:%d]\t: (config)\t: Configuration file loaded\n", __FILE__, __LINE__);
		return 0;
	}
}