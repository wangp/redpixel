#include <ctype.h>
#include <string.h>
#include <allegro.h>
#include "common.h"
#include "run.h"
#include "blood.h"



// menu stuff

typedef struct BLUBBER
{
    void (*proc)(struct BLUBBER *bp, int command, int ex);
    char *string;
    void *thing;
    int d1, d2;
} BLUBBER;

BLUBBER *cur;
FONT    *fnt;
int     end;
int     textcol;
int     count, top;



/* procs */

// dummy
void prev(BLUBBER *bp, int command, int ex) {}

void enter_menu(BLUBBER *bp)
{
    int i = 0;

    cur = bp;

    // count items
    count = 0;
    do 
    {
	bp = &cur[i++];
	count++;
    } while (bp->proc != prev);

    // offset top accordingly
    top = SCREEN_H/2 - count/2 * 32;
}

void join(BLUBBER *bp, int command, int ex)
{
    switch (command)
    {
	case MSG_DRAW:
	    // ex will be y pos
	    textout_centre(dbuf, fnt, bp->string, SCREEN_W/2, ex, textcol);
	    break;

	case MSG_CLICK:
	    enter_menu(bp->thing);
	    break;
    }
}

void func(BLUBBER *bp, int command, int ex)
{
    switch (command)
    {
	case MSG_DRAW:
	    // ex will be y pos
	    textout_centre(dbuf, fnt, bp->string, SCREEN_W/2, ex, textcol);
	    break;

	case MSG_CLICK:
	    ((void (*)())bp->thing)();
	    break;
    }
}

void slider(BLUBBER *bp, int command, int ex)
{
}



/* funcs */

BLUBBER root_menu[];
BLUBBER startgame_menu[];
BLUBBER options_menu[];


void credits_func()
{
}

void quit_func()
{
    end = 1;
}



// helpers 

#define MAX_NAME_LEN    24

char local_name[MAX_NAME_LEN+1] = "NO NAME";
int local_colour = 0;

char *colours[] = { 
    "RED", "BLOOD RED", "ORANGE", "YELLOW", "GREEN", "CYAN", "BLUE",
    "LIGHT BLUE", "PURPLE BLUE", "PURPLE", "MAROON", "BROWN", "DIARRHOEA",
    "GRAY", "WHITE", "PINK"
};

