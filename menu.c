#include <ctype.h>
#include <string.h>
#include <dir.h>
#include <stdio.h>
#include <stdarg.h>
#include <allegro.h>
#include <mss.h>
#include "common.h"
#include "run.h"
#include "skmod.h"
#include "blood.h"


//------------------------------------------------------ menu `system' -------

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


//------------------------------------------------------ ditto ---------------

BLUBBER root_menu[];
BLUBBER startgame_menu[];
BLUBBER modem_menu[];


//------------------------------------------------------ procs ---------------

void prev(BLUBBER *bp, int command, int ex) { /* i am a dummy */ } 

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


//------------------------------------------------------ funcs ---------------

void credits(); //creds.c

void credits_func()
{
    // super dooper zapper creds
    credits();
}

void quit_func()
{
    end = 1;
}


//------------------------------------------------------ whatta your name ----

#define MAX_NAME_LEN    16
char local_name[MAX_NAME_LEN+1] = "NO NAME";

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
	    play_sample(dat[WAV_TYPE].dat, 100, 128, 1000, 0);
	}
	else if (len < MAX_NAME_LEN)    // ascii key
	{
	    ch = toupper(k & 0xff);
	    if (ch >= ' ' && ch <= '~') {
		local_name[len] = ch;
		local_name[len+1] = 0;
	    }
	    play_sample(dat[WAV_TYPE].dat, 100, 128, 1000, 0);
	}
    }
}


//------------------------------------------------------ phone no. -----------

#define MAX_PHONENO_LEN    16
char phoneno[MAX_PHONENO_LEN+1] = "";

int get_phoneno()
{
    int k, len, ch;
    char temp[80];

    show_mouse(NULL);
    clear_keybuf();

    for (;;)
    {
	// screen output
	strcpy(temp, phoneno);
	strcat(temp, "_");
	clear(dbuf);
	textout_centre(dbuf, small, "NUMBER TO DIAL:", 160, 70, makecol(255,255,255));
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
	len = strlen(phoneno);

	// backspace
	if ((k>>8) == KEY_BACKSPACE && len > 0)
	{
	    phoneno[len-1] = 0;
	    play_sample(dat[WAV_TYPE].dat, 100, 128, 1000, 0);
	}
	else if (len < MAX_NAME_LEN)    // ascii key
	{
	    ch = toupper(k & 0xff);
	    if (ch >= '#' && ch <= 'F') {
		phoneno[len] = ch;
		phoneno[len+1] = 0;
	    }
	    play_sample(dat[WAV_TYPE].dat, 100, 128, 1000, 0);
	}
    }
}


//------------------------------------------------------ wak names -----------

typedef struct MAPFILE
{
    char fn[80];
    struct MAPFILE *next;
} MAPFILE;

MAPFILE maphead, *curmap, *tmpmap;

int num_maps = 0;

void get_map_filenames()
{
    // retrieve map filenames from ./maps/*.wka

    struct ffblk f;
    int i;

    curmap = maphead.next;
    while (curmap)
    {
	tmpmap = curmap->next;
	free(curmap);
	curmap = tmpmap;
    }

    maphead.next = NULL;
    num_maps = 0;
    i = 0;

    curmap = &maphead;
    for (;;)
    {
	if (i==0)
	{
	    i = 1;
	    if (findfirst("maps/*.wak", &f, FA_RDONLY))
		return;
	}
	else
	{
	    if (findnext(&f))
		return;
	}

	tmpmap = malloc(sizeof(MAPFILE));
	strcpy(tmpmap->fn, "MAPS/");
	strcat(tmpmap->fn, f.ff_name);
	strupr(tmpmap->fn); // font only has uppercase
	curmap->next = tmpmap;
	tmpmap->next = NULL;
	curmap = tmpmap;
	num_maps++;
    }
}

