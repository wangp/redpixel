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
#include "rpcd.h"


int recv_demo_inputs()
{
    int num = num_players, x;
    char packet[10];

    while (num) {
	x = demo_read_next_packet_type();

	if (x == DEMO_FRAME_DATA) {
	    demo_read_packet(packet, 4);
	    load_playerstat(packet);
	    num--;
	}
	else if (x == DEMO_MAP_CHANGE) {
	    char filename[1024], *f;

	    demo_read_string(filename, sizeof filename);
	    if (load_map_wrapper(filename) < 0) {
		alert("Error loading map", filename, "", "Damn", NULL, 13, 27);
		return -1;
	    }

	    rpcd_play_random_track();

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
	else if (x == DEMO_END || x < 0)
	    return -1;
    }

    return 0;
}
