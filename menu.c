#include <ctype.h>
#include <string.h>
#include <dir.h>
#include <allegro.h>
#include "common.h"
#include "run.h"
#include "sk.h"
#include "blood.h"



/* menu stuff */

typedef struct BLUBBER
{
    void (*proc)(struct BLUBBER *bp, int command, int ex);
    char *string;
    void *thing;
    int d1, d2;
} BLUBBER;

BLUBBER *cur;
FONT    *big,       // in -1 colour
	*small;     // in white
int     end;
int     count, top;



/* procs */

// dummy
void prev(BLUBBER *bp, int command, int ex) {} 

// switch to another node of menu
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
    top = 110 - count/2 * 32;
}

// connect between menues
void join(BLUBBER *bp, int command, int ex)
{
    switch (command)
    {
	case MSG_DRAW:
	    // ex will be y pos
	    textout_centre(dbuf, big, bp->string, 160, ex, -1);
	    break;

	case MSG_CLICK:
	    enter_menu(bp->thing);
	    break;
    }
}

// run a func if this item selected
void func(BLUBBER *bp, int command, int ex)
{
    switch (command)
    {
	case MSG_DRAW:
	    // ex will be y pos
	    textout_centre(dbuf, big, bp->string, 160, ex, -1);
	    break;

	case MSG_CLICK:
	    ((void (*)())bp->thing)();
	    break;
    }
}



/* funcs */

BLUBBER root_menu[];
BLUBBER startgame_menu[];


// super dooper zapper creds

void credits(); //creds.c

void credits_func()
{
    credits();
}

// bye bye
void quit_func()
{
    end = 1;
}



/* game starters funcs */

// helpers to get name and colour of local player

#define MAX_NAME_LEN    16

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
	textout_centre(dbuf, small, "ENTER YOUR HANDLE:", 160, 70, makecol(255,255,255));
	textout_centre(dbuf, small, temp, 160, 90, makecol(255,255,255));
	blit(dbuf, screen, 0, 0, 0, 0, 320, 200);

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
	else if (len < MAX_NAME_LEN)    // ascii key
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
	textout_centre(dbuf, small, "CHOOSE YOUR COLOUR:", 160, 70, makecol(255,255,255));
	blit(dat[BLOB000 + local_colour].dat, dbuf, 0, 0, 160-8, 100-8, 16, 16);
	textout_centre(dbuf, small, colours[local_colour], 160, 110, makecol(255,255,255));
	blit(dbuf, screen, 0, 0, 0, 0, 320, 200);

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


// helpers to select maps

#define MAX_MAPS    60

struct
{
    char fn[80];
    char on_local;
} map_list[MAX_MAPS];

int num_maps;

// retrieve map filenames from ./maps/*.wak
void get_map_filenames()
{
    struct ffblk f;
    int i;

    num_maps = 0;

    for (i=0; i<MAX_MAPS; i++)
    {
	if ((i==0 && findfirst("maps/*.wak", &f, FA_RDONLY)) ||
	    (i>0 && findnext(&f)))
	{
	    map_list[i].fn[0] = 0;
	    return;
	}

	strcpy(map_list[i].fn, "MAPS/");
	strcat(map_list[i].fn, f.ff_name);
	map_list[i].on_local = 1;
	num_maps++;
    }
}

