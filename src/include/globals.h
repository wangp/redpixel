#ifndef __included_globals_h
#define __included_globals_h


extern DATAFILE *dat;
extern BITMAP *dbuf;
extern ALLEGRO_BITMAP *light;

extern RGB_MAP rgb_table;
extern COLOR_MAP alpha_map;
extern COLOR_MAP light_map;

extern int32_t seed;


int load_dat(void);
void unload_dat(void);

void setup_lighting(void);


#endif
