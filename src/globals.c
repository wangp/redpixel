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
    if (!dat)
	return -1;

    /* Index 0 is supposed to be opaque black. */
    set_palette(dat[GAMEPAL].dat);
    convert_8bit(dat[TITLE].dat, CONVERT_8BIT_PALETTE, al_map_rgb(0, 0, 0));
    convert_8bit(dat[FRAGDROP].dat, CONVERT_8BIT_PALETTE, al_map_rgb(0, 0, 0));

    /* Index 0 is transparent. */
    convert_8bit(dat[XHAIRLCD].dat, CONVERT_8BIT_PALETTE_REPLACE_INDEX0,
	al_map_rgba(0, 0, 0, 0));

    set_palette(dat[Z_HELPPAL].dat);
    convert_8bit(dat[Z_HELP].dat, CONVERT_8BIT_PALETTE, al_map_rgb(0, 0, 0));
    convert_8bit(dat[Z_HELPDEMO].dat, CONVERT_8BIT_PALETTE, al_map_rgb(0, 0, 0));

    return 0;
}

void unload_dat(void)
{
    if (dat) {
	unload_datafile(dat);
	dat = NULL;
    }
}



ALLEGRO_BITMAP *create_bitmap_with_margin(int w, int h, int marginx, int marginy)
{
    ALLEGRO_TRANSFORM t;
    ALLEGRO_BITMAP *old;
    ALLEGRO_BITMAP *bmp;

    old = al_get_target_bitmap();

    bmp = al_create_bitmap(w + marginx + marginx, h + marginy + marginy);
    al_set_target_bitmap(bmp);

    al_identity_transform(&t);
    al_translate_transform(&t, marginx, marginx);
    al_use_transform(&t);

    al_set_target_bitmap(old);

    return bmp;
}



/* colour / lighting / translucency tables */
void setup_lighting(void)
{
    ALLEGRO_COLOR unused = al_map_rgb(0, 0, 0);
    convert_8bit(dat[L_EXPLO].dat, CONVERT_8BIT_INTENSITY, unused);
    convert_8bit(dat[L_SPOT].dat, CONVERT_8BIT_INTENSITY, unused);
    convert_8bit(dat[L_SPOTB].dat, CONVERT_8BIT_INTENSITY, unused);
}
