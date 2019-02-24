// Default names of useful files
#define EXE_SUFFIX ".exe"
#define EXE_SUFFIX_U ".EXE"
#define EXE_ALT_SUFFIX ".com"
#define EXE_ALT_SUFFIX_U ".COM"
#define DEFAULT_EXE_NAME "runme.exe"
#define DEFAULT_EXE_NAME_U "RUNME.EXE"
#define BMP_SUFFIX ".bmp"
#define BMP_SUFFIX_U ".BMP"
#define DEFAULT_BMP_NAME "folder.bmp"
#define DEFAULT_BMP_NAME_U "FOLDER.BMP"
#define README_SUFFIX ".txt"
#define README_SUFFIX_U ".TXT"
#define DEFAULT_README_NAME "readme.txt"
#define DEFAULT_README_NAME_U "README.TXT"

// The directory seperator for this plaform
#define DIRSEP "/"
#define GAMEDIR "./GAMES"

// Name of CSV file
#define CSV_FILE "MENU.CSV"

// Name of config file
#define INI_FILE "MENU.INI"

// Colours to save for UI use
#ifdef USE_SDL
#define RESERVED_COLOURS 8
#endif

// Mac number of game directories to scan for sub-folders
#define MAX_GAMEDIRS 99

// Max number of games
#define GAME_DATA_MAX 250

// Screen geometry
#define MENU_SCREEN_W 320
#define MENU_SCREEN_H 240
#define MENU_SCREEN_W_MED 640
#define MENU_SCREEN_H_MED 480
#define MENU_SCREEN_W_HI 800
#define MENU_SCREEN_H_HI 600
#define MENU_SCREEN_BPP 8

// Size of the game wallpaper we load
static unsigned const int BMP_W = 180;
static unsigned const int BMP_H = 135;

// Standard font is 6x8
static unsigned const int FONT_W = 6;
static unsigned const int FONT_H = 8;

// Maximum string lengths
#define GAME_NAME_LEN 32
#define GAME_PATH_LEN 512
#define GAME_FILE_LEN 64
