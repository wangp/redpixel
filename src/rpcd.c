/* rpcd.c - Red Pixel libcda wrapper
 *
 * Peter Wang <tjaden@psynet.net>
 */


#ifdef LIBCDA_CODE


#include <time.h>
#include <libcda.h>
#include "rg_rand.h"


static int inited;
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


#else


/*
 * Don't want libcda.
 */

void rpcd_init() {}
void rpcd_shutdown() {}
void rpcd_play_random_track() {}


#endif /* LIBCDA_CODE */
