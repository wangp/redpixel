/* 
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Blood chunks ("blods").
 */


#include <string.h>
#include <allegro.h>
#include "blod.h"
#include "blood.h"
#include "engine.h"
#include "gameloop.h"
#include "globals.h"
#include "message.h"
#include "rnd.h"
#include "tiles.h"


#define MAX_BLODS       1000

static BLOD blods[MAX_BLODS];


void spawn_blods(int x, int y, int num)
{
    int i;

    for (i = 0; i < MAX_BLODS; i++) {
	if (blods[i].life) continue;
	
	blods[i].x = x + (rnd() % 16 * 2) - 8;
	blods[i].y = y + (rnd() % 16 * 2) - 8;
	blods[i].life = (rnd() % 400) + 300;
	blods[i].pic = BLOD0 + (rnd() % 15);
	
	if (--num <= 0) return;
    }

/*      add_msg("Blods overflow", -1); */
}


void update_blods()
{
    int i;
    
    for (i = 0; i < MAX_BLODS; i++) {
	if (!blods[i].life) continue;
	
	blods[i].life--;
	if (!tile_collide_p(blods[i].x, blods[i].y + ((BITMAP *) dat[blods[i].pic].dat)->h - 1))
	    blods[i].y++;
    }
}


void draw_blods()
{
    int i;

    for (i = 0; i < MAX_BLODS; i++) {
	if (!blods[i].life) continue;
	
	draw_sprite(dbuf, dat[blods[i].pic].dat,
		    blods[i].x - px, blods[i].y - py);
    }
}


void reset_blods()
{
    memset(blods, 0, MAX_BLODS * sizeof(BLOD));
}
