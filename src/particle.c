/* 
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 * 
 *  Particles.
 */


#include <allegro.h>
#include "blood.h"
#include "engine.h"
#include "gameloop.h"
#include "globals.h"
#include "map.h"
#include "message.h"
#include "particle.h"
#include "rnd.h"
#include "tiles.h"


#define MAX_PARTICLES   5000

static PARTICLE particles[MAX_PARTICLES];


void spawn_particles(int x, int y, int num, int grad)
{
    int i;
    int hi = 0, low = 0;
    fixed angle, spd;

    /*
     *   0-31   black
     *  48-63   dark blue
     *  64-95   brown
     *  96-127  red
     * 128-159  darker brown 
     * 160-175  orange - yellow
     * 176-191  greyish
     * 192-233  light blue
     * 224-239  purple 
     * 240-255  green
     */

    switch (grad) {
	case grad_red:
	    low = 96;
	    hi = 127;
	    break;
	
	case grad_orange:
	    low = 160;
	    hi = 175;
	    break;
    }

    for (i = 0; i < MAX_PARTICLES; i++) {
	if (particles[i].life) continue;
	
	spd = itofix((rnd() % 40) + 1);
	spd = fdiv(spd, itofix(10));
	particles[i].x = itofix(x);
	particles[i].y = itofix(y);
	angle = itofix(rnd() % 256);
	particles[i].xv = fmul(fcos(angle), spd);
	particles[i].yv = fmul(fsin(angle), spd);

	particles[i].life = 
	    (grad == grad_red) 	
	    	? ((rnd() % 35) + 15) * 5    /* blood lasts longer */
	    	: (rnd() % 40) + 20;
	
	particles[i].colour = (rnd() % (hi - low)) + low;
	
	if (--num == 0) return;
    }

    add_msg("PARTICLE OVERFLOW", local);
}


/* specialised copy of spawn_particles */
void spawn_casing(int x, int y, int facing)
{
    int i;

    for (i = 0; i < MAX_PARTICLES; i++) {
	if (particles[i].life) continue;
	
	particles[i].x = itofix(x);
	particles[i].y = itofix(y);
	particles[i].xv = (facing == left ? itofix(1) : itofix(-1));
	particles[i].yv = itofix(-1);
	particles[i].life = 20;
	particles[i].colour = 175;
	return;
    }

    add_msg("PARTICLE OVERFLOW (CASING)", local);
}


void update_particles()
{
    int i, t;

    for (i = 0; i < MAX_PARTICLES; i++) {
	if (!particles[i].life) continue;
	
	particles[i].life--;

	particles[i].x += particles[i].xv;
	particles[i].y += (particles[i].yv += ftofix(.15));

	t = tile_at_p(fixtoi(particles[i].x), fixtoi(particles[i].y));
	if (tile_is_solid(t)) {
	    particles[i].xv *= -.5;
	    particles[i].yv *= -.5;
	    if (abs(particles[i].xv) < ftofix(.0) && abs(particles[i].yv) < ftofix(.0) && particles[i].life < 10)
		particles[i].life = 0;
	}
    }
}


void draw_particles()
{
    int i;
    
    for (i = 0; i < MAX_PARTICLES; i++) {
	if (particles[i].life)
	    putpixel(dbuf, fixtoi(particles[i].x) - px, fixtoi(particles[i].y) - py,
		     particles[i].colour);
    }
}


void reset_particles()
{
    memset(particles, 0, MAX_PARTICLES * sizeof(PARTICLE));
}
