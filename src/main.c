/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Main.
 */


#include <ctype.h>
#include <time.h>
#include "a4aux.h"
#include "main.h"
#include "engine.h"
#include "blood.h"
#include "globals.h"
#include "intro.h"
#include "launch.h"
#include "mapper.h"
#include "menu.h"
#include "mousespr.h"
#include "options.h"
#include "resource.h"
#include "rpagup.h"
#include "setweaps.h"
#include "sk.h"
#include "stats.h"
#include "statlist.h"
#include "version.h"
#include "vidmode.h"
#include "winhelp.h"


/*----------------------------------------------------------------------*/
/*      Misc                                                            */
/*----------------------------------------------------------------------*/

static void usage(char *options)
{
    char buf[40] = "";
    int i, j;
    
    for (i = 0, j = 0; options[i]; i++)
	if (isalpha(options[i]))
	    buf[j++] = options[i];
    buf[j] = '\0';
    
    allegro_message("See manual for help on these options: %s\n", buf);
}


static void show_version(void)
{
    allegro_message("Red Pixel " VERSION_STR " by Psyk Software " VERSION_YEAR ".\n"
		    "http://redpixel.sourceforge.net/\n");
}


/*----------------------------------------------------------------------*/
/*      Init / main                                                     */
/*----------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
    int skip_intro = 0;
    int editor = 0;

    /* command line args  */
    opterr = 0;
    
    while (1) {
	char *options = ":hveq";
	    
	int c = getopt(argc, argv, options);
	if (c < 0) break;
	    
	switch (c) {
	    case 'h': usage(options); return 0;
	    case 'v': show_version(); return 0;
	    case 'e': editor = 1; break;
	    case 'q': skip_intro = 1; break;
	    
	    case ':':
	    	allegro_message("Option missing argument.\n");
	    	return 1;
		
	    case '?': 
		allegro_message("Unknown option.\n");
	    	return 1;
	    
	    default:
		return 1;
	}
    }

    /* initialisation */
    srand(time(0));

    if (allegro_init() != 0) {
	allegro_message("Error initialising Allegro.\n");
	return 1;
    }
    set_window_title("Red Pixel");
    
    install_timer();
    install_keyboard();

    if (install_mouse() <= 0) {
        allegro_message(
	    "\nRed Pixel requires a mouse to play.\n"
#if defined(TARGET_DJGPP)
	    "Please install a mouse driver (e.g. mouse.com).\n\n"
#elif defined(TARGET_LINUX)
	    "Please check your Allegro mouse port settings\n"
	    "and make sure you have permissions to the device.\n\n"
#endif
        );
	return 1;
    }
    
    reserve_voices(32, -1);
    install_sound(DIGI_AUTODETECT, MIDI_NONE, NULL);

    /* set up game path */
    set_game_path(argv[0]);
    
    {
	char tmp[MAX_PATH_LENGTH];
	skSetConfigPath(replace_filename(tmp, argv[0], "", sizeof tmp));
    }

    load_settings();

    /* map editor option */
    if (editor) {
	int x = mapper();
	main_shutdown();
	return x;
    }

    if ((!read_stats(current_stats, stat_block)) &&
	(!read_stats(get_resource(R_SHARE, current_stats), stat_block))) {
	char *p = get_resource(R_SHARE, "stats/default.st");
	
	if (!read_stats(p, stat_block)) {
	    allegro_message("Error reading %s.\n", p);
	    return 1;
	}

	set_current_stats("stats/default.st");
    }
    set_weapon_stats();

    /* video mode */
    vidmode_init();
    if (set_video_mode() < 0) {
	main_shutdown();
	allegro_message(
	    "Error setting video mode.\n"
#ifdef TARGET_LINUX
	    "Perhaps the executable does not have root permissions?\n"
#endif
	);
	return 1;
    }

    /* load datafile */
    if (load_dat() < 0) {
	allegro_message("Error loading blood.dat\n");
	main_shutdown();
	return 1;
    }

    set_palette(dat[GAMEPAL].dat);

    rpagup_init();
    
    /* init game engine */
    engine_init();
   
    setup_lighting();

    dbuf = create_bitmap(320, 200);
    light = create_bitmap_with_margin(320, 200, 5, 5);

    set_stretched_mouse_sprite(dat[XHAIRLCD].dat, (SCREEN_W == 640) ? 2 : 1, 2, 2);
#if __A4__
    set_mouse_speed(mouse_speed, mouse_speed);
#endif

    /* begin */

    if (!skip_intro)
	intro();

    main_menu();

    main_shutdown();

    save_settings();

    return 0;
}

END_OF_MAIN();


/*----------------------------------------------------------------------*/
/*      Shutdown                                                        */
/*----------------------------------------------------------------------*/

/* "shutdown" is a sockets function, which gets called 
 * when we exit if we're under X, which is no good.  */
void main_shutdown(void)
{
    set_mouse_sprite(NULL);
    free_stretched_mouse_sprite();
    if (light) {
	al_destroy_bitmap(light);
	light = NULL;
    }
    if (dbuf) {
	destroy_bitmap(dbuf);
	dbuf = NULL;
    }
    engine_shutdown();
    rpagup_shutdown();
    unload_dat();
    vidmode_shutdown();
}
