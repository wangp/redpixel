/* 
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Backpacks.
 */


#include <stdlib.h>
#include "a4aux.h"
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


typedef struct {
    char alive, unactive;
    int x, y;
    int num_bullets, num_shells, num_rockets, num_arrows, num_mines;
} BACKPACK;


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

    if (!vector_resize(backpacks, max_backpacks + 2))
	suicide("Backpack overflow");
    
    _reset_backpacks(max_backpacks - 2);
/*      add_msg("Allocated extra 2 backpacks", -1); */

    spawn_backpack(x, y, b, s, r, a, m);
}


void update_backpacks(void)
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


void draw_backpacks(void)
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


void touch_backpacks(void)
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
	    add_msg("You stole a backpack", j);
	    snd_local(WAV_PICKUP);
	}
    }
}


int backpacks_init(void)
{
    return vector_resize(backpacks, 5);
}


void backpacks_shutdown(void)
{
    free(backpacks);
}


void reset_backpacks(void)
{
    _reset_backpacks(0);
}
