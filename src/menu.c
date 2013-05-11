/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Menu system.
 */


#include <string.h>
#include "a4aux.h"
#include "blood.h"
#include "globals.h"
#include "menu.h"
#include "stats.h"
#include "vidmode.h"


#define big	dat[UNREAL].dat


/*----------------------------------------------------------------------
 *
 * 	Menu system globals
 * 
 *----------------------------------------------------------------------*/

static BLUBBER *cur;
static int selected;
static int count, top;

int menu_end;



/*----------------------------------------------------------------------
 *
 * 	Procs
 * 
 *----------------------------------------------------------------------*/

/* Dummy function.  */
void prev_menu(BLUBBER *bp, int command, int ex)
{
    (void)bp;
    (void)command;
    (void)ex;
}


/* Switch to another node of menu.  */
void enter_menu(BLUBBER *bp)
{
    int i = 0;

    cur = bp;

    /* count items */
    count = 0;
    do {
	bp = &cur[i++];
	count++;
    } while (bp->proc != prev_menu);

    /* offset top accordingly */
    top = 110 - count/2 * 32;

    selected = 0;
}


/* Connect between menus.  */
void join_menu(BLUBBER *bp, int command, int ex)
{
    switch (command) {
	case MSG_DRAW:
	    /* `ex' will be y position */
	    textout_centre_ex(dbuf, big, bp->string, 160, ex, -1, -1);
	    break;

	case MSG_CLICK:
	    enter_menu(bp->thing);
	    break;
    }
}


/* Run a function if this item selected.  */
void menu_proc(BLUBBER *bp, int command, int ex)
{
    switch (command) {
	case MSG_DRAW:
	    /* `ex' will be y position */
	    textout_centre_ex(dbuf, big, bp->string, 160, ex, -1, -1);
	    break;

	case MSG_CLICK:
	    ((void (*)())bp->thing)();
	    break;
    }
}
    


/*----------------------------------------------------------------------
 *
 * 	Menu manager
 * 
 *----------------------------------------------------------------------*/


static int dirty;


static void switch_in_callback(void)
{
    dirty = 1;
}


static inline int touch(int item)
{
    int my = mouse_y * 200.0/SCREEN_H;
    return ((my >= (top + (item * 32) + (text_height(big) / 2) - 16))
	 && (my <= (top + (item * 32) + (text_height(big) / 2) + 16)));
}


static void draw_menu_spotlight(BITMAP *dbuf, BITMAP *spot, int y)
{
    const int cx = dbuf->w/2;
    const int half = spot->w/2;

    al_set_target_bitmap(dbuf->real);
    al_set_blender(ALLEGRO_ADD, ALLEGRO_DST_COLOR, ALLEGRO_ZERO);
    al_draw_bitmap(spot->real, cx - half, y, 0);
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

    /* Black out around the spotlight. */
    rectfill(dbuf, 0, 0, cx - half - 1, dbuf->h, 0);
    rectfill(dbuf, cx + half, 0, dbuf->w, dbuf->h, 0);
    rectfill(dbuf, cx - half, 0, cx + half, y - 1, 0);
    rectfill(dbuf, cx - half, y + spot->h, cx + half, dbuf->h, 0);
}


static void do_blubber(BLUBBER *start)
{
    BLUBBER *bp;
    int do_action = 0, do_prev = 0;
    int old_mouse_y = 0;
    int i;

    menu_end = 0;
    dirty = 1;
    enter_menu(start);
    show_mouse(screen);

    while (!menu_end) {

	/* Handle mouse.  */
	if (old_mouse_y != mouse_y) {
	    int saved = selected;
	    
	    for (i = 0; cur[i].proc != prev_menu; i++) 
		if (touch(i)) {
		    selected = i;
		    break;
		}
	    
	    old_mouse_y = mouse_y;
	    if (selected != saved)
		dirty = 1;
	}

	if (mouse_b & 1)
	    do_action = 1;
	else if (mouse_b & 2) {
	    do_prev = 1;
	    while (mouse_b) ;
	}

	/* Handle keypresses.  */
	if (keypressed()) {
	    int k = readkey() >> 8;
	    
	    if (k == KEY_UP) {
		if (selected > 0) {
		    selected--;
		    dirty = 1;
		}
	    }
	    else if (k == KEY_DOWN) {
		if (cur[selected+1].proc != prev_menu) {
		    selected++;
		    dirty = 1;
		}
	    }
	    else if (k == KEY_HOME) {
		if (selected != 0) {
		    selected = 0;
		    dirty = 1;
		}
	    }
	    else if (k == KEY_END) {
		int x = selected;
		while (cur[selected+1].proc != prev_menu)
		    selected++;
		if (selected != x)
		    dirty = 1;
	    }
	    else if (k == KEY_ENTER)
		do_action = 1;
	    else if (k == KEY_ESC)
		do_prev = 1;
	    else if (k == KEY_F11) {
		toggle_fullscreen_window();
		dirty = 1;
	    }
	}
		
	/* Redraw dirty menu.  */
	if (dirty) {
	    int y;
	    
	    color_map = &light_map;
	    blit(dat[TITLE].dat, dbuf, 0, 0, 0, 0, 320, 200);
	    
	    /* Draw menu items.  */
	    for (i = 0, y = top; cur[i].proc != prev_menu; i++, y += 32) 
		cur[i].proc(cur + i, MSG_DRAW, y);
	    
	    /* Add spotlight. */
	    y = (top + selected * 32) + (text_height(big) / 2 - 192 / 2);
	    draw_menu_spotlight(dbuf, dat[L_SPOT].dat, y);
	    
	    /* Little message space.  */
	    textout_right_ex(dbuf, dat[MINI].dat, get_filename(current_stats),
			     320-2, 200 - text_height(dat[MINI].dat), 8, -1);
	    
	    /* Blit to screen.  */
	    blit_to_screen(dbuf);
	 
	    dirty = 0;
	}

	/* Perform action.  */
	if (do_action) {
	    int s = selected;
	    
	    while (mouse_b); clear_keybuf();
	    cur[s].proc(&cur[s], MSG_CLICK, 0);
	    while (mouse_b); clear_keybuf();
	    show_mouse(screen);
		
	    dirty = 1;
	    do_action = 0;
	}
	
	/* Previous menu.  */
	if (do_prev) {
	    i = 0;
	    do {
		bp = &cur[i++]; 
	    } while (bp->proc != prev_menu);

	    if (bp->thing) {
		enter_menu(bp->thing);
		show_mouse(screen);
		selected = old_mouse_y = 0;
	    }

	    do_prev = 0;
	    dirty = 1;
	}
    }
}


void blubber(BLUBBER *start)
{
    int mode = get_display_switch_mode();
    int need = (mode == SWITCH_AMNESIA) || (mode == SWITCH_BACKAMNESIA);
    
    if (need)
	set_display_switch_callback(SWITCH_IN, switch_in_callback);

    do_blubber(start);
    
    if (need)
	remove_display_switch_callback(switch_in_callback);
}
