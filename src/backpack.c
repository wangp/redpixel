/* 
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 * 
 *  Backpacks.
 */


#include <stdlib.h>
#include <allegro.h>
#include "anim.h"
#include "backpack.h"
#include "blood.h"
#include "engine.h"
#include "gameloop.h"
#include "globals.h"
#include "map.h"
#include "message.h"
#include "player.h"
#include "sound.h"
#include "suicide.h"
#include "tiles.h"
#include "vector.h"


static VECTOR(backpacks, BACKPACK);

#define max_backpacks	vector_size(backpacks)


static void _reset_backpacks(int lower)
{
    int i;
    
    for (i = lower; i < max_backpacks; i++)
	backpacks[i].alive = 0;
}


void spawn_backpack(int x, int y, int b, int s, int r, int a, int m)
{
    int i;
    
    for (i = max_backpacks - 1; i >= 0; i--) {
	if (backpacks[i].alive) continue;
	
	backpacks[i].x = x;
	backpacks[i].y = y;
	backpacks[i].num_bullets = b;
	backpacks[i].num_shells = s;
	backpacks[i].num_rockets = r;
	backpacks[i].num_arrows = a;
	backpacks[i].num_mines = m;
	backpacks[i].alive = 1;

	/* FIXME: this is just average time it takes.  */
	backpacks[i].unactive = 13 * CORPSE_ANIM;

	return;
    }

    if (!vector_resize(backpacks, max_backpacks + 5))
	suicide("Backpack overflow");
    
    reset_backpacks(max_backpacks - 5);
    add_msg("ALLOCATED EXTRA 5 BACKPACKS", -1);

    spawn_backpack(x, y, b, s, r, a, m);
}


void update_backpacks()
{
    int i;

    for (i = max_backpacks - 1; i >= 0; i--) {
	if (!backpacks[i].alive) 
	    continue;
	
	if (backpacks[i].unactive) {
	    --backpacks[i].unactive;
	    continue;
	}
	
	if (!tile_collide_p(backpacks[i].x + 8, backpacks[i].y + 16)) 
	    if (++backpacks[i].y > map.h * 16)
		backpacks[i].alive = 0;
    }
}


void draw_backpacks()
{
    int i;
    
    for (i = max_backpacks - 1; i >= 0; i--) {
	if (backpacks[i].alive && !backpacks[i].unactive) {
	    /* backpacks are 12x16 */
	    draw_sprite(dbuf, dat[A_BACKPACK].dat,
			backpacks[i].x - px, backpacks[i].y - py);
	    draw_light(L_EXPLO, backpacks[i].x + 6 - px, backpacks[i].y + 8 - py);
	}
    }
}


void touch_backpacks()
{
    int i, j;
    
    for (i = max_backpacks - 1; i >= 0; i--) {
	if (!backpacks[i].alive || backpacks[i].unactive) 
	    continue;
	
	for (j = 0; j < MAX_PLAYERS; j++) {
	    if (!players[j].health)
		continue;
	    
	    if (!bb_collide(dat[A_BACKPACK].dat, backpacks[i].x, backpacks[i].y,
			    dat[P_BODY].dat, players[j].x - 4, players[j].y))
		continue;
	    
	    players[j].num_bullets += backpacks[i].num_bullets;
	    players[j].num_shells += backpacks[i].num_shells;
	    players[j].num_rockets += backpacks[i].num_rockets;
	    players[j].num_arrows += backpacks[i].num_arrows;
	    players[j].num_mines += backpacks[i].num_mines;
	    if (players[j].num_mines)
		players[j].have[w_mine] = 1;
	    backpacks[i].alive = 0;
	    add_msg("YOU STOLE A BACKPACK", j);
	    snd_local(WAV_PICKUP);
	}
    }
}


int backpacks_init()
{
    return vector_resize(backpacks, 20);
}


void backpacks_shutdown()
{
    free(backpacks);
}


void reset_backpacks()
{
    _reset_backpacks(0);
}
