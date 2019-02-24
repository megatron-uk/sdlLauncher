#pragma once

#ifdef TOS
#include "platforms/atari.h"
#endif

#ifdef LINUX
#include "platforms/linux.h"
#endif

#ifdef DOS
#include "platforms/msdos.h"
#endif

// Structure which holds data on the category chooser UI element
typedef struct CATEGORY_WINDOW_DATA {
	int cat_selected;	// Index to the category UI element selected
	int min_cat;
	int max_cat;   // 26 letters (0-25) + all (element 26) + fav (27) + numbers (28)
} CATEGORY_WINDOW_DATA;

#define CATEGORY_MIN_CAT -3
#define CATEGORY_MAX_CAT 25
#define CATEGORY_DEFAULT_CAT -3
#define CATEGORY_ALL -3
#define CATEGORY_FAV -2
#define CATEGORY_NUM -1
#define CATEGORY_ALPHA 0
static char ALPHABET_CATS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static char ALPHABET_CATS_L[] = "abcdefghijklmnopqrstuvwxyz"; // Lower case version of the categories

// Structure which holds data on what games are visible in the scrolling window
// which game is currently highlighted, etc.
typedef struct BROWSER_WINDOW_DATA {
	
	// THese controls used when browsing games by category - we are not allowed to scroll beyond them
	int start_pos_filtered;
	int end_pos_filtered;
	
	// Normal browser window controls
	int start_pos;	// Index into the array of games indicating the top most item
	int end_pos;	// Index into the array of games indicating the bottom most item
	
	// Which game is currently selected
	int select_pos;	// Index into the array of games indicating the currently selected item
	int last_pos;		// Which was last selected
	
	int max_lines;	// Number of lines of games that can be shown in the browser window at current res.
} BROWSER_WINDOW_DATA;

enum INFO_MODE{M_BINARY, M_README};

// Structure which holds data on what options are available and selected
// in the game info window; which binary is highlighted, which readme is selected, etc.
typedef struct INFO_WINDOW_DATA {
	enum INFO_MODE info_mode_selected;
	int binary_selected;	// Which of the default and additional binaries is selected to launch
	int readme_selected;	// Which of the detected text files is selected to view
} INFO_WINDOW_DATA;

// Windows which can be selected
enum WINDOW_NAME{W_BROWSER, W_INFO, W_TEXT, W_CONFIG};

// Structure which holds state information for
// the text reader window.
typedef struct TEXT_WINDOW_DATA {
	int start_pos;		// Start line to display
	int end_pos;		// End line to display
	int max_lines;		// Maximum height of text display
	int max_chars;		// Maximum width of text display
	FILE *readme; 		// Open file pointer
	int f_pos;			// Position in open file
	char buffer[1024];	// Text file buffer
} TEXT_WINDOW_DATA;

enum WINDOW_CONFIG_OPTION{ OPTION_NONE, OPTION_CSV_EXPORT, OPTION_CSV_IMPORT, OPTION_RESCAN};

typedef struct CONFIG_WINDOW_DATA {
	enum WINDOW_CONFIG_OPTION config_option_selected;	
} WINDOW_CONFIG_OPTION;

// Structure which holds state information for
// all of the windows on screen.
typedef struct WINDOW_STATE {	
	enum WINDOW_NAME selected_window;			// Which window is highlighted.. e.g. tab to move between browser and info
	struct BROWSER_WINDOW_DATA browser_window;	// Track state of the browser window
	struct INFO_WINDOW_DATA info_window;		// Track state of the info window
	struct TEXT_WINDOW_DATA text_window;		// Track state of the text reader window
	struct CONFIG_WINDOW_DATA config_window;	// Track state of the config window
	struct CATEGORY_WINDOW_DATA category_window;
	struct agnostic_bitmap *font_normal;
	struct agnostic_bitmap *font_reverse;
} WINDOW_STATE;

// Structures for holding data on the games that are found
typedef struct GAME_DATA_ITEM {
	char name[GAME_NAME_LEN];	// Display name of the game
	char directory[GAME_FILE_LEN];	// Subdirectory name
	char path[GAME_PATH_LEN];		// Full drive and directory path
	bool has_readme;				// Found a README.TXT
	bool has_bitmap; 				// Found a FOLDER.BMP/TITLE.BMP/COVER.BMP
	bool has_binary;				// Found at least one binary file
	char bitmap[GAME_FILE_LEN];	// Filename of the bitmap image to load for this game
	char binary_1[GAME_FILE_LEN];	// 1st TOS/PRG file
	char binary_2[GAME_FILE_LEN];	// 2nd TOS/PRG file
	char binary_3[GAME_FILE_LEN];	// 3rd TOS/PRG file
	char readme_1[GAME_FILE_LEN];	// 1st TOS/PRG file
	char readme_2[GAME_FILE_LEN];	// 2nd TOS/PRG file
	char readme_3[GAME_FILE_LEN];	// 3rd TOS/PRG file
} GAME_DATA_ITEM;

// Contains:
// - currently selected item
// - array of game_data_items
// - current position in game_data_items array (e.g. for when adding new items)
typedef struct GAME_DATA {
	struct GAME_DATA_ITEM game_data_items[GAME_DATA_MAX];
	int pos;
	int items;
} GAME_DATA;

// Number of games that can be listed
// This creates an array of N * game_data structs.

// A structure to hold window coordinates
typedef struct COORDS{
	int x;
	int y;
	int w;
	int h;
} COORDS;


// Configuration data structure
typedef struct config_object {
	
	// Filename section
	char *csv_filename;
	char *gamedirs[MAX_GAMEDIRS];
	int gamedir_count;
	
	// Video stuff
	int menu_screen_w;
	int menu_screen_h;
	int menu_screen_bpp;
	int bmp_w;
	int bmp_h;
	
} config_object;

struct config_object menu_cfg;

// Browser window coordinates
static struct COORDS BROWSER_COORDS(struct config_object cfg);

// Letter browser window coordinates
static struct COORDS ALPHABET_COORDS(struct config_object cfg);

// Info window coordinates
static struct COORDS INFO_COORDS(struct config_object cfg);

// Game bitmap window coordinates
static struct COORDS GAMECOVER_COORDS(struct config_object cfg);

// Text reader window coordinates
static struct COORDS READER_COORDS(struct config_object cfg);

// Config menu window coordinates
static struct COORDS CONFIG_COORDS(struct config_object cfg);
