/* 
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 * 
 *  Corpses.
 */


#include <allegro.h>
#include "corpse.h"
#include "anim.h"
#include "blood.h"
#include "engine.h"
#include "gameloop.h"
#include "globals.h"
#include "map.h"
#include "tiles.h"


#define MAX_CORPSES     50

static CORPSE corpses[MAX_CORPSES];
static int oldest;


void spawn_corpse(int x, int y, int facing, int first, int frames)
{
    int i;
    
    for (i = 0; i < MAX_CORPSES; i++) {
	if (corpses[i].alive) continue;
	
      set_values:
	
	corpses[i].x = x - ((BITMAP *) dat[first].dat)->w / 2;
	corpses[i].y = y - ((BITMAP *) dat[first].dat)->h;
	corpses[i].facing = facing;
	corpses[i].first_frame = first;
	corpses[i].num_frames = frames;
	corpses[i].cur = 0;
	corpses[i].anim = CORPSE_ANIM;
	corpses[i].alive = 1;
	return;
    }

    /* recycle old corpses */
    i = oldest;
    if (++oldest > MAX_CORPSES - 1) oldest = 0;
    goto set_values;
}


void update_corpses()
{
    int i;
    
    for (i = 0; i < MAX_CORPSES; i++) {
	if (corpses[i].alive) {
	    if (--corpses[i].anim == 0) {
		if (++corpses[i].cur >= corpses[i].num_frames) {
		    corpses[i].cur = corpses[i].num_frames - 1;
		    corpses[i].anim = 0xff;
		}
		else {
		    corpses[i].anim = CORPSE_ANIM;
		}
	    }

	    if (!tile_collide_p(corpses[i].x + ((BITMAP *) dat[corpses[i].first_frame].dat)->w / 2,
				corpses[i].y + ((BITMAP *) dat[corpses[i].first_frame].dat)->h - 1))
		if (++corpses[i].y > map.h * 16)
		    corpses[i].alive = 0;
	}
    }
}


void draw_corpses()
{
    void (*draw)(BITMAP *, BITMAP *, int, int);
    int i;

    for (i = 0; i < MAX_CORPSES; i++) {
	if (!corpses[i].alive) continue;
	
	draw = (corpses[i].facing == right) ? draw_sprite : draw_sprite_h_flip;
	draw(dbuf, dat[corpses[i].first_frame + corpses[i].cur].dat, corpses[i].x - px, corpses[i].y - py);

	if (corpses[i].cur < corpses[i].num_frames - 1)
	    draw_trans_sprite(light, dat[L_SPOT].dat, 
			      corpses[i].x - px - 192 / 2,
			      corpses[i].y - py - 192 / 2);
    }
}


void reset_corpses()
{
    memset(corpses, 0, MAX_CORPSES * sizeof(CORPSE));
    oldest = 0;
}
