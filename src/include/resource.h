#ifndef __included_resource_h
#define __included_resource_h


#ifndef MAX_PATH_LENGTH
#define MAX_PATH_LENGTH	1024
#endif


#define R_SHARE		1
#define R_LIB		2


char *get_resource(int type, char *name);
void set_game_path(char *p);


#endif
