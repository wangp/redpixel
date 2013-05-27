#ifndef __included_globals_h
#define __included_globals_h


extern DATAFILE *dat;
extern BITMAP *dbuf;
extern ALLEGRO_BITMAP *light;

extern int32_t seed;


int load_dat(void);
void unload_dat(void);

ALLEGRO_BITMAP *create_bitmap_with_margin(int w, int h, int marginx, int marginy);

void setup_lighting(void);


#endif
