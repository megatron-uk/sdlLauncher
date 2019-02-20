// Header file for CSV file reader and writer functions.

// Default name of CSV file is defined in the makefile
#define CSVFILE CSV_FILE

// Write a list of gamedata to csv file
int csv_writer(struct GAME_DATA *game_data, FILE *log, FILE *csv);

// Read a csv file and parse into a gamedata structure
int csv_reader(struct GAME_DATA *game_data, FILE *log, FILE *csv);
