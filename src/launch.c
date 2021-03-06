/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Game launching.
 */


#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "a4aux.h"
#include "menu.h"
#include "launch.h"
#include "credits.h"
#include "blood.h"
#include "colours.h"
#include "cpu.h"
#include "demo.h"
#include "demintro.h"
#include "engine.h"
#include "gameloop.h"
#include "globals.h"
#include "main.h"
#include "map.h"
#include "message.h"
#include "options.h"
#include "player.h"
#include "resource.h"
#include "rpstring.h"
#include "rnd.h"
#include "setweaps.h"
#include "sk.h"
#include "skhelp.h"
#include "stats.h"
#include "statlist.h"
#include "vidmode.h"
#include "winhelp.h"


#define big	dat[UNREAL].dat
#define small	dat[MINI].dat


static BLUBBER root_menu[];
static BLUBBER startgame_menu[];


static char local_name[16 + 1];
static char target_addr[32 + 1];



/*----------------------------------------------------------------------
 *
 * 	Options, credits and quit 
 * 
 *----------------------------------------------------------------------*/

static void options_proc(void)
{
    options();
}

static void credits_proc(void)
{
    credits();
}

static void quit_proc(void)
{
    fade_out(32);
    menu_end = 1;
}



/*----------------------------------------------------------------------
 *
 * 	Error message
 * 
 *----------------------------------------------------------------------*/

static void error_screen(char *err) 
{
    clear_bitmap(dbuf);
    textout_centre_ex(dbuf, dat[MINI].dat, err, 160, 90, WHITE, -1);
    blit_to_screen(dbuf);

    while (!keypressed() && !mouse_b)
	rest(0);
    clear_keybuf();
}



/*----------------------------------------------------------------------
 *
 * 	Prompting
 * 
 *----------------------------------------------------------------------*/

static int prompt(char *string, char *dest, int maxlen)
{
    char temp[maxlen+1];
    int k, len, c;
    
    clear_keybuf();
    
    while (1) {
	/* screen output */
	strcpy(temp, dest);
	strcat(temp, "_");
	clear_bitmap(dbuf);
	textout_centre_ex(dbuf, small, string, 160, 70, WHITE, -1);
	textout_centre_ex(dbuf, small, temp,   160, 90, WHITE, -1);
	blit_to_screen(dbuf);

	/* handle keypress */	
	k = readkey();

	if ((k >> 8) == KEY_ENTER) {
	    while (key[KEY_ENTER])
		rest(0);
	    return 1;
	}

	if ((k >> 8) == KEY_ESC) {
	    while (key[KEY_ESC])
		rest(0);
	    return 0;
	}

	if ((k >> 8) == KEY_F11) {
	    toggle_fullscreen_window();
	}

	/* entering name */
	len = strlen(dest);

	if ((k >> 8) == KEY_BACKSPACE && (len > 0)) {
	    dest[len - 1] = 0;
	    if (!mute_sfx)
		play_sample(dat[WAV_TYPE].dat, 100, 128, 1000, 0);
	}
	else if (len < maxlen-1) {
	    c = toupper(k & 0xff);
	    if (c >= ' ' && c <= '~') {
		dest[len] = c;
		dest[len+1] = 0;
		if (!mute_sfx)
		    play_sample(dat[WAV_TYPE].dat, 100, 128, 1000, 0);
	    }	    
	}
    }
}


static int get_name(void)
{
    int x;
    
    do {
	x = prompt("ENTER YOUR HANDLE:", local_name, sizeof local_name - 1);
    } while (x && !local_name[0]);
    return x;
}



/*----------------------------------------------------------------------
 *
 * 	Map filenames
 * 
 *----------------------------------------------------------------------*/

typedef struct MAPFILE {
    char marked;
    char fn[1024];
    struct MAPFILE *next;
} MAPFILE;

static MAPFILE maphead, *curmap, *tmpmap;

static int num_maps;


