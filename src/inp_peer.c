/* 
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 * 
 *  Peer-peer input.
 */


#include <allegro.h>
#include "demo.h"
#include "demintro.h"
#include "gameloop.h"
#include "map.h"
#include "message.h"
#include "packet.h"
#include "player.h"
#include "sk.h"


void send_local_input()
{
    char packet[20];
    
    if (num_players < 2 && !demo_is_recording())
	return;
    
    packet[0] = PACKET_PLAYERSTAT;
    packet[1] = local;		

    packet[2] = 0;
    if (players[local].up)		packet[2] |= INPUT_UP;
    if (players[local].down) 		packet[2] |= INPUT_DOWN;
    if (players[local].left)		packet[2] |= INPUT_LEFT;
    if (players[local].right) 		packet[2] |= INPUT_RIGHT;
    if (players[local].fire)		packet[2] |= INPUT_FIRE;
    if (players[local].drop_mine)	packet[2] |= INPUT_DROPMINE;
    if (players[local].respawn)		packet[2] |= INPUT_RESPAWN;
    if (players[local].facing == left)  packet[2] |= INPUT_FACINGLEFT;	

    packet[3] = players[local].select;
    packet[4] = (players[local].angle >> 16) & 0xff;

    if (comm == peerpeer)
	skWrite(packet, 5);

    demo_write_frame_data(packet + 1, 4);
}


int recv_remote_inputs()
{
    char packet[20];
    int pl, ch;

    while (1) {
	while (!skReady())
	    ;

	switch (skRecv()) {
	    
	    case PACKET_PLAYERSTAT: 
		while (skReady() < 4)
		    ;
		skRead(packet, 4);

		pl = packet[0];

		ch = packet[1];
		players[pl].up        = ch & INPUT_UP;
		players[pl].down      = ch & INPUT_DOWN;
		players[pl].left      = ch & INPUT_LEFT;
		players[pl].right     = ch & INPUT_RIGHT;
		players[pl].fire      = ch & INPUT_FIRE;
		players[pl].drop_mine = ch & INPUT_DROPMINE;
		players[pl].respawn   = ch & INPUT_RESPAWN;
		players[pl].facing = (ch & INPUT_FACINGLEFT) ? left : right;

		players[pl].select = packet[2];
		players[pl].angle = packet[3] << 16;

		demo_write_frame_data(packet, 4);
		return 0;

	    case PACKET_QUITGAME:
		skSend(PACKET_QUITOK);
		return -1;

	    default:
		add_msg("SYNC PROBLEM!", -1);
		break;
	}
    }
}
