/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  A layer above rpcd and rpjgmod.
 */


#include <allegro.h>
#include "music.h"
#include "rpcd.h"
#include "rpjgmod.h"


static int inited = 0;
static int using_cd = 0;
static int using_jgmod = 1;
static volatile int need_poll = 0;


static void need_poll_ticker(void)
{
    need_poll = 1;
}

END_OF_STATIC_FUNCTION(need_poll_ticker);


void music_init(void)
{
    rpcd_init();
    rpjgmod_init();
    LOCK_VARIABLE(need_poll);
    LOCK_FUNCTION(need_poll_ticker);
    install_int(need_poll_ticker, 4000);
    inited = 1;
}


void music_shutdown(void)
{
    if (inited) {
	remove_int(need_poll_ticker);
	rpjgmod_shutdown();
	rpcd_shutdown();
	inited = 0;
    }
}


void music_play_random_track(void)
{
    if (!inited)
	return;
    if (using_cd)
	rpcd_play_random_track();
    else if (using_jgmod)
	rpjgmod_play_random_track();
}


void music_poll(void)
{
    if (inited && need_poll) {
	if (using_cd)
	    rpcd_poll();
	else
	    rpjgmod_poll();
	need_poll = 0;
    }
}


void music_stop(void)
{
    if (!inited)
	return;
    if (using_cd)
	rpcd_stop();
    else if (using_jgmod)
	rpjgmod_stop();
}