// allow map selection
// w/ chat box 
char *select_map()
{
    int top = 0, selected = 0, i, k, y;
    int chatting = 0, line = -1;
    char chatbox[6][80] = { "", "", "", "", "", "" };
    char cur[80] = "", temp[80], ch;

    for (;;)
    {
	clear(dbuf);

	// map select

	textout(dbuf, small, "GRAVEYARD", 20, 10, RED);
	textout_centre(dbuf, small, "UP/DOWN TO SELECT, F10 TO START", 160, 120, RED);

	for (i=0; i<9; i++) {
	    if (i<num_maps && map_list[top+i].fn[0])
	    {
		int c = WHITE;
		if (top+i==selected)
		    c = YELLOW;
		textout(dbuf, small, get_filename(map_list[top+i].fn), 20, 25 + i*10, c);
	    }
	}
	textout(dbuf, small, "*", 10, 25 + (selected-top)*10, RED);

	// player list

	textout(dbuf, small, "THE DAMNED", 180, 10, RED);
	y = 20;
	for (i=0; i<num_players; i++)
	{
	    blit(dat[players[i].colour + BLOB000].dat, dbuf, 0, 0, 180, y, 16, 16);
	    textout(dbuf, small, players[i].name, 200, y + 5, WHITE);
	    y += 20;
	}

	// chat box

	hline(dbuf, 0, 130, 319, RED);
	hline(dbuf, 0, 140, 319, RED);
	hline(dbuf, 0, 190, 319, RED);
	textout(dbuf, small, "CHAT BOX", 10, 133, WHITE);

	for (i=0; i<6; i++)
	    textout(dbuf, small, chatbox[i], 0, 143 + i * 8, WHITE);

	if (chatting)
	{
	    textout(dbuf, small, cur, 0, 192, WHITE);
	    textout(dbuf, small, "_", text_length(small, cur), 192, WHITE);
	}

	// splat onto screen
	blit(dbuf, screen, 0, 0, 0, 0, 320, 200);

	// wait for keypress
	k = readkey();

	// down arrow
	if ((k>>8) == KEY_DOWN)
	{
	    if (++selected>=num_maps)
		selected--;
	    else if (selected>=top+9)
		top++;
	}

	// up arrow
	if ((k>>8) == KEY_UP)
	{
	    if (--selected < 0)
		selected = 0;
	    else if (selected<top)
		top--;
	}

	// esc... get out
	if ((k>>8) == KEY_ESC)
	    return NULL;

	// f10.. start game
	if ((k>>8) == KEY_F10)
	    return map_list[selected].fn;

	// enter.. send chat
	if ((k>>8) == KEY_ENTER)
	{
	    if (chatting && cur[0])
	    {
		// scroll?
		if (++line>5) 
		{
		    for (i=0; i<5; i++)
			strcpy(chatbox[i], chatbox[i+1]);
		    line--;
		}

		strcpy(temp, players[local].name);
		strcat(temp, ":");
		strcat(temp, cur);
		strcpy(chatbox[line], temp);
		cur[0] = 0;

		chatting = 0;
	    }
	}

	// if backspace
	if ((k>>8) == KEY_BACKSPACE && chatting)
	{
	    i = strlen(cur);
	    if (i)
		cur[i-1] = 0;
	}

	// else, into chat box
	ch = toupper(k & 0xff);
	if (ch >= ' ' && ch <= '~')
	{
	    i = strlen(cur);
	    if (i<47)
	    {
		cur[i] = ch;
		cur[i+1] = 0;
		chatting = 1;
	    }
	}
    } 
}



/* connect via serial */

#define SER_CONNECTPLS  '?'
#define SER_CONNECTOK   '!'
#define SER_THROWDICE   '@'

int linkup()
{
    int x, y;

    #define LINKUP_MSG  "LINKING UP (PRESS ESC TO ABORT)"

    textout(screen, dat[MINI].dat, LINKUP_MSG, 16, 32, WHITE);
    x = text_length(dat[MINI].dat, LINKUP_MSG) + 16;
    y = 32;

    for (;;)
    {
	skSend(SER_CONNECTPLS);
	textout(screen, dat[MINI].dat, ".", x, y, WHITE);
	if ((x+=4)>SCREEN_W-16) { y += 8; x = 16; }

	if (skRecv() == SER_CONNECTPLS)
	    break;

	speed_counter = 0;
	while (speed_counter < GAME_SPEED)
	{
	    if (key[KEY_ESC]) 
	    {
		while (key[KEY_ESC]);
		return 0; 
	    }
	}
    }

    textout(screen, dat[MINI].dat, "TOUCHED", 16, y+8, WHITE);
    return 1;
}

