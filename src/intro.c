/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Game introduction.
 *  Code converted from a SeeR script.
 *  Hacked to pieces to synchronise with music.
 */


#include <allegro.h>
#include "blood.h"
#include "globals.h"
#include "rpjgmod.h"
#include "vidmode.h"



#ifdef TARGET_LINUX

#include <sys/time.h>
#include <unistd.h>

typedef struct timeval WATCH;

static void set_watch(WATCH *watch)
{
    gettimeofday(watch, NULL);
}

static uint32_t elapsed_time(WATCH *watch)
{
    WATCH now;
    set_watch(&now);
    return ((now.tv_sec - watch->tv_sec) * 1000 +
	    (now.tv_usec - watch->tv_usec) / 1000);
}

#else

typedef uint32_t WATCH;

static void set_watch(WATCH *watch)
{
    *watch = clock() * 1000 / CLOCKS_PER_SEC;
}

static uint32_t elapsed_time(WATCH *watch)
{
    WATCH now;
    set_watch(&now);
    return (now - *watch);
}

#endif

static void wait_until(WATCH *watch, uint32_t msecs_elaspsed)
{
    while (elapsed_time(watch) < msecs_elaspsed)
	yield_timeslice();
}



static volatile int frames;

static void frame_ticker(void)
{
    frames++;
}

END_OF_STATIC_FUNCTION(frame_ticker);



/* duration to fit music: two seconds */
static int raster_words(char *s)
{
    WATCH watch;
    BITMAP *txt1;
    BITMAP *txt2;
    int x, y;
    int w, h;
    int nopress = 0;

    set_watch(&watch);
    LOCK_VARIABLE(frames);
    LOCK_FUNCTION(frame_ticker);
    install_int_ex(frame_ticker, BPS_TO_TIMER(60));

    /* create text images */
    w = text_length(dat[UNREAL].dat, s);
    h = text_height(dat[UNREAL].dat);
    if ((w % 2) == 0) w++;

    txt1 = create_bitmap(w, h);
    txt2 = create_bitmap(w, h);
    clear_bitmap(txt1);
    clear_bitmap(txt2);
    textout(txt1, dat[UNREAL].dat, s, 0, 0, -1);
    textout(txt2, dat[UNREAL].dat, s, 0, 0, -1);
    for (y = 0; y < h; y += 2) {
	hline(txt1, 0, y,     w - 1, 0);
	hline(txt2, 0, y + 1, w - 1, 0);
    }

    y = 100 - h / 2;
    x = -w;

    /* text in */
    frames = 1;
    while (x < 160 - w/2) {
	if (frames > 0) {
	    x += 6;
	    if (--frames == 0) {
		clear_bitmap(dbuf);
		draw_sprite(dbuf, txt1, x, y);
		draw_sprite(dbuf, txt2, 319 - x - w, y);
		blit_to_screen(dbuf);
	    }
	}
	
	if (keypressed())
	    goto quit;
    }

    /* pause a little */
    x = 160 - w/2 - 1;
    clear_bitmap(dbuf);
    draw_sprite(dbuf, txt1, x, y);
    draw_sprite(dbuf, txt2, 319 - x - w, y);
    blit_to_screen(dbuf);
    wait_until(&watch, 1500);

    /* text out */
    frames = 1;
    while (x < 320) {
	if (frames > 0) {
	    x += 6;
	    if (--frames == 0) {
		clear_bitmap(dbuf);
		draw_sprite(dbuf, txt1, x, y);
		draw_sprite(dbuf, txt2, 319 - x - w, y);
		blit_to_screen(dbuf);
	    }
	}
	
	if (keypressed())
	    goto quit;
    } 

    /* pause a little */
    clear_bitmap(screen);
    wait_until(&watch, 2000);

    nopress = 1;

  quit:

    destroy_bitmap(txt1);
    destroy_bitmap(txt2);
    
    remove_int(frame_ticker);
    
    return nopress;
}


static int scan(int x, int y)
{
    BITMAP *bmp;
    int x2, i, j = 0;
    WATCH watch;
    int keyed = 0;

    set_watch(&watch);
    
    x2 = x + 60;

    bmp = create_bitmap(320, 200);
    clear_bitmap(bmp);

    do {
	j = (j) ? 0 : 1;

	for (i = j; i < 80; i += 2)
	    blit(dat[TITLE].dat, bmp, x, y + i, 120, 60 + i, 80, 1);

	blit_to_screen(bmp);
	x++;
	rest(60);
	if (keypressed()) {
	    keyed = 1;
            break;
	}
    } while ((x < x2) && (elapsed_time(&watch) < 3600));

    destroy_bitmap(bmp);
    
    return !keyed;
}


void intro(void)
{
    int x, y, x2, y2;

    /* Give monitor a chance to set the mode.  I'm serious.  
     * Only important because we want to see the year scroll in.  */
    rest(1500);
    
    clear_keybuf();

    rpjgmod_play_specific_track("specific/present.xm");

    if (raster_words("1998")
	&& scan(10, 100) 
	&& raster_words("PSYK SOFTWARE") 
	&& scan(160, 90)
	&& raster_words("PRESENTS")) {

	WATCH watch;
	unsigned int step = 0;
	set_watch(&watch);

	x = 159; x2 = 160;
	y = 99;  y2 = 100;

	do {
	    clear_bitmap(dbuf);
	    blit(dat[TITLE].dat, dbuf, x, y, x, y, x2-x, y2-y);
	    blit_to_screen(dbuf);

	    do {
		step += 8;
		x--; x2++;
		y--; y2++;
	    } while (step < elapsed_time(&watch));
	    wait_until(&watch, step);
	} while (x > 0);

	rest(1600);
    }
    else {
	rpjgmod_stop();
    }

    fade_out(6);
    clear_bitmap(screen);
    set_palette(dat[GAMEPAL].dat);

    clear_keybuf();
}