static void get_map_filenames(void)
{
    struct al_ffblk f;
    int i;
    char path[MAX_PATH_LENGTH];

    curmap = maphead.next;
    while (curmap) {
	tmpmap = curmap->next;
	free(curmap);
	curmap = tmpmap;
    }

    maphead.next = NULL;
    num_maps = 0;
    i = 0;

    strcpy(path, get_resource(R_SHARE, "maps/*.wak"));
    
    curmap = &maphead;
    while (1) {
	if (i == 0) {
	    i = 1;
	    if (al_findfirst(path, &f, FA_RDONLY | FA_ARCH | FA_SYSTEM))
		return;
	}
	else {
	    if (al_findnext(&f)) {
		al_findclose(&f);
		return;
	    }
	}

	tmpmap = malloc(sizeof(MAPFILE));
	strcpy(tmpmap->fn, f.name);
	rp_strupr(tmpmap->fn);	/* font USED to only have uppercase */
	curmap->next = tmpmap;
	tmpmap->next = NULL;
	curmap = tmpmap;
	num_maps++;
    }
}

static void sort_map_filenames(void)
{
    int finish = 0, order;
    MAPFILE *a, *b, *c, *d;

    do {
	finish = 1;
	curmap = maphead.next;
	tmpmap = &maphead;

	while (curmap && curmap->next) {
	    order = strcmp(curmap->fn, curmap->next->fn);

	    if (order > 0) {
		finish = 0;

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
	    else {
		tmpmap = tmpmap->next;
		curmap = curmap->next;
	    }
	}
    } while (!finish);
}

static void free_map_filenames(void)
{
    curmap = maphead.next;
    while (curmap) {
	tmpmap = curmap->next;
	free(curmap);
	curmap = tmpmap;
    }
    
    maphead.next = NULL;
    num_maps = 0;
}



/*----------------------------------------------------------------------
 *
 * 	Map filename trading
 * 
 *----------------------------------------------------------------------*/

#define MAPLIST_START   'M'
#define MAPLIST_END     'm'

static MAPFILE *match(char *fn)
{
    MAPFILE *t = maphead.next;
    
    while (t) {
	if (rp_stricmp(get_filename(t->fn), fn) == 0)
	    return t;
	
	t = t->next; 
    }

    return NULL;
}

static void trade_map_filenames(void)
{
    char buf[1024];
    int x = 16, ch;
    int done_recv;
    
    /* Clear `marked' flag.  */
    curmap = maphead.next;
    while (curmap)
	curmap->marked = 0, curmap = curmap->next;
        
    /* Send and receive map filenames (no paths).  */
    curmap = maphead.next;
    done_recv = 0;
    
    while (!done_recv || curmap) {
	/* send */
	if (curmap) {
	    sprintf(buf, "%c%s", MAPLIST_START, get_filename(curmap->fn));
	    skSendString((unsigned char *) buf);
	    
	    curmap = curmap->next;
	    putpixel(screen, x++, SCREEN_H - 20, RED);
	    
	    if (!curmap)
		skSend(MAPLIST_END);
	    
	    rest(5);
	}
	
	/* receive */
	if (!done_recv) {
	    ch = skRecv();
	    if (ch == MAPLIST_START) {
		receive_string(buf);
		putpixel(screen, x++, SCREEN_H - 20, LBLUE);
		
		tmpmap = match(buf);
		if (tmpmap) 
		    tmpmap->marked = 1;

		rest(5);
	    }
	    else if (ch == MAPLIST_END)
		done_recv = 1;
	}
	
	/* Hack to keep libnet polling.  */
	skReady();
    }

    /* Delete locals that aren't marked.  */
    curmap = maphead.next;
    tmpmap = &maphead;
    while (curmap) {
	if (curmap->marked) {
	    curmap = curmap->next;
	    tmpmap = tmpmap->next;
	}
	else {
	    /* Not marked, kill.  */
	    tmpmap->next = curmap->next;
	    free(curmap);
	    curmap = tmpmap->next;
	    num_maps--;
	}

	putpixel(screen, x++, SCREEN_H - 20, GREEN);
    } 
}



/*----------------------------------------------------------------------
 *
 * 	Map selection with chat box
 * 
 *----------------------------------------------------------------------*/

#define CHAT_INCOMING   1
#define CHAT_KEYDOWN    2
#define CHAT_KEYUP      3
#define CHAT_KEYHOME	4
#define CHAT_KEYEND	5
#define CHAT_KEYPGUP	6
#define CHAT_KEYPGDN	7
#define CHAT_NEWMAP     8 
#define CHAT_LEAVE      9 
#define CHAT_RETURN     10 

static char return_str[] = "return me to my game NOW!";

static char record_reminder[128];

static char *select_map(int *top, int *selected, char *current_map)
{
    int i, k, y;
    int chatting = 0, line = -1;
    char chatbox[6][80] = { "", "", "", "", "", "" };
    char cur[80] = "", temp[80], ch;
    int remote;
    
    while (keypressed())
	readkey();

    while (1) {
	clear_bitmap(dbuf);
	
	/* recording demos reminder */
	textout_right_ex(dbuf, small, record_reminder, 320-10, 2, DARKGREY, -1);
	
	/* map select */
	textout_ex(dbuf, small, "GRAVEYARD", 20, 10, RED, -1);
	textout_centre_ex(dbuf, small,
		          "UP/DOWN: SELECT  F10: START  F4: DISCONNECT",
		          160, 120, RED, -1);

	curmap = maphead.next;
	for (i = 0; i < (*top); i++)
	    curmap = curmap->next;

	for (i = 0; i < 9; i++) {
	    if (i + (*top) < num_maps) {
		int c = WHITE;
		if ((*top)+i == (*selected))
		    c = YELLOW;
		else if (current_map == curmap->fn)
		    c = GRAY;
		textout_ex(dbuf, small, get_filename(curmap->fn), 20, 25 + i*10, c, -1);
		curmap = curmap->next;
	    }
	}
	
	textout_ex(dbuf, small, "*", 10, 25 + ((*selected) - (*top)) * 10, RED, -1);

	/* player list */
	textout_ex(dbuf, small, "THE DAMNED", 180, 10, RED, -1);
	for (i = 0, y = 20; i < num_players; i++, y += 20) 
	    textout_ex(dbuf, small, players[i].name, 190, y + 5, WHITE, -1);

	/* chat box */
	hline(dbuf, 0, 130, 319, RED);
	hline(dbuf, 0, 140, 319, RED);
	hline(dbuf, 0, 190, 319, RED);
	textout_ex(dbuf, small, "CHAT BOX", 10, 131, WHITE, -1);

	for (i = 0; i < 6; i++)
	    textout_ex(dbuf, small, chatbox[i], 0, 141 + i * 8, WHITE, -1);

	if (chatting) {
	    textout_ex(dbuf, small, cur, 0, 192, WHITE, -1);
	    textout_ex(dbuf, small, "_", text_length(small, cur), 192, WHITE, -1);
	}

	/* blit onto screen  */
	blit_to_screen(dbuf);

	/* Wait for keypress.  */
	while (1) {
	    if (keypressed()) {
		remote = 0;
		k = readkey();
		break;
	    }

	    if ((comm == peerpeer) && skReady()) {
		k = 0;
		remote = skRecv();
		break;
	    }

	    rest(0);
	}

	/* down key */
	if ((k >> 8) == KEY_DOWN || (remote == CHAT_KEYDOWN)) {
	    if (++(*selected) >= num_maps)
		(*selected)--;
	    else if ((*selected) >= (*top) + 9)
		(*top)++;

	    if ((k >> 8) == KEY_DOWN && (comm == peerpeer))
		skSend(CHAT_KEYDOWN);
	}

	/* up key */
	if ((k >> 8) == KEY_UP || (remote == CHAT_KEYUP)) {
	    if (--(*selected) < 0)
		(*selected) = 0;
	    else if ((*selected) < (*top))
		(*top)--;

	    if ((k >> 8) == KEY_UP && (comm == peerpeer)) 
		skSend(CHAT_KEYUP);
	}
	
	/* home key */
	if ((k >> 8) == KEY_HOME || (remote == CHAT_KEYHOME)) {
	    (*selected) = 0;
	    (*top) = 0;

	    if ((k >> 8) == KEY_HOME && (comm == peerpeer)) 
		skSend(CHAT_KEYHOME);
	}

	/* end key */
	if ((k >> 8) == KEY_END || (remote == CHAT_KEYEND)) {
	    (*selected) = num_maps-1;
	    (*top) = (*selected)-8;
	    if ((*top) < 0)
		(*top) = 0;

	    if ((k >> 8) == KEY_END && (comm == peerpeer)) 
		skSend(CHAT_KEYEND);
	}

	/* pgup key */
	if ((k >> 8) == KEY_PGUP || (remote == CHAT_KEYPGUP)) {
	    (*selected) -= 8;
	    if ((*selected) < 0)
		(*selected) = 0;
	    *top = *selected;

	    if ((k >> 8) == KEY_PGUP && (comm == peerpeer)) 
		skSend(CHAT_KEYPGUP);
	}

	/* pgdn key */
	if ((k >> 8) == KEY_PGDN || (remote == CHAT_KEYPGDN)) {
	    (*selected) += 8;
	    if ((*selected) >= num_maps)
		(*selected) = num_maps-1;
	    (*top) = (*selected)-8;
	    if ((*top) < 0)
		(*top) = 0;

	    if ((k >> 8) == KEY_PGUP && (comm == peerpeer)) 
		skSend(CHAT_KEYPGUP);
	}

	
	/* escape key */
	if ((k >> 8) == KEY_ESC || (remote == CHAT_RETURN)) {
	    while (key[KEY_ESC])
		rest(0);
	    if ((k >> 8) == KEY_ESC && (comm == peerpeer))
		skSend(CHAT_RETURN);
	    return return_str;
	}

	/* F4 key */
	if ((k >> 8) == KEY_F4 || (remote == CHAT_LEAVE)) {
	    if ((k >> 8) == KEY_F4 && (comm == peerpeer))
		skSend(CHAT_LEAVE);
	    return NULL;
	}

	/* F10 key */
	if ((k >> 8) == KEY_F10 || (remote == CHAT_NEWMAP)) {
	    if ((k >> 8) == KEY_F10 && (comm == peerpeer))
		skSend(CHAT_NEWMAP);

	    {
		int x = *selected;
		curmap = maphead.next;
		while (x--) curmap = curmap->next;
	    }

	    return curmap->fn;
	}

	/* F11 key */
	if ((k >> 8) == KEY_F11) {
	    toggle_fullscreen_window();
	}

	/* Enter: send chat */
	if ((k >> 8) == KEY_ENTER) {
	    if (chatting && cur[0]) {

		/* scroll */
		if (++line > 5) {
		    for (i = 0; i < 5; i++)
			strcpy(chatbox[i], chatbox[i+1]);
		    line--;
		}

		strcpy(temp, players[local].name);
		strcat(temp, ":");
		strcat(temp, cur);
		strcpy(chatbox[line], temp);
		cur[0] = 0;

		chatting = 0;

		if (!mute_sfx)
		    play_sample(dat[WAV_INCOMING].dat, 255, 128, 1000, 0);

		if (comm == peerpeer) {
		    skSend(CHAT_INCOMING);
		    skSendString((unsigned char *) temp);
		}
	    }
	}

	/* recv chat */
	if (remote == CHAT_INCOMING) {
	    int pos = 0;

	    /* scroll */
	    if (++line > 5) {
		for (i = 0; i < 5; i++)
		    strcpy(chatbox[i], chatbox[i+1]);
		line--;
	    }

	    do {
		while (!skReady())
		    ;
		remote = skRecv();
		temp[pos++] = remote;
	    } while (remote);

	    strcpy(chatbox[line], temp);

	    if (!mute_sfx)
		play_sample(dat[WAV_INCOMING].dat, 255, 128, 1000, 0);
	}

	/* backspace  */
	if ((k >> 8) == KEY_BACKSPACE && chatting) {
	    i = strlen(cur);
	    if (i) {
		cur[i-1] = 0;
		if (!mute_sfx)
		    play_sample(dat[WAV_TYPE].dat, 100, 128, 1000, 0);
	    }
	}

	/* else, type into chat box */
	ch = toupper(k & 0xff);
	if (ch >= ' ' && ch <= '~') {
	    i = strlen(cur);
	    if (i < 47) {
		cur[i] = ch;
		cur[i+1] = 0;
		chatting = 1;
		if (!mute_sfx)
		    play_sample(dat[WAV_TYPE].dat, 100, 128, 1000, 0);
	    }
	}
    }
}



/*----------------------------------------------------------------------
 *
 * 	Frags table
 * 
 *----------------------------------------------------------------------*/

static void score_sheet(void)
{
    int i, y;

    blit(dat[FRAGDROP].dat, dbuf, 0, 0, 0, 0, 320, 200);
    textout_centre_ex(dbuf, dat[MINI].dat, "FRAGS", 160, 50, RED, -1);

    y = 70;

    for (i = 0; i < num_players; i++) {
	textprintf_ex(dbuf, dat[MINI].dat, 100, y, WHITE, -1,
		      "%s: %d frags", players[i].name, players[i].frags);
	y += 16;
    }

    blit_to_screen(dbuf);

    rest(500);
    clear_keybuf();
    while (!keypressed() && !mouse_b)
	rest(0);
    clear_keybuf();
    while (mouse_b)
	rest(0);
}



/*----------------------------------------------------------------------
 *
 * 	Game session
 * 
 *----------------------------------------------------------------------*/

static void try_demo_write_open(void)
{
    char *names[2] = { players[0].name, players[1].name };
    char *fn;
    int i;
    
    for (i = 0; i < 2; i++) {
	if (i == 0) 
	    fn = new_demo_filename(".");
	else {
	    char *home = getenv("HOME");
	    fn = (home) ? new_demo_filename(home) : 0;
	}
	
	if (fn && (demo_write_open(fn, num_players, names) == 0)) {
	    rp_strupr((sprintf(record_reminder, "RECORDING DEMO AS %s",
			    get_filename(fn)), record_reminder));
	    return;
	}
    }

    record_reminder[0] = 0;
}


static void do_session(void)
{
    char *fn;
    char *last_fn = NULL;
    int played = 0;
    int top = 0, selected = 0;
    
    record_reminder[0] = '\0';
    if (record_demos) 
	try_demo_write_open();
   
    demo_write_set_rng_seed(seed);
    
    reset_player_frags();

    while (1) {
	/* Pick a level.  */
      loop:
	fn = select_map(&top, &selected, last_fn);
	if (!fn) break;
	
	if (fn == return_str) {
	    if (played)
		goto returntogame;
	    else
		goto loop;
	}

	/* Load level.  */
	if (load_map_wrapper(fn) < 0)
	    break;
	last_fn = fn;
	demo_write_change_map(fn);

	/* Init players.  */
	retain_players();
	reset_engine(); 
	restore_players();
	spawn_players();
	
	/* Go!  */
	if (!mute_sfx)
	    play_sample(dat[WAV_LETSPARTY].dat, 255, 128, 1000, 0);

	if ((comm == cpu) && (!played))	/* hack */
	    add_msg("Use F2 and F3 to adjust CPU level", local);
	
      returntogame:   
	game_loop();

	played = 1;
    }

    if (played) 
	score_sheet();

    demo_write_close();
}



/*----------------------------------------------------------------------
 *
 * 	Peer-Peer stuff
 * 
 *----------------------------------------------------------------------*/

#define PEERPEER_THROWDICE   '@'
#define PEERPEER_MYNAMEIS    1	/* happy face :) */

static int peerpeer_negotiate_environment(void)
{
    int l, r;

    seed = rand();

    /* Throw dice.  */
    skSend(PEERPEER_THROWDICE);
    while (skRecv() != PEERPEER_THROWDICE) {
	if (key[KEY_ESC]) {
	    while (key[KEY_ESC])
		rest(0);
	    skClose();
	    return 0;
	}
    }

    do {
	l = (rand() % 255) + 1; 
	skSend(l);

	while (!skReady()) {
	    if (key[KEY_ESC]) {
		while (key[KEY_ESC])
		    rest(0);
		skClose();
		return 0; 
	    }
	}

	r = skRecv();

    } while (l == r);

    if (l > r) {	/* l > r: we win */
	local = 0;
	send_long(seed);
    }
    else {		/* l < r: we lose */
	local = 1;
	seed = recv_long();
    }

    srnd(seed);
    sirnd(seed);
    next_position = irnd() % (24*24);

    num_players = 2;
    comm = peerpeer;
    return 1;
}

static int peerpeer_check_stats(void)
{
    uint32_t sum = make_stat_checksum(stat_block);
    send_long(sum);
    return sum == (uint32_t)recv_long();
}

static void peerpeer_trade_names(void)
{
    int pos, player, ch, left;

    skSend(PEERPEER_MYNAMEIS);
    skSend(local);
    skSendString((unsigned char *) local_name);

    left = num_players - 1;

    do {
	while (skRecv() != PEERPEER_MYNAMEIS)
	    rest(0);
	while (!skReady())
	    rest(0);
	player = skRecv();

	pos = 0;
	do {
	    while (!skReady())
		rest(0);
	    ch = skRecv();
	    players[player].name[pos++] = ch; 
	} while (ch);

	left--;
    } while (left);
}

static void peerpeer_session(void)
{
    strcpy(players[local].name, local_name);
    peerpeer_trade_names();

    get_map_filenames();
    trade_map_filenames();
    if (!num_maps) 
	return;
    sort_map_filenames();
    
    do_session();

    free_map_filenames();
}



/*----------------------------------------------------------------------
 *
 * 	Solo
 * 
 *----------------------------------------------------------------------*/

static void single_proc(void)
{
    if (!get_name())
	return;

    /* set up environment */
    comm = single;
    local = 0;
    num_players = 1;

    seed = rand();
    srnd(seed);
    sirnd(seed);
    next_position = irnd() % (24*24);

    strcpy(players[local].name, local_name);
    
    get_map_filenames();
    sort_map_filenames();
    
    do_session();

    free_map_filenames();

    enter_menu(root_menu); 
}



/*----------------------------------------------------------------------
 *
 * 	CPU
 * 
 *----------------------------------------------------------------------*/

static void cpu_proc(void)
{
    if (!get_name())
	return;

    /* set up environment */
    comm = cpu;
    local = 0;
    cpu_num = 1;
    num_players = 2;

    seed = rand();
    srnd(seed);
    sirnd(seed);
    next_position = irnd() % (24*24);

    strcpy(players[local].name, local_name);
    strcpy(players[cpu_num].name, "CPU");
    
    get_map_filenames();
    sort_map_filenames();
    
    do_session();

    free_map_filenames();

    enter_menu(root_menu); 
}



/*----------------------------------------------------------------------
 *
 * 	Libnet
 * 
 *----------------------------------------------------------------------*/
	
#ifndef NO_LIBNET_CODE

static char *connect_msg;
static int no_error;

static int libnet_connect_callback(void)
{
    if (connect_msg) {
	clear_bitmap(dbuf);
	textout_centre_ex(dbuf, dat[MINI].dat, connect_msg, 160, 90, WHITE, -1);
	blit_to_screen(dbuf);

	/* only need to do this once */
	connect_msg = 0;
    }

    if (keypressed() && (readkey() >> 8) == KEY_ESC) {
	no_error = 1;
	return -1;
    }

    sleep(0);
    
    return 0;
}

static void libnet_proc(int x, char *addr)
{
    skSetDriver(SK_LIBNET);

    no_error = 0;
    _sk_libnet_open_callback = libnet_connect_callback;
    
    if (!skOpen(x, addr)) {
	if (!no_error)
	    error_screen("ERROR OPENING CONNECTION");
	return;
    }
    
    if (!peerpeer_negotiate_environment())
	return;
    
    if (!peerpeer_check_stats()) {
	error_screen("INCOMPATIBLE STATS");
	return;
    }

    peerpeer_session();
    
    enter_menu(root_menu);
} 

static void libnet_listen_proc(void)
{
    if (!get_name()) 
	return;

    connect_msg = "AWAITING CONNECTION";
    libnet_proc(1, 0);
}

static void libnet_connect_proc(void)
{
    if (!get_name()) 
	return;

    if (prompt("ENTER TARGET ADDRESS:", target_addr, sizeof target_addr - 1)) {
	connect_msg = "INITIATING CONNECTION";
	libnet_proc(0, target_addr);
    }
}

#endif



/*----------------------------------------------------------------------
 *
 * 	Demo playback
 * 
 *----------------------------------------------------------------------*/

static void demo_playback_proc(void)
{
    char filename[MAX_PATH_LENGTH];
    int x;

    strncpy(filename, get_resource(R_SHARE, "demos/"), sizeof filename);
    
    x = file_select_ex("Load demo", filename, "rec", sizeof filename, 0, 0);
    if (!x) return;
    
    if (demo_read_open(filename) < 0) {
	char buf[1024];
	sprintf(buf, "Error opening %s", filename);
	error_screen(buf);
	return;
    }
    
    push_stat_block();
    
    comm = demo;
    local = 0;

    reset_engine();

    demo_read_header();		/* sets num_players and names */

    seed = demo_read_seed();
    srnd(seed);
    sirnd(seed);
    next_position = irnd() % (24 * 24);
    
    introduce_demo();
    game_loop();
    
    /* fade out, for end of movie feel */
    fade_out(4);
        
    score_sheet();

    demo_read_close();
    
    pop_stat_block();
    set_weapon_stats();
    
    enter_menu(root_menu); 
}



/*----------------------------------------------------------------------
 *
 * 	Game menus
 * 
 *----------------------------------------------------------------------*/

#ifndef NO_LIBNET_CODE

static BLUBBER libnet_menu[] =
{
    { menu_proc, "Connect", 	libnet_connect_proc, 0, 0 },
    { menu_proc, "Listen",	libnet_listen_proc, 0, 0 },
    { prev_menu, "", 		startgame_menu, 0, 0 }
};

#endif

static BLUBBER startgame_menu[] = 
{
    { menu_proc, "Solo", 	single_proc, 0, 0 },
#ifndef NO_LIBNET_CODE
    { join_menu, "Sockets",	libnet_menu, 0, 0 },
#endif
    { menu_proc, "CPU", 	cpu_proc, 0, 0 },
    { menu_proc, "Play Demo", 	demo_playback_proc, 0, 0 },
    { prev_menu, "", 		root_menu, 0, 0 }
};

static BLUBBER root_menu[] =
{
    { join_menu, "Start", 	startgame_menu, 0, 0 },
    { menu_proc, "Options", 	options_proc, 0, 0 },
    { menu_proc, "Credits", 	credits_proc, 0, 0 },
    { menu_proc, "Quit", 	quit_proc, 0, 0 },
    { prev_menu, "", 		NULL, 0, 0 }
};



/*----------------------------------------------------------------------
 *
 * 	Menu entry point
 * 
 *----------------------------------------------------------------------*/

void main_menu(void)
{
    big = dat[UNREAL].dat;
    small = dat[MINI].dat;

    maphead.next = NULL;
    num_maps = 0;

    blubber(root_menu);
}
