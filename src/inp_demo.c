/* 
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 * 
 *  Demo file input.
 */


#include <string.h>
#include <allegro.h>
#include "demo.h"
#include "demintro.h"
#include "map.h"
#include "packet.h"
#include "player.h"


int recv_demo_inputs()
{
    int pl, ch, x;
    int num = num_players;
    char packet[20];

    while (num) {
	x = demo_read_next_packet_type();

	if (x == DEMO_FRAME_DATA) {
	    demo_read_packet(packet, 4);

	    pl = packet[0];

	    ch = packet[1];
	    players[pl].up     	  = ch & INPUT_UP; 
	    players[pl].down  	  = ch & INPUT_DOWN;
	    players[pl].left  	  = ch & INPUT_LEFT;
	    players[pl].right 	  = ch & INPUT_RIGHT;
	    players[pl].fire  	  = ch & INPUT_FIRE;
	    players[pl].drop_mine = ch & INPUT_DROPMINE;
	    players[pl].respawn   = ch & INPUT_RESPAWN;
	    
	    players[pl].facing = (ch & INPUT_FACINGLEFT) ? left : right;

	    players[pl].select = packet[2];
	    players[pl].angle = packet[3] << 16;

	    num--;
	}
	else if (x == DEMO_MAP_CHANGE) {
	    char filename[1024], *f;

	    demo_read_string(filename, sizeof filename);
	    if (load_map_wrapper(filename) < 0) {
		alert("Error loading map", filename, "", "Damn", NULL, 13, 27);
		return -1;
	    }

	    retain_players();
	    reset_engine();
	    restore_players();
	    spawn_players();

	    f = strstr(filename, ".WAK");
	    if (f)
		*f = 0;
	    introduce_map(filename);

	    speed_counter = 0;
	}
	else if (x == DEMO_END)
	    return -1;
    }

    return 0;
}
