/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Misc game engine stuff.
 */


#include <allegro.h>
#include "fastsqrt/fastsqrt.h"
#include "engine.h"
#include "backpack.h"
#include "blod.h"
#include "bullet.h"
#include "corpse.h"
#include "explo.h"
#include "globals.h"
#include "message.h"
#include "mine.h"
#include "particle.h"
#include "player.h"
#include "suicide.h"


/*----------------------------------------------------------------------*/

volatile int speed_counter;

static void speed_timer()
{
    speed_counter++;
}

END_OF_STATIC_FUNCTION(speed_timer);


/*----------------------------------------------------------------------*/

volatile int frame_counter, last_fps;

static void fps_timer()
{
    last_fps = frame_counter;
    frame_counter = 0;
}

END_OF_STATIC_FUNCTION(fps_timer);


/*----------------------------------------------------------------------*/


int bb_collide(BITMAP *spr1, int x1, int y1, BITMAP *spr2, int x2, int y2)
{
    return !((x1 > x2 + spr2->w) || (x2 > x1 + spr1->w) ||
	     (y1 > y2 + spr2->h) || (y2 > y1 + spr1->h));
}


fixed find_angle(int x1, int y1, int x2, int y2)
{    
    return fatan2(itofix(y2 - y1), itofix(x2 - x1)) & 0x00ff0000; /* get rid of decimals */
}


int find_distance(int x1, int y1, int x2, int y2)
{
    int a, b;

    a = x1 - x2;
    b = y1 - y2;
    a *= a;
    b *= b;

    /* fast sqrt, asm version -- fastest; does not work under Linux */
    /* return (int)fast_fsqrt_asm((float)a + (float)b); */

    /* fast sqrt, C version -- medium; works under DOS and Linux */
    return (int)fast_fsqrt((float)a + (float)b);

    /* libc version -- slowest; libc dependant */
    /* return (int)sqrt((float)a + (float)b); */
}


void draw_light(int bmp, int cx, int cy)
{
    BITMAP *mask = dat[bmp].dat;
    draw_trans_sprite(light, mask, cx - (mask->w / 2), cy - (mask->h / 2));
}


/*----------------------------------------------------------------------*/

void reset_engine()
{
    reset_bullets();
    reset_mines();
    reset_backpacks();
    reset_players();
    reset_particles();
    reset_explo();
    reset_blods();
    reset_corpses();
    reset_msgs();
}


void engine_init()
{
    build_sqrt_table();

    if (!bullets_init() 
	|| !mines_init() 
	|| !backpacks_init()
	|| !particles_init())
	suicide("Initial memory allocation failed");

    /* install timer interrupts */
    LOCK_VARIABLE(speed_counter);
    LOCK_FUNCTION(speed_timer);
    LOCK_VARIABLE(last_fps);
    LOCK_VARIABLE(frame_counter);
    install_int_ex(speed_timer, BPS_TO_TIMER(GAME_SPEED));
    install_int_ex(fps_timer, BPS_TO_TIMER(1));
}


void engine_shutdown()
{
    remove_int(fps_timer);
    remove_int(speed_timer);

    particles_shutdown();
    bullets_shutdown();
    mines_shutdown();
    backpacks_shutdown();
}
