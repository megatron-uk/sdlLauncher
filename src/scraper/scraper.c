#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/types.h>

#ifdef AMIGAOS
#include <glob.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#ifdef TOS
#include <unistd.h>
#include <sys/stat.h>
#endif

#include "../menu.h"
#include "../misc/logging.h"

// ==============================================
//
// This probably needs splitting into game data
// scrapers that are distinct to each platform...
//
// ==============================================

#ifdef DOS
#include "scraper_dos.c"
#endif

#ifdef TOS
#include "scraper_atari.c"
#endif

#ifdef AMIGAOS
#include "scraper_amiga.c"
#endif

#ifdef LINUX
#include "scraper_linux.c"
#endif

// ==============================================
// 
// Common functions for all platforms
//
// ==============================================

// Return index position of a single game given its name
int get_gamedata(FILE *log, char *gamename, struct GAME_DATA *game_data){
	
	int i;
	for (i = 0; i<game_data->items; i++){
		if (strcmp(game_data->game_data_items[i].name, gamename) == 0){
			return i;
		}
	}
	return -1;
}

// Compare two game data items and return the id
// of the game which has the name first in the alphabetic sequence.
// Used in the sorting function.
int comparegames(const void *s1, const void *s2){

	struct GAME_DATA_ITEM *game_a = (struct GAME_DATA_ITEM *)s1;
	struct GAME_DATA_ITEM *game_b = (struct GAME_DATA_ITEM *)s2;
	int r = 0;
	char c;
	
	// Uppercase the first character
	c = toupper(game_a->name[0]);
	game_a->name[0] = c;
	c = toupper(game_b->name[0]);
	game_b->name[0] = c;
	
	r = strcmp(game_a->name, game_b->name);
	
	// Game b comes first in character sequence
	if (r < 0){
		return -1;
	}
	
	// Game a and b are equal in character sequence
	if (r == 0){
		return 0;
	}
	
	// Game a comes first in character sequence
	if (r > 0){
		return 1;;
	}
	
	return 0;
}

// Sorts the list of games in alphabetical order
int sortgames(struct GAME_DATA *game_data){
	
	qsort(game_data->game_data_items, game_data->items, sizeof (game_data->game_data_items[0]), comparegames);
	return 0;
}