void sort_map_filenames()
{
    int finish = 0, order;
    MAPFILE *a, *b, *c, *d;

    do
    {
	finish = 1;
	curmap = maphead.next;
	tmpmap = &maphead;

	while (curmap && curmap->next)
	{
	    order = strcmp(curmap->fn, curmap->next->fn);
	    if (order>0)    // gotta swap
	    {
		finish = 0;

		// tmpmap---->curmap---->curmap.next----->curmap.next.next
		//    1         2             3                 4

		// needs to go:
		//      1       3             2                 4

		// then end with curmap at ^^^^^^ here
		// and tmpmap ^^^^^^ here (one before

		a = tmpmap;
		b = curmap;
		c = curmap->next;
		if (c)
		    d = curmap->next->next;
		else 
		    d = NULL;

		a->next = c;
		c->next = b;
		b->next = d;

		curmap = b;
		tmpmap = c;
	    }
	    else
	    {
		tmpmap = tmpmap->next;
		curmap = curmap->next;
	    }
	}

	//putpixel(screen, x++, 180, YELLOW);
    } while (!finish);
}

void free_map_filenames()
{
    curmap = maphead.next;
    while (curmap)
    {
	tmpmap = curmap->next;
	free(curmap);
	curmap = tmpmap;
    }

    num_maps = 0;
}


//------------------------------------------------------ trade waks ----------

#define MAPLIST_START   'M'
#define MAPLIST_END     'm'

MAPFILE *match(char *fn)
{
    MAPFILE *t;
    t = maphead.next;
    while (t)
    { 
	// disregard case
	if (stricmp(get_filename(t->fn), fn)==0)
	    return t; 
	t =t->next; 
    }

    return NULL;
}

/* send
 * recv and compare map filenames
 * then sort
 */
void trade_map_filenames()
{
    char buf[80];
    int pos, ch;
    int x = 16;

    // send first (no paths)
    curmap = maphead.next;
    while (curmap)
    {
	skSend(MAPLIST_START);
	skSendString(get_filename(curmap->fn));
	skSend(0);
	curmap = curmap->next;
	putpixel(screen, x++, 180, RED);
    }
    skSend(MAPLIST_END);

    // now recv 
    for (;;)
    {
	ch = skRecv();
	if (ch==MAPLIST_START)
	{
	    // get filename
	    pos = 0;
	    do {
		while (!skReady());
		ch = skRecv();
		buf[pos++] = ch;
	    } while (ch);

	    putpixel(screen, x++, 180, LBLUE);

	    tmpmap = match(buf);
	    if (tmpmap)
	    {
		tmpmap->fn[0] = 'Q'; 
		// mark (replace M from maps/*.wak to Q) 
	    }
	}
	else if (ch==MAPLIST_END)
	    break;
    } 

    // delete locals that aren't marked
    // fix up marked 
    curmap = maphead.next;
    tmpmap = &maphead;
    while (curmap)
    {
	if (curmap->fn[0]=='Q') //marked, fix up
	{
	    curmap->fn[0] = 'M';
	    curmap=curmap->next;
	    tmpmap=tmpmap->next;
	}
	else    // not marked, kill
	{
	    tmpmap->next = curmap->next;
	    free(curmap);
	    curmap = tmpmap->next;
	    num_maps--;
	}

	putpixel(screen, x++, 180, GREEN);
    } 

    // shitty bubble sort 
    sort_map_filenames();
}


//------------------------------------------------------ select wak ----------

// allow map selection
// w/ chat box 

#define CHAT_INCOMING   1
#define CHAT_KEYDOWN    2
#define CHAT_KEYUP      3
#define CHAT_NEWMAP     4 
#define CHAT_LEAVE      5 
#define CHAT_RETURN     6 

char return_str[] = "return me to my game NOW!";

