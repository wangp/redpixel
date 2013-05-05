/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  A layer above rpjgmod (and formerly rpcd).
 */


#include "a4aux.h"
#include "music.h"
#include "rpjgmod.h"


static int inited = 0;
static int format =  MUSIC_FMT_MOD;
static volatile int need_poll = 0;


static void need_poll_ticker(void)
{
    need_poll = 1;
}


void music_init(void)
{
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
	inited = 0;
    }
}


int music_get_format(void)
{
    return format;
}


void music_set_format(int music_format)
{
    if (music_format == MUSIC_FMT_MOD)
	format = music_format;
    else
	format = MUSIC_FMT_NONE;
}


void music_play_random_track(void)
{
    if (!inited)
	return;
    if (format == MUSIC_FMT_MOD)
	rpjgmod_play_random_track();
}


void music_poll(void)
{
    if (inited && need_poll) {
	if (format == MUSIC_FMT_MOD)
	    rpjgmod_poll();
	need_poll = 0;
    }
}


void music_stop(void)
{
    if (!inited)
	return;
    if (format == MUSIC_FMT_MOD)
	rpjgmod_stop();
}
