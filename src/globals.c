/*
 *  Just some common globals that don't belong.
 */

#include <sys/time.h>
#include <allegro.h>
#include "resource.h"
#include "globals.h"
#include "blood.h"


DATAFILE *dat;
BITMAP *dbuf;
BITMAP *light;

RGB_MAP rgb_table;
COLOR_MAP alpha_map;
COLOR_MAP light_map;

long seed;



int load_dat()
{
    dat = load_datafile(get_resource(R_SHARE, "blood.dat"));
    return (dat) ? 0 : -1;
}

void unload_dat()
{
    unload_datafile(dat);
}



/* colour / lighting / translucency tables */
void setup_lighting()
{
    int x, y;
    
    create_rgb_table(&rgb_table, dat[GAMEPAL].dat, NULL);
    rgb_map = &rgb_table;
        
    create_light_table(&light_map, dat[GAMEPAL].dat, 0, 0, 0, NULL);

    for (x = 0; x < 256; x++)
	for (y = 0; y < 256; y++)
	    alpha_map.data[x][y] = MIN(x+y, 255);
}



long generate_seed()
{
    struct timeval tv;

    gettimeofday(&tv, 0);
    return tv.tv_sec + tv.tv_usec;
}
