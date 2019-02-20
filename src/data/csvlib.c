#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "../menu.h"

char *strtok_single(char *str, char const *delims){
	static char *src = NULL;
	char *p, *ret = 0;

	if (str != NULL)
		src = str;

	if (src == NULL)
		return NULL;

	if ((p = strpbrk(src, delims)) != NULL){
		*p  = 0;
		ret = src;
		src = ++p;
	} else if (*src){
		ret = src;
		src = NULL;
	}
	return ret;
}

// Write a list of gamedata to csv file
int csv_writer(struct GAME_DATA *game_data, FILE *log, FILE *csv){
	
	int i; 		// Loop counter
	int records = 0;// Number of records written
	
	// Return number of bytes written, otherwise -1 for failure
	
	// name, directory name, path, has_readme, has_bitmap, has_binary, binary_1, binary_2, binary_3, readme_1, readme_2, readme_3
	
	fprintf(csv, "name,directory,path,has_readme,has_bitmap,has_binary,bitmap,binary_1,binary_1,binary_3,readme_1,readme_2,readme_3\n");
	for (i = 0; i < game_data->items; i++){
	
		//game_data->game_data_items[i];
		fprintf(log, "csv_writer: %d %s\n", i, game_data->game_data_items[i].name);
		fprintf(csv, "%s,", game_data->game_data_items[i].name);
		fprintf(csv, "%s,", game_data->game_data_items[i].directory);
		fprintf(csv, "%s,", game_data->game_data_items[i].path);
		fprintf(csv, "%d,", game_data->game_data_items[i].has_readme);
		fprintf(csv, "%d,", game_data->game_data_items[i].has_bitmap);
		fprintf(csv, "%d,", game_data->game_data_items[i].has_binary);
		fprintf(csv, "%s,", game_data->game_data_items[i].bitmap);
		fprintf(csv, "%s,", game_data->game_data_items[i].binary_1);
		fprintf(csv, "%s,", game_data->game_data_items[i].binary_2);
		fprintf(csv, "%s,", game_data->game_data_items[i].binary_3);
		fprintf(csv, "%s,", game_data->game_data_items[i].readme_1);
		fprintf(csv, "%s,", game_data->game_data_items[i].readme_2);
		fprintf(csv, "%s\n", game_data->game_data_items[i].readme_3);
		records++;
	}
	return records;
}

// Read a csv file and parse into a gamedata structure
int csv_reader(struct GAME_DATA *game_data, FILE *log, FILE *csv){
	
	int i = 0; 		// Loop counter
	int field_num = 0;
	char *p;
	// Allocate a big enough array to hold an entire game_data_item
	char line_buffer[sizeof(game_data->game_data_items[0])];
	
	// Read first line - skip it, it contains column names only
	while (fgets(line_buffer, sizeof(game_data->game_data_items[0]), csv) != NULL){
		field_num = 0;
		// Blank the gamedata structure
		memset(game_data->game_data_items[i].name, '\0', GAME_NAME_LEN);
		memset(game_data->game_data_items[i].directory, '\0', GAME_FILE_LEN);
		memset(game_data->game_data_items[i].path, '\0',GAME_PATH_LEN);
		game_data->game_data_items[i].has_readme = 0;
		game_data->game_data_items[i].has_bitmap = 0;
		memset(game_data->game_data_items[i].bitmap, '\0', GAME_FILE_LEN);
		memset(game_data->game_data_items[i].binary_1, '\0', GAME_FILE_LEN);
		memset(game_data->game_data_items[i].binary_2, '\0', GAME_FILE_LEN);
		memset(game_data->game_data_items[i].binary_3, '\0', GAME_FILE_LEN);
		memset(game_data->game_data_items[i].readme_1, '\0', GAME_FILE_LEN);
		memset(game_data->game_data_items[i].readme_2, '\0', GAME_FILE_LEN);
		memset(game_data->game_data_items[i].readme_3, '\0', GAME_FILE_LEN);
		
		// Read first field
		p = strtok_single(line_buffer, ",");
		while(p){
			//printf("%d %s\n", i, p);
			if (strcmp(p, "name") == 0){
				//printf("Skip row %d\n", i);
				i--;
				break;
			} else {
				//printf("%d %s\n", field_num, p);
			}			
			// Map to gamedata fields
			switch(field_num){
				case 0:
					strncpy(game_data->game_data_items[i].name, p, strlen(p));
					break;
				case 1:
					strncpy(game_data->game_data_items[i].directory, p, strlen(p));
					break;
				case 2:
					strncpy(game_data->game_data_items[i].path, p, strlen(p));
					break;
				case 3:
					if (strcmp(p, "1") == 0){
						game_data->game_data_items[i].has_readme = 1;
					} else {
						game_data->game_data_items[i].has_readme = 0;
					}
					break;
				case 4:
					if (strcmp(p, "1") == 0){
						game_data->game_data_items[i].has_bitmap = 1;
					} else {
						game_data->game_data_items[i].has_bitmap = 0;
					}
					break;
				case 5:
					if (strcmp(p, "1") == 0){
						game_data->game_data_items[i].has_binary = 1;
					} else {
						game_data->game_data_items[i].has_binary = 0;
					}
					break;
				case 6:
					strncpy(game_data->game_data_items[i].bitmap, p, strlen(p));
					break;
				case 7:
					strncpy(game_data->game_data_items[i].binary_1, p, strlen(p));
					break;
				case 8:
					strncpy(game_data->game_data_items[i].binary_2, p, strlen(p));
					break;
				case 9:
					strncpy(game_data->game_data_items[i].binary_3, p, strlen(p));
					break;
				case 10:
					strncpy(game_data->game_data_items[i].readme_1, p, strlen(p));
					break;
				case 11:
					strncpy(game_data->game_data_items[i].readme_2, p, strlen(p));
					break;
				case 12:
					strncpy(game_data->game_data_items[i].readme_3, p, strlen(p));
					break;
				default:
					break;
			}
			field_num++;
			p = strtok_single(NULL, ",");
		}
		//printf("game_id: %d %s\n", i, game_data->game_data_items[i].name);
		i++;
	}
	// Return number of records parsed, otherwise -1 for failure
	game_data->items = i;
	game_data->pos = 0;
	return i;
}