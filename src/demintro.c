/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 *
 *  Demo playback intermission screens.
 */


#include "a4aux.h"
#include "blood.h"
#include "colours.h"
#include "demo.h"
#include "demintro.h"
#include "engine.h"
#include "globals.h"
#include "player.h"
#include "vidmode.h"


/* Although the real screen can be a different size, the screen we
 * work with is always 320x200.  */
#define screen_w	320
#define screen_h	200


static void fade_in_and_out(BITMAP *buf, int wait, int allow_int)
{
    int i;
    
    /* Fade in, then out.  */
    rp_hide_mouse();
    rp_fade_in(buf, 8);
    
    /* wait a bit */
    for (i = wait / 100; i && (allow_int ? (!keypressed() && !mouse_b) : 1); i--)
	rest(100);
        
    rp_fade_out(buf, 8);
    clear_bitmap(screen);
    
    clear_keybuf();
}



/*
 *  Player vs. Player
 *    at date, time
 */

void introduce_demo(void)
{
    BITMAP *buf;
    FONT *small = dat[MINI].dat;
    FONT *big = dat[UNREAL].dat;
    int cx = screen_w / 2, cy = screen_h / 2;
	
    buf = create_bitmap(screen_w, screen_h);
    if (buf) {
	clear_bitmap(buf);
	
	textout_centre_ex(buf, big, players[0].name, cx, cy - 40, -1, -1);
	textout_centre_ex(buf, small, "VS", cx, cy, WHITE, -1);
	textout_centre_ex(buf, big, (num_players == 2) ? players[1].name : "NOBODY",
		          cx, cy + 20, -1, -1);
	textout_centre_ex(buf, small, demo_description, cx, cy + 70, WHITE, -1);
	
	fade_in_and_out(buf, 3000, 1);
	
	destroy_bitmap(buf);
    }
}


/*
 * Show mapname and current frag count.
 */

void introduce_map(const char *mapname)
{
    BITMAP *buf;
    FONT *small = dat[MINI].dat;
    FONT *big = dat[UNREAL].dat;
    int cx = screen_w / 2, cy = screen_h / 2;
	
    buf = create_bitmap(screen_w, screen_h);
    if (buf) {
	clear_bitmap(buf);
	
	textout_centre_ex(buf, big, mapname, cx, cy - 20, -1, -1);
	
	if ((num_players == 2) && (players[0].frags || players[1].frags)) {
	    textprintf_centre_ex(buf, small, cx, cy + 70, WHITE, -1,
			         "%s - %d    %s - %d",
				 players[0].name, players[0].frags,
				 players[1].name, players[1].frags);
	}

	fade_in_and_out(buf, 1000, 0);
	
	destroy_bitmap(buf);
    }
}
