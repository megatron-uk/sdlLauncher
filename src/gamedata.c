#include <stdio.h>
#include <string.h>
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

#include "menu.h"
#include "logging.h"

// Sets the gamedata object
int set_gamedata(FILE *log, char *gamepath, char *gamename, struct GAME_DATA *game_data){
	
	char fullpath[GAME_PATH_LEN];
	DIR *dir;
	struct dirent *ep;
#ifdef TOS
	char filename[GAME_PATH_LEN];
	struct stat stat_buf;
	memset( &stat_buf, 0, sizeof(struct stat) );
#endif
#ifdef AMIGAOS
	char filename[GAME_PATH_LEN];
	struct stat stat_buf;
	memset( &stat_buf, 0, sizeof(struct stat) );
#endif
	int pos = game_data->pos;
	char *substring;
		
	log_debug(log, "set_gamedata: Setting gamedata[%d] object for [%s]\n", game_data->pos, gamename);
	
	// Blank everything first
	memset(game_data->game_data_items[pos].name, '\0', GAME_NAME_LEN);
	memset(game_data->game_data_items[pos].directory, '\0', GAME_FILE_LEN);
	memset(game_data->game_data_items[pos].path, '\0',GAME_PATH_LEN);
	game_data->game_data_items[pos].has_readme = 0;
	game_data->game_data_items[pos].has_bitmap = 0;
	memset(game_data->game_data_items[pos].bitmap, '\0', GAME_FILE_LEN);
	memset(game_data->game_data_items[pos].binary_1, '\0', GAME_FILE_LEN);
	memset(game_data->game_data_items[pos].binary_2, '\0', GAME_FILE_LEN);
	memset(game_data->game_data_items[pos].binary_3, '\0', GAME_FILE_LEN);
	memset(game_data->game_data_items[pos].readme_1, '\0', GAME_FILE_LEN);
	memset(game_data->game_data_items[pos].readme_2, '\0', GAME_FILE_LEN);
	memset(game_data->game_data_items[pos].readme_3, '\0', GAME_FILE_LEN);
	
	// Set game name and directory name to be the subfolder name we scraped
	// If we decide to load this via csv at a later date, we can always alter 'name'
	// but directory must stay as-is.
	strcpy(game_data->game_data_items[pos].name, gamename);
	strcpy(game_data->game_data_items[pos].directory, gamename);
		
	// Set full pathname = gamepath + directory_seperator + directory name
	memset(fullpath, '\0', sizeof(fullpath));
	strcpy(fullpath, gamepath);
	strcat(fullpath, DIRSEP);
	strcat(fullpath, gamename);
	strcpy(game_data->game_data_items[pos].path, fullpath);
	log_debug(log, "set_gamedata: Full path set [%s]\n", game_data->game_data_items[pos].path);
	
	// Scan for further files
	dir = opendir(fullpath);
	
	// Find the additional files needed to run or display the game
	if (dir != NULL){
		while ((ep = readdir (dir)) != NULL){
			// Strip out "." and ".." on Posix type systems
			if (strcmp(ep->d_name, ".") != 0 && strcmp(ep->d_name, "..") != 0){
				// Only add directory entries, not plain files
				//log_debug(log, "%s found: %s\n", gamename, ep->d_name);
#ifdef POSIX
				if (ep->d_type == DT_REG){
					//log_debug(log, "Regular file %s\n", ep->d_name);
#endif
#ifdef TOS
				strcpy(filename, fullpath);
				strcat(fullpath, DIRSEP);
				strcat(filename, ep->d_name);
				stat(filename, &stat_buf);
				if (S_ISREG(stat_buf.st_mode) != 0){
#endif
#ifdef AMIGAOS
				strcpy(filename, fullpath);
				strcat(fullpath, DIRSEP);
				strcat(filename, ep->d_name);
				stat(filename, &stat_buf);
				if (S_ISREG(stat_buf.st_mode) != 0){
#endif
					// A readme file
					if ((strcmp(ep->d_name, DEFAULT_README_NAME) == 0) || (strcmp(ep->d_name, DEFAULT_README_NAME_U) == 0)){
						log_debug(log, "set_gamedata: Default readme [%s]\n", ep->d_name);
						game_data->game_data_items[pos].has_readme = 1;
						memset(game_data->game_data_items[pos].readme_1, '\0', sizeof(game_data->game_data_items[pos].readme_1));
						strcpy(game_data->game_data_items[pos].readme_1, ep->d_name);
					}
					
					// A folder bitmap
					if ((strcmp(ep->d_name, DEFAULT_BMP_NAME) == 0) || (strcmp(ep->d_name, DEFAULT_BMP_NAME_U) == 0)){
						log_debug(log, "set_gamedata: Folder bitmap [%s]\n", ep->d_name);
						game_data->game_data_items[pos].has_bitmap = 1;
						memset(game_data->game_data_items[pos].bitmap, '\0', sizeof(game_data->game_data_items[pos].bitmap));
						strcpy(game_data->game_data_items[pos].bitmap, ep->d_name);
					}
					
					// An exe file
					if ((strcmp(ep->d_name, DEFAULT_EXE_NAME) == 0) || (strcmp(ep->d_name, DEFAULT_EXE_NAME_U) == 0)){
						log_debug(log, "set_gamedata: Default exe [%s]\n", ep->d_name);
						game_data->game_data_items[pos].has_binary = 1;
						memset(game_data->game_data_items[pos].binary_1, '\0', sizeof(game_data->game_data_items[pos].binary_1));
						strcpy(game_data->game_data_items[pos].binary_1, ep->d_name);
					}
					
					// Something else... maybe another exe?
					substring = strrchr(ep->d_name, '.');
					
					if (substring){
						//log_debug(log, "set_gamedata: Additional file [%s, type %s]\n", ep->d_name, substring);
						// Is it a binary?
						if ((strcmp(substring, EXE_SUFFIX) == 0) || (strcmp(substring, EXE_SUFFIX_U) == 0)){
							// Check its not the default name
							if ((strcmp(ep->d_name, DEFAULT_EXE_NAME) != 0) && (strcmp(ep->d_name, DEFAULT_EXE_NAME_U) != 0)){
								// Its's an additional exe
								if (game_data->game_data_items[pos].binary_2[0] == '\0'){
									// Additional binary slot 1 free
									log_debug(log, "set_gamedata: Additional exe 1 [%s]\n", ep->d_name);
									strcpy(game_data->game_data_items[pos].binary_2, ep->d_name);
								} else if (game_data->game_data_items[pos].binary_3[0] == '\0'){
									// Additional binary slot 2 free
									log_debug(log, "set_gamedata: Additional exe 2 [%s]\n", ep->d_name);
									strcpy(game_data->game_data_items[pos].binary_3, ep->d_name);
								} else {
									log_debug(log, "set_gamedata: All additional binaries set\n");
								}
							} 
						} else if ((strcmp(substring, README_SUFFIX) == 0) || (strcmp(substring, README_SUFFIX_U) == 0)){
							// Check its not the default name
							if ((strcmp(ep->d_name, DEFAULT_README_NAME) != 0) && (strcmp(ep->d_name, DEFAULT_README_NAME_U) != 0)){
								// Its's an additional readme
								if (game_data->game_data_items[pos].readme_2[0] == '\0'){
									// Additional readme slot 1 free
									log_debug(log, "set_gamedata: Additional readme 1 [%s]\n", ep->d_name);
									strcpy(game_data->game_data_items[pos].readme_2, ep->d_name);
								} else if (game_data->game_data_items[pos].readme_3[0] == '\0'){
									// Additional readme slot 2 free
									log_debug(log, "set_gamedata: Additional readme 2 [%s]\n", ep->d_name);
									strcpy(game_data->game_data_items[pos].readme_3, ep->d_name);
								} else {
									log_debug(log, "set_gamedata: All additional readme's set\n");
								}
							} 
						} else {
							//log_debug(log, "set_gamedata: Not a matching filetype [%s]\n", substring);
						}
					}
					
				} else {
					log_debug(log, "set_gamedata: Skip subdir [%s]\n", ep->d_name);	
				}
			}
		}
		closedir(dir);
	} else {
		log_warn(log, "set_gamedata: Error - Unable to open directory [%s]\n", fullpath);
	}
	return 0;
}

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

// Returns a list of game directories available in the defined gamepath folder
int scangames(FILE *log, char *gamepath, struct GAME_DATA *game_data){
	
	DIR *dir;
	struct dirent *ep;
#ifdef TOS
	struct stat stat_buf;
	memset(&stat_buf, 0, sizeof(struct stat));
#endif
#ifdef AMIGAOS
	struct stat stat_buf;
	memset(&stat_buf, 0, sizeof(struct stat));
#endif
	
	log_debug(log, "scangames: Scanning for folders: [%s]\n", gamepath);
	dir = opendir(gamepath);
	
	if (dir != NULL){
		while (((ep = readdir (dir)) != NULL) && (game_data->items < GAME_DATA_MAX)){
			
			// Strip out "." and ".." on Posix type systems
			if (strcmp(ep->d_name, ".") != 0 && strcmp(ep->d_name, "..") != 0){
				// Only add directory entries, not plain files
#ifdef POSIX
				if (ep->d_type == DT_DIR){
#endif
#ifdef TOS
				stat(gamepath, &stat_buf);
				if (S_ISDIR(stat_buf.st_mode) != 0){
#endif
#ifdef AMIGAOS
				stat(gamepath, &stat_buf);
				if (S_ISDIR(stat_buf.st_mode) != 0){
#endif
					log_debug(log, "scangames: Adding dir: [%s @ game_id %d]\n", ep->d_name, game_data->items);
					set_gamedata(log, gamepath, ep->d_name, game_data);
					game_data->pos++;
					game_data->items++;
				}
			}
		}
		closedir(dir);
	} else {
		log_warn(log, "scangames: Error - Unable to open directory\n");
		return -1;
	}
	return game_data->items;
}

// Compare two game data items and return the id
// of the game which has the name first in the alphabetic sequence.
// Used in the sorting function.
int comparegames(const void *s1, const void *s2){

	struct GAME_DATA_ITEM *game_a = (struct GAME_DATA_ITEM *)s1;
	struct GAME_DATA_ITEM *game_b = (struct GAME_DATA_ITEM *)s2;
	int r = 0;
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