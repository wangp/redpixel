/* resource.c
 *
 * Peter Wang <tjaden@psynet.net>
 */

/* TODO: This will one day implement proper Unix file system handling.  */


#include <string.h>
#include <allegro.h>
#include "resource.h"


static char game_path[MAX_PATH_LENGTH], path[MAX_PATH_LENGTH];


char *get_resource(int type, char *name)
{
    return replace_filename(path, game_path, name, MAX_PATH_LENGTH);
}


void set_game_path(char *p)
{
    strcpy(game_path, p);
}
