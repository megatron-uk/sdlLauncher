#include <SDL/SDL.h>

// Default names of useful files
#define EXE_SUFFIX ".tos"
#define EXE_SUFFIX_U ".TOS"
#define EXE_ALT_SUFFIX ".prg"
#define EXE_ALT_SUFFIX_U ".PRG"
#define DEFAULT_EXE_NAME "runme.tos"
#define DEFAULT_EXE_NAME_U "RUNME.TOS"
#define BMP_SUFFIX ".bmp"
#define BMP_SUFFIX_U ".BMP"
#define DEFAULT_BMP_NAME "folder.bmp"
#define DEFAULT_BMP_NAME_U "FOLDER.BMP"
#define README_SUFFIX ".txt"
#define README_SUFFIX_U ".TXT"
#define DEFAULT_README_NAME "readme.txt"
#define DEFAULT_README_NAME_U "README.TXT"

// The directory seperator for this plaform
#define DIRSEP DIR_SEP

// Maximum string lengths
#define GAME_NAME_LEN 32
#define GAME_PATH_LEN 128
#define GAME_FILE_LEN 13 // 8.3 naming scheme

// Screen geometry
static unsigned const int SCREEN_W = 320;
static unsigned const int SCREEN_H = 200;
static unsigned const int SCREEN_BPP = 8;

// Size of the game wallpaper we load
static unsigned const int BMP_W = 180;
static unsigned const int BMP_H = 135;

// Standard font is 6x8
static unsigned const int FONT_W = 6;
static unsigned const int FONT_H = 8;

// A structure to hold window coordinates
typedef struct COORDS{
	int x;
	int y;
	int w;
	int h;
} COORDS;

// Browser window coordinates
static struct COORDS BROWSER_COORDS(){
	COORDS coords;
	coords.x = 0;
	coords.y = 0;
	coords.w = (SCREEN_W - BMP_W);
	coords.h = (BMP_H + 2);
	return coords;
}

// Info window coordinates
static struct COORDS INFO_COORDS(){
	COORDS coords;
	coords.x = 0;
	coords.y = (BMP_H + 2);
	coords.w = SCREEN_W;
	coords.h = (SCREEN_H - (BMP_H + 2));
	return coords;
}

// Game bitmap window coordinates
static struct COORDS GAMECOVER_COORDS(){
	COORDS coords;
	coords.x = (SCREEN_W - (BMP_W + 2));
	coords.y = 0;
	coords.w = (BMP_W + 2);
	coords.h = (BMP_H + 2);
	return coords;
}

// Text reader window coordinates
static struct COORDS READER_COORDS(){
	COORDS coords;
	coords.x = 20;
	coords.y = 20;
	coords.w = (SCREEN_W - 40);
	coords.h = (SCREEN_H - 40);
	return coords;
}

// Structure which holds data on what games are visible in the scrolling window
// which game is currently highlighted, etc.
typedef struct BROWSER_WINDOW_DATA {
	int start_pos;	// Index into the array of games indicating the top most item
	int end_pos;	// Index into the array of games indicating the bottom most item
	int select_pos;	// Index into the array of games indicating the currently selected item
	int last_pos;	// Which was last selected
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
enum WINDOW_NAME{W_BROWSER, W_INFO, W_TEXT};

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

// Structure which holds state information for
// all of the windows on screen.
typedef struct WINDOW_STATE {	
	enum WINDOW_NAME selected_window;			// Which window is highlighted.. e.g. tab to move between browser and info
	struct BROWSER_WINDOW_DATA browser_window;	// Track state of the browser window
	struct INFO_WINDOW_DATA info_window;		// Track state of the info window
	struct TEXT_WINDOW_DATA text_window;		// Track state of the text reader window
	SDL_Surface *font_normal;
	SDL_Surface *font_reverse;
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
	struct GAME_DATA_ITEM game_data_items[250];
	int pos;
	int items;
} GAME_DATA;

// Number of games that can be listed
// This creates an array of N * game_data structs.
static const int GAME_DATA_MAX = 250;