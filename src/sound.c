/* 
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Sound helpers.
 */


#include "a4aux.h"
#include "blood.h"
#include "engine.h"
#include "gameloop.h"
#include "globals.h"
#include "options.h"
#include "player.h"
#include "sound.h"


void snd_local(int snd)
{
    if (!mute_sfx)
	play_sample(dat[snd].dat, 255, 128, 1000, 0);
}


void snd_3d(int snd, int maxvol, int sourcex, int sourcey)
{
    int dist, pan;

    if (mute_sfx)
	return;

    dist = find_distance(players[local].x, players[local].y, sourcex, sourcey) / 2;

    if (sourcex < players[local].x)
	pan = 128 - dist * 3;
    else
	pan = 128 + dist * 3;

    pan = MID(0, pan, 255);

    maxvol -= dist;
    if (maxvol > 0)
	play_sample(dat[snd].dat, maxvol, pan, 1000, 0);
}

