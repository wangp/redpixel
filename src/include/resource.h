#ifndef __included_resource_h
#define __included_resource_h


#ifndef MAX_PATH_LENGTH
#define MAX_PATH_LENGTH	1024
#endif


#define R_SHARE		1
#define R_LIB		2
#define R_HOME		3


char *get_resource(int type, const char *name);
void set_game_path(const char *p);


#endif
