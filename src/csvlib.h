
#define CSVFILE CSV_FILE

// Write a list of gamedata to csv file
int csvwriter(struct GAME_DATA *game_data);

// Read a csv file and parse into a gamedata structure
int csvreader(struct GAME_DATA *game_data);
