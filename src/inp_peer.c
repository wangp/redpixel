/* 
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
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
    char packet[10];
    
    packet[0] = PACKET_PLAYERSTAT;
    make_playerstat(packet + 1, local);
    skWrite(packet, 5);
}


int recv_remote_inputs()
{
    char packet[10];

    while (1) {
	while (!skReady())
	    ;

	switch (skRecv()) {
	    
	    case PACKET_PLAYERSTAT: 
		while (skReady() < 4)
		    ;
		skRead(packet, 4);
	    	load_playerstat(packet);
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
