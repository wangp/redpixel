#ifndef __included_globals_h
#define __included_globals_h


extern DATAFILE *dat;
extern BITMAP *dbuf;
extern BITMAP *light;

extern RGB_MAP rgb_table;
extern COLOR_MAP alpha_map;
extern COLOR_MAP light_map;

extern long seed;


int load_dat();
void unload_dat();

void setup_lighting();


#endif
