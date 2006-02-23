/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  libcda wrapper.
 */


#ifndef NO_LIBCDA_CODE


#include <time.h>
#include "libcda.h"
#include "rg_rand.h"
#include "rpcd.h"


#ifndef NULL
#define NULL	0
#endif


static int inited = 0;
static long rnd;


void rpcd_init()
{
    rnd = slongrand(time(0));
    
    inited = (cd_init() == 0);
}


void rpcd_shutdown()
{
    if (inited) {
	cd_stop();
	cd_exit();
	inited = 0;
    }
}


int rpcd_get_volume()
{
    int v = 0;
    if (inited) {
	cd_get_volume(&v, NULL);
	cd_set_volume(v, v);  /* potentially annoying to some people */
    }
    return v;
}


void rpcd_set_volume(int volume)
{
    if (inited)
	cd_set_volume(volume, volume);
}


void rpcd_play_random_track()
{
    int t0, t1, i;
    
    if (!inited) 
	return;
    
    if (cd_get_tracks(&t0, &t1) != 0)
	return;

    /* make sure there is at least one audio track */
    for (i = t0; i <= t1; i++)
	if (cd_is_audio(i)) break;
    
    if (i > t1) return;

    while (1) {
	rnd = longrand(rnd);
	i = (rnd % t1) + 1;
	

	if ((i < t0) || !cd_is_audio(i))
	    continue;
	
	cd_play_from(i);
	break;
    }
}


void rpcd_poll()
{
    if (cd_current_track() == 0)
	if (cd_get_tracks(NULL, NULL))
	    rpcd_play_random_track();
}


void rpcd_stop()
{
    cd_stop();
}


#else


/*
 * Don't want libcda.
 */

void rpcd_init() {}
void rpcd_shutdown() {}
void rpcd_play_random_track() {}
void rpcd_poll() {}
void rpcd_stop() {}


#endif /* NO_LIBCDA_CODE */
