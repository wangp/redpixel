/* 
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Just some common globals that don't belong.
 */


#include <sys/time.h>
#include "a4aux.h"
#include "blood.h"
#include "globals.h"
#include "resource.h"


DATAFILE *dat;
BITMAP *dbuf;
ALLEGRO_BITMAP *light;

RGB_MAP rgb_table;
COLOR_MAP alpha_map;
COLOR_MAP light_map;

int32_t seed;



int load_dat(void)
{
    ASSERT(!dat);
    dat = load_datafile(get_resource(R_SHARE, "blood.dat"));
    return (dat) ? 0 : -1;
}

void unload_dat(void)
{
    if (dat) {
	unload_datafile(dat);
	dat = NULL;
    }
}



static void setup_light_sprite(BITMAP *bitmap)
{
    ALLEGRO_LOCKED_REGION *lock;
    unsigned char *rgba;
    int x, y;

    bitmap->real = al_create_bitmap(bitmap->w, bitmap->h);
    lock = al_lock_bitmap(bitmap->real, ALLEGRO_PIXEL_FORMAT_ABGR_8888_LE,
	ALLEGRO_LOCK_WRITEONLY);
    rgba = lock->data;
    for (y = 0; y < bitmap->h; y++){
	for (x = 0; x < bitmap->w; x++){
	    int c = *(bitmap->line[y] + x);
	    unsigned char red = c;
	    unsigned char green = c;
	    unsigned char blue = c;
	    unsigned char alpha = 255;
	    rgba[y * lock->pitch + x * 4 + 0] = red;
	    rgba[y * lock->pitch + x * 4 + 1] = green;
	    rgba[y * lock->pitch + x * 4 + 2] = blue;
	    rgba[y * lock->pitch + x * 4 + 3] = alpha;
	}
    }
    al_unlock_bitmap(bitmap->real);
}

/* colour / lighting / translucency tables */
void setup_lighting(void)
{
    setup_light_sprite(dat[L_EXPLO].dat);
    setup_light_sprite(dat[L_SPOT].dat);
    setup_light_sprite(dat[L_SPOTB].dat);
}
