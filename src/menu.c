/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Menu system.
 */


#include <string.h>
#include <allegro.h>
#include "blood.h"
#include "globals.h"
#include "menu.h"
#include "scrblit.h"


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
void prev_menu(BLUBBER *bp, int command, int ex) { }


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
	    textout_centre(dbuf, big, bp->string, 160, ex, -1);
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
	    textout_centre(dbuf, big, bp->string, 160, ex, -1);
	    break;

	case MSG_CLICK:
	    ((void (*)())bp->thing)();
	    break;
    }
}



/*----------------------------------------------------------------------
 *
 * 	Menu message
 * 
 *----------------------------------------------------------------------*/

static char menu_message[64];

void set_menu_message(char *msg)
{
    if (!msg)
	menu_message[0] = 0;
    else {
	strncpy(menu_message, msg, sizeof menu_message - 1);
	strupr(menu_message);
    }
}
    


/*----------------------------------------------------------------------
 *
 * 	Menu manager
 * 
 *----------------------------------------------------------------------*/

static int inline touch(int item)
{
    int my = mouse_y * 200.0/SCREEN_H;
    return ((my >= (top + (item * 32) + (text_height(big) / 2) - 16))
	 && (my <= (top + (item * 32) + (text_height(big) / 2) + 16)));
}


void blubber(BLUBBER *start)
{
    BLUBBER *bp;
    int do_action = 0, do_prev = 0;
    int old_mouse_pos = 0;
    int dirty = 1;
    int i;

    menu_end = 0;
    enter_menu(start);
    show_mouse(screen);

    while (!menu_end) {

	/* Handle mouse.  */
	if (old_mouse_pos != mouse_pos) {
	    int saved = selected;
	    
	    for (i = 0; cur[i].proc != prev_menu; i++) 
		if (touch(i)) {
		    selected = i;
		    break;
		}
	    
	    old_mouse_pos = mouse_pos;
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
	    else if (k == KEY_ENTER)
		do_action = 1;
	    else if (k == KEY_ESC)
		do_prev = 1;
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
		
	    clear_bitmap(light);
	    blit(dbuf, light, 160 - 192/2, y, 160 - 192/2, y, 192, 192);
	    draw_trans_sprite(light, dat[L_SPOT].dat, 160 - 192/2, y);
	    
	    /* Little message space.  */
	    if (menu_message[0]) 
		textout_right(light, dat[MINI].dat, menu_message,
			      320-2, 200 - text_height(dat[MINI].dat), 8);
	    
	    /* Blit to screen.  */
	    scare_mouse();
	    blit_to_screen(light);
	    unscare_mouse();
	 
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
		selected = old_mouse_pos = 0;
	    }

	    do_prev = 0;
	    dirty = 1;
	}
    }
}