char *select_map()
{
    int top = 0, selected = 0, i, k, y;
    int chatting = 0, line = -1;
    char chatbox[6][80] = { "", "", "", "", "", "" };
    char cur[80] = "", temp[80], ch;
    int remote;

    for (;;)
    {
	clear(dbuf);

	// map select

	textout(dbuf, small, "GRAVEYARD", 20, 10, RED);
	textout_centre(dbuf, small, "UP/DOWN: SELECT  F10: START  F4: DISCONNECT", 160, 120, RED);

	curmap = maphead.next;
	for (i=0; i<top; i++)
	    curmap = curmap->next;

	for (i=0; i<9; i++) 
	{
	    if (i+top<num_maps)
	    {
		int c = WHITE;
		if (top+i==selected)
		    c = YELLOW;
		textout(dbuf, small, get_filename(curmap->fn), 20, 25 + i*10, c);
		curmap = curmap->next;
	    }
	}
	textout(dbuf, small, "*", 10, 25 + (selected-top)*10, RED);

	// player list

	textout(dbuf, small, "THE DAMNED", 180, 10, RED);
	y = 20;
	for (i=0; i<num_players; i++)
	{
	    textout(dbuf, small, players[i].name, 190, y + 5, WHITE);
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
	for(;;)
	{
	    if (keypressed())
	    {
		remote = 0;
		k = readkey();
		break;
	    }

	    if ((comm==serial || comm==modem) && skReady())
	    {
		k = 0;
		remote = skRecv();
		break;
	    }
	}

	// down arrow
	if ((k>>8) == KEY_DOWN || remote == CHAT_KEYDOWN)
	{
	    if (++selected>=num_maps)
		selected--;
	    else if (selected>=top+9)
		top++;

	    if ((k>>8)==KEY_DOWN && (comm==serial || comm==modem))
		skSend(CHAT_KEYDOWN);
	}

	// up arrow
	if ((k>>8) == KEY_UP || remote == CHAT_KEYUP)
	{
	    if (--selected < 0)
		selected = 0;
	    else if (selected<top)
		top--;

	    if ((k>>8)==KEY_UP && (comm==serial || comm==modem))
		skSend(CHAT_KEYUP);
	}

	// esc... return to game
	if ((k>>8) == KEY_ESC || remote == CHAT_RETURN)
	{
	    while (key[KEY_ESC]);
	    if ((k>>8) == KEY_ESC && (comm==serial || comm==modem))
		skSend(CHAT_RETURN);
	    return return_str;
	}

	// f4... get out
	if ((k>>8) == KEY_F4 || remote == CHAT_LEAVE)
	{
	    if ((k>>8) == KEY_F4 && (comm==serial || comm==modem))
		skSend(CHAT_LEAVE);
	    return NULL;
	}

	// f10.. start game
	if ((k>>8) == KEY_F10 || remote == CHAT_NEWMAP)
	{
	    if ((k>>8) == KEY_F10 && (comm==serial || comm==modem))
		skSend(CHAT_NEWMAP);

	    curmap = maphead.next;
	    while (selected--)
		curmap = curmap->next;

	    return curmap->fn;
	}

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

		play_sample(dat[WAV_INCOMING].dat, 255, 128, 1000, 0);

		if ((comm==serial || comm==modem))
		{
		    skSend(CHAT_INCOMING);
		    skSendString(temp);
		    skSend(0);
		}
	    }
	}

	// recv chat
	if (remote==CHAT_INCOMING)
	{ 
	    int pos=0;

	    // scroll?
	    if (++line>5) 
	    {
		for (i=0; i<5; i++)
		    strcpy(chatbox[i], chatbox[i+1]);
		line--;
	    }

	    do
	    {
		while (!skReady());
		remote = skRecv();
		temp[pos++]=remote;
	    } while (remote);

	    strcpy(chatbox[line], temp);

	    play_sample(dat[WAV_INCOMING].dat, 255, 128, 1000, 0);
	}

	// if backspace
	if ((k>>8) == KEY_BACKSPACE && chatting)
	{
	    i = strlen(cur);
	    if (i) {
		cur[i-1] = 0;
		play_sample(dat[WAV_TYPE].dat, 100, 128, 1000, 0);
	    }
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
		play_sample(dat[WAV_TYPE].dat, 100, 128, 1000, 0);
	    }
	}
    } 
}


//------------------------------------------------------ i beat you! ---------

