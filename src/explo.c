/* 
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 * 
 *  "Explosions" i.e. overlays and lights.
 */


#include <allegro.h>
#include "anim.h"
#include "blood.h"
#include "explo.h"
#include "engine.h"
#include "gameloop.h"
#include "globals.h"
#include "message.h"
#include "player.h"


#define MAX_EXPLOSIONS  100

static EXPLOSION explos[MAX_EXPLOSIONS];


void spawn_explo(int x, int y, int pic, int frames)
{
    int i;
    EXPLOSION *ex;
    
    for (i = 0; i < MAX_EXPLOSIONS; i++) {
	if (explos[i].alive) continue;
	
	ex = &explos[i];
	ex->alive = 1;
	ex->x = x;
	ex->y = y;
	ex->pic = pic;
	ex->cur = 0;
	ex->nopic = !pic;
	ex->frames = frames;
	ex->anim = EXPLO_ANIM;
	return;
    }

    add_msg("EXPLOSION OVERFLOW", -1);
}


void update_explo()
{
    int i;
    
    for (i = 0; i < MAX_EXPLOSIONS; i++) {
	if (explos[i].alive && (--explos[i].anim == 0)) {
	    explos[i].anim = EXPLO_ANIM;
	    if (++explos[i].cur >= explos[i].frames)
		explos[i].alive = 0;
	}
    }
}


void draw_explo()
{
    int i;

    for (i = 0; i < MAX_EXPLOSIONS; i++) {
	if (!explos[i].alive) continue;
	
	if (!explos[i].nopic)
	    draw_sprite(dbuf, dat[explos[i].pic + explos[i].cur].dat, 
			explos[i].x - px, explos[i].y - py);
	
	draw_light(L_EXPLO, explos[i].x - px, explos[i].y - py);
    }
}


void reset_explo()
{
    memset(explos, 0, MAX_EXPLOSIONS * sizeof(EXPLOSION));
}
