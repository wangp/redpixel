/* 
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Helpers for Unix file system handling.
 */


#include <stdio.h>
#include <string.h>
#include <allegro.h>
#include "resource.h"


#ifndef ALLEGRO_UNIX


static char game_path[MAX_PATH_LENGTH];
static char path[MAX_PATH_LENGTH];


char *get_resource(int type, char *name)
{
    return replace_filename(path, game_path, name, MAX_PATH_LENGTH);
}


void set_game_path(char *p)
{
    strcpy(game_path, p);
}


#else  /* ALLEGRO_UNIX */


static char prefix_share[MAX_PATH_LENGTH];
static char prefix_lib[MAX_PATH_LENGTH];
static char path[MAX_PATH_LENGTH];


char *get_resource(int type, char *name)
{
    snprintf(path, sizeof path, "%s/%s",
	     (type == R_SHARE) ? prefix_share : prefix_lib, name);
    return path;
}


void set_game_path(char *p)
{
    if ((p[0] == 0) || (strchr(p, '/'))) {
	if (strstr(p, "/usr/local/") == p) {
	    strcpy(prefix_share, "/usr/local/share/redpixel");
	    strcpy(prefix_lib, "/usr/local/lib/redpixel");
	    return;
	}
	
	if (strstr(p, "/usr/") == p) {
	    strcpy(prefix_share, "/usr/share/games/redpixel");
	    strcpy(prefix_lib, "/usr/lib/games/redpixel");
	    return;
	}
	
	replace_filename(prefix_share, p, "", MAX_PATH_LENGTH);
	strcpy(prefix_lib, prefix_share);
	return;
    }
    else {
	char *env_path = getenv("PATH");
	char *q;
	
	if (!env_path) {
	    set_game_path("");
	    return;
	}
	
	env_path = strdup(env_path);
	
	q = strtok(env_path, ":");
	if (q) {
	    do {
		snprintf(path, MAX_PATH_LENGTH, "%s/%s", q, p);
		if (exists(path)) {
		    set_game_path(path);
		    break;
		}
		q = strtok(NULL, ":");
	    } while (q);
	}
	
	free(env_path);
    }
}


#endif