void score_sheet()
{
    int i, y;

    show_mouse(NULL);
    clear(screen);
    textout_centre(screen, dat[MINI].dat, "FRAGS", 160, 50, RED);

    y = 70;

    for (i=0; i<MAX_PLAYERS; i++)
    {
	if (players[i].exist)
	{
	    textprintf(screen, dat[MINI].dat, 100, y, WHITE, "%s: %d FRAGS", players[i].name, players[i].frags);
	    y+=16;
	}
    }

    speed_counter = 0;
    while (speed_counter < GAME_SPEED);

    clear_keybuf();
    while (!keypressed() && !mouse_b);
    clear_keybuf();
    while (mouse_b);
}


//------------------------------------------------------ connect via serial --

#define SER_CONNECTPLS  '?'
#define SER_CONNECTOK   '!'
#define SER_THROWDICE   '@'
#define SER_MYNAMEIS    ''

int linkup()
{
    int x, y;

    #define LINKUP_MSG  "LINKING UP (PRESS ESC TO ABORT)"

    rest(500);
    skClear();

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

    textout(screen, dat[MINI].dat, "TOUCHED..", 16, y+8, WHITE);
    return 1;
}

int connect_serial(int comport)
{
    int l = 0, r = 0;

    seed = time(NULL);
    srandom(seed);

    if (!skOpen(comport, BAUD_19200, BITS_8 | PARITY_NONE | STOP_1))
    {
	clear(screen);
	textout_centre(screen, dat[MINI].dat, "ERROR OPENING COM PORT", 160, 90, WHITE);
	while (!keypressed() && !mouse_b);
	clear_keybuf();
	return 0;
    }

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
    {
	skClose();
	return 0;
    }

    skSend(SER_THROWDICE);
    while (skRecv() != SER_THROWDICE) 
    {
	if (key[KEY_ESC])
	{
	    while (key[KEY_ESC]);
	    skClose();
	    return 0;
	}
    }

    do
    {
	l = (random()%255) + 1;     // 1-255
	skSend(l);

	while (!skReady())
	{
	    if (key[KEY_ESC]) 
	    {
		while (key[KEY_ESC]);
		skClose();
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

    srandom(seed);
    next_position = random()%(24*24);
    rnd_index = random()%600;
    irnd_index = random()%600;

    num_players = 2;
    comm = serial;
    return 1;
}

void trade_names()
{
    int pos, player, ch, left;

    // send name
    skSend(SER_MYNAMEIS);
    skSend(local);
    skSendString(players[local].name);
    skSend(0);

    left = num_players-1;

    // get remote name

    do 
    {
	while (skRecv() != SER_MYNAMEIS);
	while (!skReady());
	player = skRecv();

	pos = 0;
	do
	{
	    while (!skReady());
	    ch = skRecv();
	    players[player].name[pos++] = ch; 
	} while (ch);

	left--;
    } while (left);
}

extern int com_port;    // run.c

void serial_func()
{
    char *fn;
    int first_play = 1;

    // name
    if (!get_name())
	return;

    // connect
    if (!connect_serial(com_port))
	return;

    no_germs();
    strcpy(players[local].name, local_name);
    trade_names();

    // get list of maps (from local and remote)
    get_map_filenames();
    trade_map_filenames();

    for (;;)
    {
	if (!num_maps) break;

	show_mouse(NULL);

	// select level
	loop:
	fn = select_map();
	if (!fn) break;
	if (fn==return_str) 
	{
	    if (!first_play)
		goto returntogame;
	    else
		goto loop;
	}

	// load level
	load_map(fn);

	// init players 
	retain_players();
	no_germs(); 
	restore_players();
	spawn_players();

	// final synching
	skSend(SER_CONNECTOK);
	while (skRecv()!=SER_CONNECTOK);

	// go!
	play_sample(dat[WAV_LETSPARTY].dat, 255, 128, 1000, 0);

	returntogame:   // dodgy gotos
	game_loop();

	first_play = 0;
    }

    if (!first_play)
    {
	// disconnected, show who won
	score_sheet();
    }

    free_map_filenames();

    // back to root menu
    enter_menu(root_menu); 
}


//------------------------------------------------------ connect via modem ---

int gy = 10;

void gprintf_reset()
{
    gy = 10;
    clear(screen);
    show_mouse(NULL);
}

void gprintf(char *fmt, ...)
{
    char buf[200];
    va_list va;

    va_start(va, fmt); 
    vsprintf(buf, fmt, va);
    strupr(buf);

    if (gy>=SCREEN_H-10)
    {
	blit(screen, screen, 10, 20, 10, 10, SCREEN_W-20, SCREEN_H-10);
	gy = SCREEN_H-10-10;
    }

    textout(screen, dat[MINI].dat, buf, 10, gy, WHITE);
    gy+=10;
}

void _rest50() { rest(50); }

int _cancel() { if (key[KEY_ESC]) return 1; return 0; }

int connect_modem()
{
    int l = 0, r = 0;

    seed = time(NULL);
    srandom(seed);

    rest(1000);

    gprintf("throwing dice");

    skSend(SER_THROWDICE);
    while (skRecv() != SER_THROWDICE);

    gprintf("trhowing 2");

    do
    {
	l = (random()%255) + 1;     // 1-255
	skSend(l);

	while (!skReady())
	{
	    if (key[KEY_ESC]) 
	    {
		while (key[KEY_ESC]);
		skClose();
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

    srandom(seed);
    next_position = random()%(24*24);
    rnd_index = random()%600;
    irnd_index = random()%600;

    num_players = 2;
    comm = modem;
    return 1;
}

int modem_init(comport)
{
    int ch;

    gprintf_reset();

    if (!skOpen(comport, BAUD_19200, BITS_8 | PARITY_NONE | STOP_1))
    {
	textout_centre(screen, dat[MINI].dat, "ERROR OPENING COM PORT", 160, 90, WHITE);
	while (!keypressed() && !mouse_b);
	clear_keybuf();
	return 0;
    }

    switch (comport)
    {
	case COM1: gprintf("COM1 OPENED"); break;
	case COM2: gprintf("COM2 OPENED"); break;
	case COM3: gprintf("COM3 OPENED"); break;
	case COM4: gprintf("COM4 OPENED"); break;
    }

    if (skEnableFIFO())
	gprintf("16550A UART FIFO ENABLED");
    else
	gprintf("FIFO NOT ENABLED");

    skm_delay = _rest50;

    for (;;)
    {
	if (!skmModemPresent())
	{
	    gprintf("MODEM NOT FOUND");
	    gprintf("PRESS R to retry or esc to cancel");

	    do { 
		ch = readkey() & 0xff;
		if (ch=='r') ch = 'R';
	    } while (ch != 27 && ch != 'R');

	    if (ch==27) return 0;
	} 
	else
	{
	    gprintf("MODEM PRESENT");
	    break;
	}
    }

    skmInit();
    gprintf("modem Initialised");
    skmDisableAutoAnswer();
    gprintf("disable autoanswer"); 
    return 1;
}

int dial()
{
    int err, ch;

    for (;;)
    {
	gprintf("Dialing %s...", phoneno);
	err = skmDial(phoneno, _cancel);

	switch (err)
	{
	    case 0:
		gprintf("Connected!");
		return 1;
	    case ERR_NOCARRIER:
		gprintf("No carrier");
		break;
	    case ERR_ERROR:
		gprintf("Invalid phone number");
		break;
	    case ERR_NODIALTONE:
		gprintf("No dialtone");
		break;
	    case ERR_BUSY:
		gprintf("Busy");
		break;
	    case ERR_NOANSWER:
		gprintf("No answer");
		break;
	    case ERR_USERABORT:
		gprintf("User aborted");
		while (key[KEY_ESC]);
		clear_keybuf();
		break;
	}

	gprintf("press r to retry or esc to cancel");

	do { 
	    ch = readkey() & 0xff;
	    if (ch=='r') ch = 'R';
	} while (ch != 27 && ch != 'R');

	if (ch==27) return 0;
    }
}

int modem_dial(int comport)
{
    if (modem_init(comport)==0) return 0;

    if (!dial())
    {
	skClose();
	return 0;
    }

    connect_modem();
    return 1;
}

int answer()
{
    skmEnableAutoAnswer();
    gprintf("Awaiting incoming call...");

    if (skmAwaitConnect(_cancel)==ERR_USERABORT)
    {
	gprintf("User aborted");
	rest(100);
	skmDisableAutoAnswer();
	return 0;
    }

    gprintf("Connected!");
    return 1;
}

int modem_answer(int comport)
{
    if (modem_init(comport)==0) return 0;

    if (!answer())
    {
	skClose();
	return 0;
    }

    connect_modem();
    return 1;
}

void modem_part_two()
{
    char *fn;
    int first_play = 1;

    no_germs();
    strcpy(players[local].name, local_name);
    trade_names();

    // get list of maps (from local and remote)
    get_map_filenames();
    trade_map_filenames();

    for (;;)
    {
	if (!num_maps) break;

	show_mouse(NULL);

	// select level
	loop:
	fn = select_map();
	if (!fn) break;
	if (fn==return_str) 
	{
	    if (!first_play)
		goto returntogame;
	    else
		goto loop;
	}

	// load level
	load_map(fn);

	// init players 
	retain_players();
	no_germs(); 
	restore_players();
	spawn_players();

	// final synching
	skSend(SER_CONNECTOK);
	while (skRecv()!=SER_CONNECTOK);

	// go!
	play_sample(dat[WAV_LETSPARTY].dat, 255, 128, 1000, 0);

	returntogame:   // dodgy gotos
	game_loop();

	first_play = 0;
    }

    if (!first_play)
    {
	// disconnected, show who won
	score_sheet();
    }

    skmHangup();

    free_map_filenames();

    // back to root menu
    enter_menu(root_menu); 
}

void dial_func()
{
    // name
    if (!get_name()) return;

    // phone no.
    if (!get_phoneno()) return;

    if (!modem_dial(com_port)) return;

    modem_part_two();
}

void answer_func()
{
    // name
    if (!get_name()) return;

    if (!modem_answer(com_port)) return;

    modem_part_two();
}


//------------------------------------------------------ lonely --------------

void single_func()
{
    char *fn;
    int first_play = 1;

    // name
    if (!get_name())
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

    get_map_filenames();
    sort_map_filenames();

    for (;;)
    {
	show_mouse(NULL);

	// select level
	loop:
	fn = select_map();
	if (!fn) break;
	if (fn==return_str) 
	{
	    if (!first_play)
		goto returntogame;
	    else
		goto loop;
	}

	// load level
	load_map(fn);

	// init players 
	retain_players();
	no_germs();
	restore_players();
	spawn_players();

	// go!
	play_sample(dat[WAV_LETSPARTY].dat, 255, 128, 1000, 0);

	returntogame:
	game_loop();

	first_play = 0;
    }

    if (!first_play)
    {
	// disconnected, show who won
	score_sheet();
    }

    free_map_filenames();

    // back to root menu
    enter_menu(root_menu); 
}


//------------------------------------------------------ not yet, okay? ------

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


//------------------------------------------------------ menus ---------------

BLUBBER modem_menu[] = 
{
    { func, "Dial", dial_func },
    { func, "Answer", answer_func },
    { prev, "", startgame_menu }
};

BLUBBER startgame_menu[] = 
{
    { func, "Solo", single_func },
    { func, "Serial", serial_func },
    { join, "Modem-Modem", modem_menu },
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


//------------------------------------------------------ manager -------------

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
		    while (key[KEY_ESC] || mouse_b);
		    break;
		}
	    }
	}
    }
}


//------------------------------------------------------ start me up----------

void menu()
{
    big = dat[UNREAL].dat;
    small = dat[MINI].dat;
    text_mode(-1);

    maphead.next = NULL;
    num_maps = 0;

    blubber(root_menu);
}