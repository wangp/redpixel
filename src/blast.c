/* 
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 * 
 *  Explosion splash damage.
 */


#include <allegro.h>
#include "engine.h"
#include "gameloop.h"
#include "map.h"
#include "mine.h"
#include "player.h"
#include "tiles.h"


#define CLAMP(x, lower, upper) x = MID(lower, x, upper)


void blast(int x, int y, int dmg, int tag)
{
    int u, v, d;
    int i, j;
    fixed angle;

    u = x / 16;
    v = y / 16;

    shake_factor += 10;

    /* radius in pixels, 40 pixels around */

    /* barrels, wood, crates*/
    for (j = -2; j <= 2; j++) {
	for (i = -2; i <= 2; i++) {
	    if ((v + j >= 0) && (v + j < map.h)
		&& (u + i >= 0) && (u + i < map.w)) {
		d = find_distance(u, v, u + i, v + j);
		if (d == 0) 
		    d = 1;
		hurt_tile(u + i, v + j, (int)dmg / d, tag);
	    }
	}
    }

    /* mines */
    for (i = max_mines() - 1; i >= 0; i--)
	if (mine_in_range(i, x - 40, x + 40, y - 40, y + 40))
	    blow_mine(i);

    /* players */
    for (i = 0; i < MAX_PLAYERS; i++) {
	if (!players[i].health) continue;
	
	u = players[i].x + 8;
	v = players[i].y + 8;
	if ((u >= x - 40) && (u <= x + 40)
	    && (v >= y - 40) && (v <= y + 40)) {
	    
	    d = find_distance(u, v, x, y) / 16;
	    if (d == 0) d = 1;
	    hurt_player(i, dmg / d, 1, tag, 0);

	    angle = find_angle(x, y, u, v);
	    players[i].xv += fixtoi(fcos(angle) * 15) / d;
	    players[i].yv += fixtoi(fsin(angle) * 15) / d - 1;
	    
	    CLAMP(players[i].yv, -15, 15);
	    CLAMP(players[i].xv, -15, 15);

	    players[i].jump = (players[i].yv <= 0);
	}
    }
}
