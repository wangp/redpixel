/* 
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 * 
 *  Sound helpers.
 */


#include <allegro.h>
#include "blood.h"
#include "engine.h"
#include "gameloop.h"
#include "globals.h"
#include "player.h"
#include "sound.h"


void snd_local(int snd)
{
    play_sample(dat[snd].dat, 255, 128, 1000, 0);
}


void snd_3d(int snd, int maxvol, int sourcex, int sourcey)
{
    int dist, pan;

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
