#include "../menu.h"

// Configuration data structure
typedef struct config_object {
	
	// Filename section
	char *csv_filename;
	char *gamedirs[MAX_GAMEDIRS];
	int gamedir_count;
} config_object;

int config(FILE *log, struct config_object *cfg);