int get_name()
{
    int k, len, ch;
    char temp[80];

    show_mouse(NULL);
    clear_keybuf();

    for (;;)
    {
	// screen output
	strcpy(temp, local_name);
	strcat(temp, "_");
	clear(dbuf);
	textout_centre(dbuf, dat[MINI].dat, "ENTER YOUR HANDLE:", SCREEN_W/2, 70, makecol(255,255,255));
	textout_centre(dbuf, dat[MINI].dat, temp, SCREEN_W/2, 90, makecol(255,255,255));
	blit(dbuf, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

	// wait for keypress
	k = readkey();

	// enter.. continue
	if ((k>>8) == KEY_ENTER) {
	    while (key[KEY_ENTER]);
	    return 1;
	}

	// esc.. get out
	if ((k>>8) == KEY_ESC) {
	    while (key[KEY_ESC]);
	    return 0;
	}

	// otherwise entering name
	len = strlen(local_name);

	// backspace
	if ((k>>8) == KEY_BACKSPACE && len > 0)
	{
	    local_name[len-1] = 0;
	}
	else if (len <= MAX_NAME_LEN)   // ascii key
	{
	    ch = toupper(k & 0xff);
	    if (ch >= ' ' && ch <= '~') {
		local_name[len] = ch;
		local_name[len+1] = 0;
	    }
	}
    }
}

int get_colour()
{
    int k;

    show_mouse(NULL);
    clear_keybuf();

    for (;;)
    {
	// to screen
	clear(dbuf);
	textout_centre(dbuf, dat[MINI].dat, "CHOOSE YOUR COLOUR:", SCREEN_W/2, 70, makecol(255,255,255));
	blit(dat[BLOB000 + local_colour].dat, dbuf, 0, 0, SCREEN_W/2-8, SCREEN_H/2-8, 16, 16);
	textout_centre(dbuf, dat[MINI].dat, colours[local_colour], SCREEN_W/2, 110, makecol(255,255,255));
	blit(dbuf, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

	// wait for keypress
	k = readkey() >> 8;

	// enter.. continue
	if (k == KEY_ENTER) {
	    while (key[KEY_ENTER]);
	    return 1;
	}

	// esc.. get out
	if (k == KEY_ESC) {
	    while (key[KEY_ESC]);
	    return 0;
	}

	// left/up
	if (k == KEY_UP || k == KEY_LEFT)
	    if (--local_colour<0) local_colour = 15;

	// down/right
	if (k == KEY_RIGHT || k == KEY_DOWN)
	    if (++local_colour==16) local_colour = 0;
    }
}



void serial_func()
{
    // name and colour
    // connect
    // select level
    // game loop
    // back to root menu
}

void single_func()
{
    // name and colour
    if (!get_name() || !get_colour())
	return;

    // select level

    // game loop
    comm = single;
    local = 0;
    num_players = 1;

    seed = time(NULL);
    srandom(seed);
    next_position = random()%(24*24);
    rnd_index = random()%600;
    irnd_index = random()%600;

    no_germs();
    strcpy(players[local].name, local_name);
    players[local].colour = local_colour;
    spawn_players();

    game_loop();

    // back to root menu
    while (key[KEY_ESC]);
    enter_menu(root_menu);
}

void not_yet_func()
{
    show_mouse(NULL);
    clear(screen);
    textout_centre(screen, dat[MINI].dat, "SORRY,", SCREEN_W/2, 50, makecol(255,255,255));
    textout_centre(screen, dat[MINI].dat, "THIS FEATURE HAS NOT BEEN IMPLEMENTED YET.", SCREEN_W/2, 70, makecol(255,255,255));
    textout_centre(screen, dat[MINI].dat, "WAIT UNTIL I GET THE TIME", SCREEN_W/2, 90, makecol(255,255,255));
    textout_centre(screen, dat[MINI].dat, "OR HELP ME OUT.", SCREEN_W/2, 110, makecol(255,255,255));
    textout_centre(screen, dat[MINI].dat, "ALL MAJOR CREDIT CARDS ACCEPTED. :)", SCREEN_W/2, 130, makecol(255,255,255));
    while (!mouse_b && !key[KEY_ESC]);
    while (mouse_b || key[KEY_ESC]);
}



/* menus */

BLUBBER startgame_menu[] = 
{
    { func, "Solo", single_func },
    { func, "Serial", serial_func },
    { func, "IPX Network", not_yet_func },
    { func, "Internet", not_yet_func },
    { prev, "", root_menu }
};

BLUBBER options_menu[] =
{
    //{ slider, "Sound Volume", sound_volume_slider, 0, 10 },
    //{ slider, "CD Volume", cd_volume_slider, 0, 10 },
    { prev, "", root_menu }
};

BLUBBER root_menu[] =
{
    { join, "Kill!", startgame_menu },
    { join, "Options", options_menu },
    { func, "Credits", credits_func },
    { func, "Quit", quit_func },
    { prev, "", NULL }
};



/* manager */

int inline touch(int item)
{
    if (mouse_y >= top + item*32 + text_height(fnt)/2 - 16 && mouse_y <= top + item*32 + text_height(fnt)/2 + 16)
	return 1;
    return 0;
}

void blubber(BLUBBER *start)
{
    BLUBBER *bp;
    int i, y, spoty = 0;

    end = 0;
    enter_menu(start);

    color_map = &light_map;

    while (!end)
    {
	// show menu
	blit(dat[TITLE].dat, dbuf, 0, 0, 0, 0, 320, 200);
	i = 0;
	y = top;
	do 
	{
	    bp = &cur[i];
	    if (touch(i) && bp->proc != prev)
		spoty = y + text_height(fnt)/2 - 192/2;
	    i++;
	    bp->proc(bp, MSG_DRAW, y);
	    y += 32;
	} while (bp->proc != prev);

	// stack on spotlight
	clear(light);
	blit(dbuf, light, SCREEN_W/2 - 192/2, spoty, SCREEN_W/2 - 192/2, spoty, 192, 192);
	draw_trans_sprite(light, dat[L_SPOT].dat, SCREEN_W/2 - 192/2, spoty);

	// blit to screen
	vsync();
	show_mouse(NULL);
	blit(light, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
	show_mouse(screen);

	// click
	if (mouse_b & 1)
	{
	    for (i=0; i<count; i++)
	    {
		// if touch menu item
		if (touch(i))
		{
		    // do action
		    while (mouse_b);
		    cur[i].proc(&cur[i], MSG_CLICK, 0);
		}
	    }
	}

	// prev menu
	if (key[KEY_ESC] || mouse_b & 2)
	{
	    while (key[KEY_ESC] || mouse_b & 2);

	    i = 0;
	    do {
		bp = &cur[i++];
	    } while (bp->proc != prev);

	    if (bp->thing)
		enter_menu(bp->thing);
	}
    }
}

void menu()
{
    fnt = dat[UNREAL].dat;
    textcol = -1; //makecol(255,0,0);
    text_mode(-1);
    blubber(root_menu);
}