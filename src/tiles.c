/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Tiles (and pickups).
 */


#include <allegro.h>
#include "anim.h"
#include "blast.h"
#include "blood.h"
#include "explo.h"
#include "gameloop.h"
#include "globals.h"
#include "map.h"
#include "player.h"
#include "stats.h"
#include "statlist.h"
#include "sound.h"
#include "tiles.h"


static int feels_pain(int x)
{
    return (x == T_BAR) || (x == T_CRATE) || (x == T_WOOD);
}


int tile_is_solid(int x)
{
    return (x && (x != T_LAD));
}


int hurt_tile(int u, int v, int dmg, int tag)
{
    int t = tile_at(u, v);
    
    if (feels_pain(t)) {
	map.tiletics[v][u] -= dmg;
	
	if (map.tiletics[v][u] <= 0) {
	    /* tile is dead */
	    map.tile[v][u] = 0;
	    spawn_explo(u * 16, v * 16, X_EXPLO0, 2);
	    map.tiletics[v][u] = st_tile_respawn * GAME_SPEED;

	    if (t == T_BAR) {
		u = u * 16 + 8;
		v = v * 16 + 8;
		blast(u, v, st_barrel_damage, tag);
		snd_3d(WAV_EXPLODEBAR, 150, u, v);
	    }
	}

	return 2; /* ouch! */

    }
    else if (tile_is_solid(t))
	return 1; /* collision */

    return 0; /* no collision */
}


void respawn_tiles()
{
    int v, u, i;

    for (v = map.h - 1; v >= 0; v--) {
	for (u = map.w - 1; u >= 0; u--) {
	    if (map.tile[v][u] || !map.tiletics[v][u])
		continue;
	    
	    if (--map.tiletics[v][u] == 0) {
		map.tile[v][u] = map.tileorig[v][u];
		map.tiletics[v][u] = st_tile_health;

		/* check no idiot's in the way */
		for (i = 0; i < MAX_PLAYERS; i++) {
		    if (players[i].health &&
			((players[i].x / 16 == u && players[i].y / 16 == v) 
			 || ((players[i].x + 5) / 16 == u
			     && (players[i].y + 15) / 16 == v))) {
			map.tile[v][u] = 0;
			map.tiletics[v][u] = st_tile_respawn * GAME_SPEED / 2;
			break;
		    }
		}
	    }
	    else if (map.tiletics[v][u] == EXPLO_ANIM * 7) {
		/* do that bright blue thing */
		spawn_explo(u * 16, v * 16, RESPAWN0, 7);
	    }
	}
    }
}


void respawn_ammo()
{
    int v, u;
    for (v = map.h - 1; v >= 0; v--) {
	for (u = map.w - 1; u >= 0; u--) {
	    if (!map.ammo[v][u] && map.ammotics[v][u]) {
		if (--map.ammotics[v][u] < 1) {
		    map.ammo[v][u] = map.ammoorig[v][u];
		    map.ammotics[v][u] = ammo_respawn_rate(map.ammo[v][u]);
		}
		else if (map.ammotics[v][u] == EXPLO_ANIM * 7) {
		    spawn_explo(u * 16, v * 16, RESPAWN0, 7);
		}
	    }
	}
    }
}


void draw_tiles_and_stuff()
{
    int u, v;

    for (v = 0; v < 14; v++)
	for (u = 0; u < 21; u++) {
	    /* pickup */
	    if (map.ammo[v + my][u + mx])
		draw_sprite(dbuf, dat[map.ammo[v + my][u + mx]].dat, 
			    u * 16 - offsetx, v * 16 - offsety);
	    
	    /* tile */
	    if (map.tile[v + my][u + mx])
		draw_sprite(dbuf, dat[map.tile[v + my][u + mx]].dat,
			    u * 16 - offsetx, v * 16 - offsety);
	}
}


int tile_collide(int u, int v)
{
    if ((u < 0) || (u > map.w - 1) ||
	(v < 0) || (v > map.h - 1))
	return 0;
    
    return map.tile[v][u];
}


int tile_collide_p(int x, int y)
{
    if ((x < 0) || (y < 0) || (x >= (map.w * 16)) || (y >= (map.h * 16)))
	return 0;
    
    return map.tile[y / 16][x / 16];
}


int ammo_collide(int u, int v)
{
    if ((u < 0) || (u > map.w - 1) ||
	(v < 0) || (v > map.h - 1))
	return 0;
    
    return map.ammo[v][u];
}
