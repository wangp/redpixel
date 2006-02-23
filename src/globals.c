/* 
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Just some common globals that don't belong.
 */


#include <sys/time.h>
#include <allegro.h>
#include "blood.h"
#include "globals.h"
#include "resource.h"


DATAFILE *dat;
BITMAP *dbuf;
BITMAP *light;

RGB_MAP rgb_table;
COLOR_MAP alpha_map;
COLOR_MAP light_map;

int32_t seed;



int load_dat()
{
    ASSERT(!dat);
    dat = load_datafile(get_resource(R_SHARE, "blood.dat"));
    return (dat) ? 0 : -1;
}

void unload_dat()
{
    if (dat) {
	unload_datafile(dat);
	dat = NULL;
    }
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
