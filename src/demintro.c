/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 * 
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 * 
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Email: tjaden@psynet.net
 *  WWW:   http://www.psynet.net/tjaden/
 * 
 *  Demo playback intermission screens.
 */


#include <allegro.h>
#include "run.h"
#include "common.h"
#include "demo.h"
#include "demintro.h"
#include "blood.h"



static void fade_in_and_out(BITMAP *buf, int wait, int allow_int)
{
    int i;
    
    /* Fade in, then out.  */
    show_mouse(NULL);
    set_palette(black_palette);
    blit(buf, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    fade_in(dat[GAMEPAL].dat, 8);
    
    /* wait a bit */
    for (i = wait / 100; i && (allow_int ? (!keypressed() && !mouse_b) : 1); i--)
	rest(100);
        
    fade_out(8);
    clear(screen);
    
    /* Restore palette.  */
    set_palette(dat[GAMEPAL].dat);
    
    clear_keybuf();
}



/*
 *  Player vs. Player
 *    at date, time
 */

void introduce_demo()
{
    BITMAP *buf;
    FONT *small = dat[MINI].dat;
    FONT *big = dat[UNREAL].dat;
    int cx = SCREEN_W / 2, cy = SCREEN_H / 2;
	
    buf = create_bitmap(SCREEN_W, SCREEN_H);
    if (buf) {
	clear(buf);
	
	textout_centre(buf, big, players[0].name, cx, cy - 40, -1);
	textout_centre(buf, small, "VS", cx, cy, WHITE);
	textout_centre(buf, big, (num_players == 2) ? players[1].name : "NOBODY",
		       cx, cy + 20, -1);
	textout_centre(buf, small, demo_description, cx, cy + 70, WHITE);
	
	fade_in_and_out(buf, 3000, 1);
	
	destroy_bitmap(buf);
    }
}


/*
 * Show mapname and current frag count.
 */

void introduce_map(char *mapname)
{
    BITMAP *buf;
    FONT *small = dat[MINI].dat;
    FONT *big = dat[UNREAL].dat;
    int cx = SCREEN_W / 2, cy = SCREEN_H / 2;
	
    buf = create_bitmap(SCREEN_W, SCREEN_H);
    if (buf) {
	clear(buf);
	
	textout_centre(buf, big, mapname, cx, cy - 20, -1);
	
	if ((num_players == 2) && (players[0].frags || players[1].frags)) {
	    textprintf_centre(buf, small, cx, cy + 70, WHITE, 
			      "%s - %d    %s - %d",
			      players[0].name, players[0].frags,
			      players[1].name, players[1].frags);
	}

	fade_in_and_out(buf, 1000, 0);
	
	destroy_bitmap(buf);
    }
}
