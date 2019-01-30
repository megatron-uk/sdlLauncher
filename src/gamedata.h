#pragma once

// Sets the gamedata object
int set_gamedata(FILE *log, char *gamepath, char *gamename, struct GAME_DATA *game_data);

// Return data on a single game; bitmap, exe name etc.
int get_gamedata(FILE *log, char *gamename, struct GAME_DATA *game_data);

// Returns a list of game directories available in the defined gamepath folder
int scangames(FILE *log, char *gamepath, struct GAME_DATA *game_data);

// Sort games into alphabetical order
int sortgames(struct GAME_DATA *game_data);