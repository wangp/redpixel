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
 *  Main.
 */

#include <allegro.h>
#include "main.h"
#include "engine.h"
#include "resource.h"
#include "globals.h"
#include "menu.h"
#include "setweaps.h"
#include "intro.h"
#include "launch.h"
#include "mapper.h"
#include "sk.h"
#include "stats.h"
#include "statlist.h"
#include "version.h"
#include "blood.h"


int com_port = COM2;

int record_demos = 0;


static void usage(char *options)
{
    /* :) */
    allegro_message("See manual for help on these options: %s\n", options);
}


static void show_version(int legal)
{
    allegro_message("Red Pixel " VERSION_STR " by Psyk Software " VERSION_YEAR ".\n"
		    "http://www.psynet.net/tjaden/\n%s",
		    (legal 
		     ? ("Licensed under the GNU Lesser General Public Licence.\n"
			"See COPYING for details.\n")
		     : ""));
}


void shutdown()
{
    engine_shutdown();
    
    unload_dat();
    destroy_bitmap(dbuf);
    destroy_bitmap(light);
    allegro_exit();
}


int main(int argc, char *argv[])
{
    int skip_intro = 0;
    int lcd_cur = 0;
    int alt_stats = 0;
    int c;

    /* init allegro */
    allegro_init();
    
    if (install_mouse() <= 0) {
#ifdef TARGET_DJGPP
	allegro_message("\nRed Pixel requires a mouse to play.\n"
			"Please install a mouse driver (e.g. mouse.com).\n\n");
#else
	allegro_message("\nRed Pixel requires a mouse to play.\n"
			"Please check your Allegro mouse port settings\n"
			"and make sure you have permissions to the device.\n\n");
#endif
	return 1;
    }
    
    install_timer();
    install_keyboard();
    install_sound(DIGI_AUTODETECT, MIDI_NONE, NULL);

    /* set up game path */
    set_game_path(argv[0]);

    /* load datafile */
    if (load_dat() < 0) {
	allegro_message("Error loading blood.dat\n");
	return 1;
    }
    
    /* command line args  */
    while (1) {
	    char *options = "hveqld1234s:";
	    
	    c = getopt(argc, argv, options);
	    if (c < 0) break;
	    
	    switch (c) {
		case 'h': usage(options); return 0;
		case 'v': show_version(1); return 0;
		case 'e': return mapper();
		case 'q': skip_intro = 1; break;
		case 'l': lcd_cur = 1; break;
		case 'd': record_demos = 1; break;
		case '1': com_port = COM1; break;
		case '2': com_port = COM2; break;
		case '3': com_port = COM3; break;
		case '4': com_port = COM4; break;
		case 's': 
		    if (!optarg) {
			allegro_message("No stats file specified!\n");
			return 1;
		    }
		    else if (!read_stats(optarg, stat_block)) {
			allegro_message("Error reading %s.\n", optarg);
			return 1;
		    }
		    
		    set_menu_message(get_filename(optarg));
		    alt_stats = 1;
	 	    break;
		
		case '?':
		default:
		    return 1;
	    }
    }

    /* stats, if none read yet */
    if (!alt_stats) {
	char *p = get_resource(R_SHARE, "stats");
	
	if (!read_stats(p, stat_block)) {
	    allegro_message("Error reading %s.\n", p);
	    return 1;
	}
    }

    set_weapon_stats();

    engine_init();
   
    setup_lighting();

    /* init screen etc */
    if (set_gfx_mode(GFX_AUTODETECT, 320, 200, 0, 0) < 0) {
	shutdown();
	allegro_message("Error setting 320x200 video mode.\n"
#ifdef TARGET_LINUX
			"Perhaps the executable is not suid root?\n"
#endif
			);
	
	return 1;
    }
    
    set_window_title("Red Pixel");
    set_palette(dat[GAMEPAL].dat);
    dbuf = create_bitmap(SCREEN_W, SCREEN_H);
    light = create_bitmap(SCREEN_W, SCREEN_H);
    if (lcd_cur)
	set_mouse_sprite(dat[XHAIRLCD].dat);
    else
	set_mouse_sprite(dat[XHAIR].dat);
    set_mouse_sprite_focus(2, 2);
    set_mouse_speed(1, 1);

    if (!skip_intro)
	intro();

    main_menu();

    shutdown();
    
    show_version(0);

    return 0;
}

END_OF_MAIN()