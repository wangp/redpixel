/* 
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Mines.
 */


#include <allegro.h>
#include "anim.h"
#include "blast.h"
#include "blood.h"
#include "engine.h"
#include "explo.h"
#include "gameloop.h"
#include "globals.h"
#include "map.h"
#include "message.h"
#include "mine.h"
#include "player.h"
#include "sound.h"
#include "suicide.h"
#include "tiles.h"
#include "vector.h"


typedef struct {
    char alive;
    int x, y;
    char frame, anim;
    char tag, unactive;
} MINE;


static VECTOR(mines, MINE);


static void _reset_mines(int lower)
{
    int i;
    
    for (i = lower; i < vector_size(mines); i++)
	mines[i].alive = 0;
}


void spawn_mine(int x, int y, int tag)
{
    int i;
    
    for (i = 0; i < vector_size(mines); i++) {
	if (mines[i].alive) continue;
	
	mines[i].alive = 1;
	mines[i].x = x;
	mines[i].y = y;
	mines[i].unactive = 30;
	mines[i].frame = 0;
	mines[i].anim = MINE_ANIM;
	mines[i].tag = tag;
	return;
    }

    if (!vector_resize(mines, vector_size(mines) + 10))
	suicide("Mine overflow");

    _reset_mines(vector_size(mines) - 10);
    add_msg("ALLOCATED EXTRA 10 MINES", -1);

    spawn_mine(x, y, tag);
}


void update_mines()
{
    int i, t;
    
    for (i = vector_size(mines) - 1; i >= 0; i--) {
	if (!mines[i].alive) continue;
	
	/* drop */
	if (!tile_collide_p(mines[i].x + 3, mines[i].y + 2) 
	    || (mines[i].y % 16) != 14)
	    if (++mines[i].y > map.h * 16)
		mines[i].alive = 0;

	/* activate */
	if (mines[i].unactive) {
	    mines[i].unactive--;
	    continue;
	}

	/* animate */
	if (--mines[i].anim == 0) {
	    mines[i].anim = MINE_ANIM;
	    if (mines[i].frame == 0) {
		mines[i].frame = 1;
		/* blink blink: create light */
		t = tile_at_p(mines[i].x + 3, mines[i].y);
		if (!tile_is_solid(t))
		    spawn_explo(mines[i].x + 3, mines[i].y, 0, 2);
	    }
	    else {
		mines[i].frame = 0;
	    }
	}
    }
}


void draw_mines()
{
    int i;
    
    for (i = vector_size(mines) - 1; i >= 0; i--) {
	if (mines[i].alive)
	    draw_sprite(dbuf, dat[J_MINE1 + mines[i].frame].dat, 
			mines[i].x - px, mines[i].y - py);
    }
}


void blow_mine(int i)
{
    mines[i].alive = 0;
    spawn_explo(mines[i].x - 2, mines[i].y - 6, X_EXPLO0, 2);
    snd_3d(WAV_EXPLODE, 255, mines[i].x, mines[i].y);
    blast(mines[i].x + 3, mines[i].y, weaps[w_mine].dmg, mines[i].tag);
}


void touch_mines()
{
    int i, j;
    int x, y, x2, y2;
    
    for (i = vector_size(mines) - 1; i >= 0; i--) {
	if (!mines[i].alive || mines[i].unactive)
	    continue;

	for (j = 0; j < MAX_PLAYERS; j++) {
	    if (!players[j].health)
		continue;

	    x = players[j].x + 3;
	    y = players[j].y + 6;
	    x2 = mines[i].x + 3;
	    y2 = mines[i].y;

	    if ((x >= x2 - 10) && (x <= x2 + 10)
		&& (y >= y2 - 10) && (y <= y2 + 10))
		blow_mine(i);
	}
    }
}


int mine_in_range(int i, int x1, int y1, int x2, int y2)
{
    int x = mines[i].x + 3, y = mines[i].y;
    return mines[i].alive && (x >= x1) && (x <= x2) && (y >= y1) && (y <= y2);
}


int mines_init()
{
    return vector_resize(mines, 30);
}


void mines_shutdown()
{
    free(mines);
}


void reset_mines()
{
    _reset_mines(0);
}


void reset_mines_with_tag(int tag)
{
    int i;
    
    for (i = vector_size(mines) - 1; i >= 0; i--) {
	if (mines[i].tag == tag)
	    mines[i].alive = 0;
    }
}


int max_mines()
{
    return vector_size(mines);
}
