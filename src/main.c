/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Main.
 */


#include <ctype.h>
#include <time.h>
#include <allegro.h>
#include "main.h"
#include "engine.h"
#include "blood.h"
#include "fblit.h"
#include "globals.h"
#include "intro.h"
#include "launch.h"
#include "mapper.h"
#include "menu.h"
#include "music.h"
#include "resource.h"
#include "setweaps.h"
#include "sk.h"
#include "stats.h"
#include "statlist.h"
#include "version.h"
#include "winhelp.h"


int record_demos = 0;
int filtered = 0;


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


static void show_version()
{
    allegro_message("Red Pixel " VERSION_STR " by Psyk Software " VERSION_YEAR ".\n"
		    "http://www.alphalink.com.au/~tjaden/\n");
}


/* see also scrblit.c */
static int set_video_mode(void)
{
    if (set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, 320, 200, 0, 0) == 0)
	goto done;
    
    if (set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, 320, 240, 0, 0) == 0) {
	set_mouse_range(0, 20, 319, 219);
	goto done;
    }
    
    if (set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, 640, 400, 0, 0) == 0)
	goto done;
    
    if (set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, 640, 480, 0, 0) == 0) {
	set_mouse_range(0, 40, 640, 439);
	goto done;
    }
    
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 400, 0, 0) == 0)
	goto done;
    
    if (set_gfx_mode(GFX_AUTODETECT, 320, 200, 0, 0) < 0)
	return -1;
    
  done:
    
    clear_bitmap(screen);
    return 0;
}



static BITMAP *stretched_mouse_sprite;

static void free_stretched_mouse_sprite(void)
{
    if (stretched_mouse_sprite) {
	destroy_bitmap(stretched_mouse_sprite);
	stretched_mouse_sprite = NULL;
    }
}

static void set_stretched_mouse_sprite(BITMAP *sprite, int factor, int hotx, int hoty)
{
    free_stretched_mouse_sprite();
    stretched_mouse_sprite = create_bitmap(sprite->w * factor, sprite->h * factor);
    clear_bitmap(stretched_mouse_sprite);
    stretch_blit(sprite, stretched_mouse_sprite, 0, 0, sprite->w, sprite->h,
		 0, 0, stretched_mouse_sprite->w, stretched_mouse_sprite->h);
    set_mouse_sprite(stretched_mouse_sprite);
    set_mouse_sprite_focus(hotx * factor, hoty * factor);
}


/* "shutdown" is a sockets function, which gets called 
 * when we exit if we're under X, which is no good.  */
void main_shutdown()
{
    engine_shutdown();
    
    unload_dat();
    free_stretched_mouse_sprite();
    destroy_bitmap(dbuf);
    destroy_bitmap(light);
    
    music_shutdown();
}


int main(int argc, char *argv[])
{
    int skip_intro = 0;
    int lcd_cur = 0;
    int alt_stats = 0;
    int mute = 0;
    int editor = 0;
    int c;

    /* command line args  */
    opterr = 0;
    
    while (1) {
	char *options = ":hveqldmfs:";
	    
	c = getopt(argc, argv, options);
	if (c < 0) break;
	    
	switch (c) {
	    case 'h': usage(options); return 0;
	    case 'v': show_version(); return 0;
	    case 'e': editor = 1; break;
	    case 'q': skip_intro = 1; break;
	    case 'l': lcd_cur = 1; break;
	    case 'd': record_demos = 1; break;
	    case 'm': mute = 1; break;
	    case 'f': filtered = 1; break;
	    case 's': 
	    	if (!read_stats(optarg, stat_block)) {
		    allegro_message("Error reading `%s'.\n", optarg);
		    return 1;
		}
		    
		set_menu_message(get_filename(optarg));
		alt_stats = 1;
		break;
	    
	    case ':':
	    	allegro_message("Option `-%c' missing argument.\n", optopt);
	    	return 1;

	    case '?': 
		allegro_message("Unknown option `-%c'.\n", optopt);
	    	return 1;
	    
	    default:
		return 1;
	}
    }

    /* initialisation */
    srand(time(0));

    allegro_init();
    set_window_title("Red Pixel");
    
    install_timer();
    install_keyboard();

    if (install_mouse() <= 0) {
        allegro_message("\nRed Pixel requires a mouse to play.\n"
#if defined(TARGET_DJGPP)
			"Please install a mouse driver (e.g. mouse.com).\n\n"
#elif defined(TARGET_LINUX)
			"Please check your Allegro mouse port settings\n"
			"and make sure you have permissions to the device.\n\n"
#endif
	               );
	return 1;
    }
    
    if (!mute) {
	reserve_voices(32, -1);
	install_sound(DIGI_AUTODETECT, MIDI_NONE, NULL);
	music_init();
    }

    /* set up game path */
    set_game_path(argv[0]);
    
    {
	char tmp[MAX_PATH_LENGTH];
	skSetConfigPath(replace_filename(tmp, argv[0], "", sizeof tmp));
    }

    /* load datafile */
    if (load_dat() < 0) {
	allegro_message("Error loading blood.dat\n");
	return 1;
    }
    
    /* map editor option */
    if (editor) {
	int x = mapper();
	main_shutdown();
	return x;
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

    if (set_video_mode() < 0) {
	main_shutdown();
	allegro_message("Error setting video mode.\n"
#ifdef TARGET_LINUX
			"Perhaps the executable does not have root permissions?\n"
#endif
			);
	
	return 1;
    }
    
    set_palette(dat[GAMEPAL].dat);
    if (filtered)
	fblit_init(dat[GAMEPAL].dat);
    
    dbuf = create_bitmap(320, 200);
    light = create_bitmap(320, 200);
    set_stretched_mouse_sprite(dat[lcd_cur ? XHAIRLCD : XHAIR].dat,
			       (SCREEN_W == 640) ? 2 : 1, 2, 2);
    set_mouse_speed(1, 1);

    if (!skip_intro)
	intro();

    main_menu();

    main_shutdown();

    return 0;
}

END_OF_MAIN()
