/* 
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Bullets.
 */


#include <allegro.h>
#include "particle.h"
#include "bullet.h"
#include "blast.h"
#include "blood.h"
#include "engine.h"
#include "explo.h"
#include "gameloop.h"
#include "globals.h"
#include "map.h"
#include "message.h"
#include "player.h"
#include "rnd.h"
#include "sound.h"
#include "suicide.h"
#include "tiles.h"
#include "vector.h"


static VECTOR(bullets, PARTICLE);


static int is_explosive(int pic)
{
    return ((pic == J_ROCKET) || (pic == J_ARROW));
}


static void _reset_bullets(int lower)
{
    int i;
    
    for (i = lower; i < vector_size(bullets); i++)
	bullets[i].life = 0;
}


void spawn_bullet(int pl, fixed angle, int c, int bmp)
{
    const int speed = 8;
    PARTICLE *b;
    int i, w;

    for (i = vector_size(bullets) - 1; i >= 0; i--) {
	if (bullets[i].life) continue;

	w = (((BITMAP *) dat[gun_pic(pl)].dat)->w / 4);
	
	b = bullets + i;
	
	b->x = itofix(players[pl].x + 3) + fcos(players[pl].angle) * w;
	b->y = itofix(players[pl].y + 4) + fsin(players[pl].angle) * w;
	b->xv = fcos(angle) * speed;
	b->yv = fsin(angle) * speed;
	b->dmg = weaps[players[pl].cur_weap].dmg * (players[pl].bloodlust + 1);
	b->colour = c;
	b->bmp = bmp;
	b->tag = pl;
	b->angle = angle;
	b->life = 1;
	return;
    }

    if (!vector_resize(bullets, vector_size(bullets) + 50))
	suicide("Bullet overflow");
    
    _reset_bullets(vector_size(bullets) - 50);
/*      add_msg("Allocated extra 50 bullets", -1); */

    spawn_bullet(pl, angle, c, bmp);
}


void update_bullets()
{
    int u, v, i;
    PARTICLE *b;
    
    for (i = vector_size(bullets) - 1; i >= 0; i--) {
	if (!bullets[i].life) continue;
	
	b = bullets + i;

	b->x += b->xv;
	b->y += b->yv;

	/* knife spinning motion */
	if (b->bmp == J_KNIFE)
	    b->angle += (b->xv > 0) ? itofix(20) : -itofix(20);

	/* check off map */
	b = bullets + i;
	    
	if ((b->x < 0 && b->xv <= 0)
	    || (b->y < 0 && b->yv <= 0)
	    || (b->x > itofix(map.w * 16) && b->xv >= 0)
	    || (b->y > itofix(map.h * 16) && b->yv >= 0)) {
	    b->life = 0;
	    continue;
	}

	/* check hit tile? */
	u = fixtoi(b->x) / 16;
	v = fixtoi(b->y) / 16;

	if (!hurt_tile(u, v, b->dmg, b->tag)) 
	    continue;
	
	/* what happens when bullet hits a tile */
	
	b->life = 0;

	u = fixtoi(b->x - b->xv);
	v = fixtoi(b->y - b->yv);
	spawn_particles(u, v, 50, grad_orange);
	
	if (is_explosive(b->bmp)) {
	    /* explosion was caused */
	    spawn_explo(u - 8, v - 8, X_EXPLO0, 2);
	    blast(u + ((BITMAP *) dat[b->bmp].dat)->w / 2, v + 1,
		  b->dmg, b->tag);
	    snd_3d(WAV_EXPLODE, 255, u, v);
	}
	else {
	    /* ricochet */
	    spawn_explo(u, v, 0, 1);
	    if ((irnd() % 8) == 0)
		snd_3d(WAV_RIC + (irnd() % 2), 255, u, v);
	}
    }
}


void draw_bullets()
{
    int i;
    PARTICLE *b;

    for (i = vector_size(bullets) - 1; i >= 0; i--) {
	if (!bullets[i].life) continue;

	b = bullets + i;
	
	if (b->bmp) {
	    if (is_explosive(b->bmp)) 
		draw_light(L_EXPLO, fixtoi(b->x) - px, fixtoi(b->y) - py);
	    rotate_sprite(dbuf, dat[b->bmp].dat, fixtoi(b->x) - px, fixtoi(b->y) - py, b->angle);
	}
	else
	    putpixel(dbuf, fixtoi(b->x) - px, fixtoi(b->y) - py, b->colour);
    }
}


void touch_bullets()
{
    int i, j, x, y;
    PARTICLE *b;

    for (i = vector_size(bullets) - 1; i >= 0; i--) {
	if (!bullets[i].life) continue;
	
	b = bullets + i;
	
	for (j = 0; j < MAX_PLAYERS; j++) {
	    if (j != b->tag && players[j].health) {
		x = fixtoi(b->x);
		y = fixtoi(b->y);
		
		/* got in my way -- you're gonna die */
		if ((x >= players[j].x - 4) && (x <= players[j].x + 8) &&
		    (y >= players[j].y + 0) && (y <= players[j].y + 15)) {
		    
		    b->life = 0;
		    
		    if (is_explosive(b->bmp)) {
			spawn_explo(x, y, X_EXPLO0, 2);
			blast(x + ((BITMAP *) dat[b->bmp].dat)->w / 2,
			      y + 1, b->dmg, b->tag);
			snd_3d(WAV_EXPLODE, 255, x, y);
		    }
		    else 
			hurt_player(j, b->dmg, 1, b->tag, 0);
		}
	    }
	}
    }
}


int bullets_init()
{
    return vector_resize(bullets, 200);
}


void bullets_shutdown()
{
    vector_resize(bullets, 0);
}


void reset_bullets()
{
    _reset_bullets(0);
}


void reset_bullets_with_tag(int tag)
{
    int i;
    
    for (i = vector_size(bullets) - 1; i >= 0; i--) {
	if (bullets[i].tag == tag)
	    bullets[i].life = 0;
    }
}