int connect_serial(int comport)
{
    int l = 0, r = 0;

    seed = time(NULL);
    srandom(seed);

    if (!skOpen(comport, BAUD_19200, BITS_8 | PARITY_NONE | STOP_1))
	return 0;

    clear(screen);

    switch (comport)
    {
	case COM1:
	    textout(screen, dat[MINI].dat, "COM1 OPENED", 16, 16, WHITE);
	    break;
	case COM2:
	    textout(screen, dat[MINI].dat, "COM2 OPENED", 16, 16, WHITE);
	    break;
	case COM3:
	    textout(screen, dat[MINI].dat, "COM3 OPENED", 16, 16, WHITE);
	    break;
	case COM4:
	    textout(screen, dat[MINI].dat, "COM4 OPENED", 16, 16, WHITE);
	    break;
    }

    if (skEnableFIFO())
	textout(screen, dat[MINI].dat, "16550A UART FIFO ENABLED", 16, 24, WHITE);
    else
	textout(screen, dat[MINI].dat, "FIFO NOT ENABLED", 16, 24, WHITE);

    if (!linkup(SER_CONNECTPLS))
	return 0;

    skSend(SER_THROWDICE);
    while (skRecv() != SER_THROWDICE);

    do
    {
	l = (random()%255) + 1;     // 1-255
	skSend(l);

	while (!skReady())
	{
	    if (key[KEY_ESC]) 
	    {
		while (key[KEY_ESC]);
		return 0; 
	    }
	}

	r = skRecv();
    } while (l==r);

    if (l>r)    // l>r we win
    {
	local = 0;
	send_long(seed);
    }
    else        // l<r we lose
    {
	local = 1;
	seed = recv_long();
    }

    num_players = 2;
    comm = serial;
    return 1;
}

void serial_func()
{
    char *fn;

    // name and colour
    if (!get_name() || !get_colour())
	return;

    // connect
    if (!connect_serial(COM2)) {
	skClose();
	return;
    }

    // set up few things
    seed = time(NULL);
    srandom(seed);
    next_position = random()%(24*24);
    rnd_index = random()%600;
    irnd_index = random()%600;

    no_germs();
    strcpy(players[local].name, local_name);
    players[local].colour = local_colour;

    // get list of maps (from local and remote)
    get_map_filenames();
    // FIXME

    // select level
    fn = select_map();
    if (!fn) return;
    load_map(fn);

    // game loop
    spawn_players();
    game_loop();

    // back to root menu
    enter_menu(root_menu); 
}



/* lonely */

void single_func()
{
    char *fn;

    // name and colour
    if (!get_name() || !get_colour())
	return;

    // set up few things
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

    // select level
    get_map_filenames();
    fn = select_map();
    if (!fn) return;
    load_map(fn);

    // game loop
    spawn_players();
    game_loop();

    // back to root menu
    enter_menu(root_menu);
}



/* not yet, okay? */

void not_yet_func()
{
    show_mouse(NULL);
    clear(screen);
    textout_centre(screen, small, "SORRY,", 160, 50, makecol(255,255,255));
    textout_centre(screen, small, "THIS FEATURE HAS NOT BEEN IMPLEMENTED YET.", 160, 70, makecol(255,255,255));
    textout_centre(screen, small, "WAIT UNTIL I GET THE TIME", 160, 90, makecol(255,255,255));
    textout_centre(screen, small, "OR HELP ME OUT.", 160, 110, makecol(255,255,255));
    textout_centre(screen, small, "ALL MAJOR CREDIT CARDS ACCEPTED. :)", 160, 130, makecol(255,255,255));
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

BLUBBER root_menu[] =
{
    { join, "Start", startgame_menu },
    { func, "Credits", credits_func },
    { func, "Quit", quit_func },
    { prev, "", NULL }
};



/* manager */

int inline touch(int item)
{
    if (mouse_y >= top + item*32 + text_height(big)/2 - 16 && mouse_y <= top + item*32 + text_height(big)/2 + 16)
	return 1;
    return 0;
}

void blubber(BLUBBER *start)
{
    BLUBBER *bp;
    int i, y, spoty = 0;

    end = 0;
    enter_menu(start);

    while (!end)
    {
	// show menu
	color_map = &light_map;
	blit(dat[TITLE].dat, dbuf, 0, 0, 0, 0, 320, 200);
	i = 0;
	y = top;
	do 
	{
	    bp = &cur[i];
	    if (touch(i) && bp->proc != prev)
		spoty = y + text_height(big)/2 - 192/2;
	    i++;
	    bp->proc(bp, MSG_DRAW, y);
	    y += 32;
	} while (bp->proc != prev);

	// stack on spotlight
	clear(light);
	blit(dbuf, light, 160 - 192/2, spoty, 160 - 192/2, spoty, 192, 192);
	draw_trans_sprite(light, dat[L_SPOT].dat, 160 - 192/2, spoty);

	// blit to screen
	vsync();
	show_mouse(NULL);
	blit(light, screen, 0, 0, 0, 0, 320, 200);
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



/* to start it all */

void menu()
{
    big = dat[UNREAL].dat;
    small = dat[MINI].dat;
    text_mode(-1);
    blubber(root_menu);
}