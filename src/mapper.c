/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Map editor: three hour job.
 *  06 June, 1998 
 */


#include <string.h>
#include <allegro.h>
#include "blood.h"
#include "colours.h"
#include "globals.h"
#include "map.h"
#include "resource.h"
#include "rpagup.h"



static int tile_count;
static int ammo_count;



enum {
    m_tile,
    m_player,
    m_ammo,
    m_num
} mode;

#define SEL_DRAW    0
#define SEL_CHOOSE  1
#define SEL_EDIT    2

static void (*selector)();

static int first[m_num];
static int chosen[m_num];



static int mx, my;



static void selector_tiles(int command)
{
    int i;
    int kx, ky;
    kx = mouse_x / 16;
    ky = mouse_y / 16;

    switch (command)
    {
	case SEL_DRAW:
	    for (i=0; i<SCREEN_W/16; i++)
	    { 
		if (first[m_tile]+i < tile_count &&
		    tiles[first[m_tile]+i].pic != -1)
		    draw_sprite(dbuf, dat[tiles[first[m_tile]+i].pic].dat, i*16, SCREEN_H-16);
	    }
	    break;

	case SEL_CHOOSE:
	    if (first[m_tile] + kx < tile_count)
		chosen[m_tile] = tiles[first[m_tile]+kx].pic;
	    break;

	case SEL_EDIT:
	    if (kx < mx+map.w && ky < my+map.h)
	    {
		if (mouse_b & 1) 
		    map.tile[ky+my][kx+mx] = chosen[m_tile];
		else 
		    map.tile[ky+my][kx+mx] = 0;
	    }
	    break;
    }
}

static void selector_ammo(int command)
{
    int i;
    int kx, ky;
    kx = mouse_x / 16;
    ky = mouse_y / 16;

    switch (command)
    {
	case SEL_DRAW:
	    for (i=0; i<SCREEN_W/16; i++)
	    { 
		if (first[m_ammo]+i < ammo_count &&
		    ammos[first[m_ammo]+i].pic != -1)
		    draw_sprite(dbuf, dat[ammos[first[m_ammo]+i].pic].dat, i*16, SCREEN_H-16);
	    }
	    break;

	case SEL_CHOOSE:
	    if (first[m_ammo] + kx < ammo_count)
		chosen[m_ammo] = ammos[first[m_ammo]+kx].pic;
	    break;

	case SEL_EDIT:
	    if (kx < mx+map.w && ky < my+map.h)
	    {
		if (mouse_b & 1) 
		    map.ammo[ky+my][kx+mx] = chosen[m_ammo];
		else 
		    map.ammo[ky+my][kx+mx] = 0;
	    }
	    break;
    }
}

static void selector_players(int command)
{
    int kx, ky;
    kx = mouse_x / 16;
    ky = mouse_y / 16;

    switch (command)
    {
	case SEL_DRAW:
	    draw_sprite(dbuf, dat[PLACE1].dat,  0, SCREEN_H-16);
	    break;

	case SEL_CHOOSE:
	    chosen[m_player] = kx;
	    break;

	case SEL_EDIT:
	    if (kx < mx+map.w && ky < my+map.h)
	    {
		int i;
		if (mouse_b & 1)
		{
		    for (i=0; i<24; i++)
		    {
			if (map.start[i].x == kx+mx && map.start[i].y == ky+my)
			    i = 666;
		    }

		    if (i==24)
		    {
			for (i=0; i<24; i++)
			{
			    if (map.start[i].x==255)
			    {
				map.start[i].x = kx+mx;
				map.start[i].y = ky+my;
				break;
			    }
			}
		    }
		}
		else
		{
		    for (i=0; i<24; i++)
		    {
			if (map.start[i].x==kx+mx && map.start[i].y==ky+my)
			{
			    map.start[i].x = 255;
			    break;
			}
		    }
		}
	    }
	    break; 
    }
}



static void map_edit()
{
    int u, v;
    int dirty = 1;
    int mouse_grid_x = -1, mouse_grid_y = -1;

    mode = m_tile;
    selector = selector_tiles;
    reset_map();

    show_mouse(screen);

    while (!(key[KEY_Q] && (key_shifts & KB_CTRL_FLAG)))
    {
	if (key[KEY_PGUP] && first[mode]) 
	    first[mode]--;
	if (key[KEY_PGDN]) 
	    first[mode]++;

	if (key[KEY_I] && map.h>32)  { map.h--; rest(50); dirty = 1; }
	if (key[KEY_K] && map.h<127) { map.h++; rest(50); dirty = 1; }
	if (key[KEY_J] && map.w>64)  { map.w--; rest(50); dirty = 1; }
	if (key[KEY_L] && map.w<127) { map.w++; rest(50); dirty = 1; }

	if (key[KEY_UP] && my) my--, dirty = 1;
	if (key[KEY_LEFT] && mx) mx--, dirty = 1;
	if (key[KEY_RIGHT] && mx < map.w - SCREEN_W/16) mx++, dirty = 1;
	if (key[KEY_DOWN] && my < map.h - (SCREEN_H-16)/16) my++, dirty = 1;

	if (key[KEY_1]) {
	    mode = m_tile;
	    selector = selector_tiles;
	    dirty = 1;
	}
	else if (key[KEY_2]) {
	    mode = m_ammo;
	    selector = selector_ammo;
	    dirty = 1;
	}
	else if (key[KEY_3]) {
	    mode = m_player;
	    selector = selector_players;
	    dirty = 1;
	}

	if (key_shifts & KB_CTRL_FLAG) {
	    if (key[KEY_S])
	    {
		char tmp[1024];
		strcpy(tmp, get_resource(R_SHARE, "maps/"));
		
		if (file_select("Save As...", tmp, "wak"))
		    if (save_map(tmp) < 0)
			alert("Error saving", tmp, "", "Ok", NULL, 13, 27);
		dirty = 1;
	    }
	    else if (key[KEY_L])
	    {
		char tmp[1024];
		strcpy(tmp, get_resource(R_SHARE, "maps/"));

		if (file_select("Load...", tmp, "wak"))
		    if (load_map(tmp) < 0)
			alert("Error loading", tmp, "", "Ok", NULL, 13, 27);
		mx = my = 0;
		dirty = 1;
	    }
	    else if (key[KEY_N])
	    {
		reset_map();
		mx = my = 0;
		dirty = 1;
	    }
	}

	if (mouse_b) {
	    if (mouse_y > SCREEN_H-16)
		selector(SEL_CHOOSE);
	    else 
		selector(SEL_EDIT);
	    dirty = 1;
	} 

	/* drawing */
	if (dirty) {
	    clear_bitmap(dbuf);

	    for (v=0; v<(SCREEN_H-16)/16; v++)
		for (u=0; u<SCREEN_W/16; u++) {
		    if (my+v < map.h && mx+u < map.w) { 
			if (map.ammo[v+my][u+mx])
			    draw_sprite(dbuf, dat[map.ammo[v+my][u+mx]].dat, u*16, v*16);
			if (map.tile[v+my][u+mx])
			    draw_sprite(dbuf, dat[map.tile[v+my][u+mx]].dat, u*16, v*16);
		    }
		}
	    
	    for (u=0; u<24; u++)
		if (map.start[u].x != 255 &&
		    map.start[u].x >= mx && map.start[u].x < mx+SCREEN_W/16 &&
		    map.start[u].y >= my && map.start[u].y < my+(SCREEN_H-16)/16)
		    draw_sprite(dbuf, dat[PLACE1].dat, (map.start[u].x-mx) * 16, (map.start[u].y-my) * 16);

	    selector(SEL_DRAW);

	    mouse_grid_x = mouse_x/16+mx;
	    mouse_grid_y = mouse_y/16+my;
	    text_mode(0);
	    textprintf(dbuf, font, 0, 0, WHITE, "x: %2d  y: %2d  w: %2d  h: %2d", mouse_grid_x, mouse_grid_y, map.w, map.h); 

	    scare_mouse();
	    blit(dbuf, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
	    unscare_mouse();	
	    
	    dirty = 0;
	}
	else {
	    /* not dirty, but maybe update the mouse coordinates anyway */
	    int xx = mouse_x/16+mx;
	    int yy = mouse_y/16+my;
	    if ((xx != mouse_grid_x) || (yy != mouse_grid_y)) {
		mouse_grid_x = xx;
		mouse_grid_y = yy;
		scare_mouse();
		text_mode(0);
		textprintf(screen, font, 0, 0, WHITE, "x: %2d  y: %2d  w: %2d  h: %2d", mouse_grid_x, mouse_grid_y, map.w, map.h); 
		unscare_mouse();
	    }
	}
    }
}



int mapper()
{
    if (set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0) < 0) {
	allegro_message("Error setting 640x480 video mode.\n");
	return 1;
    }
    
    set_window_title("Red Pixel map editor");

    dbuf = create_bitmap(SCREEN_W, SCREEN_H);
    clear_bitmap(dbuf);

    set_mouse_sprite_focus(0,0);

    set_palette(dat[GAMEPAL].dat);
    set_mouse_sprite(dat[XHAIRLCD].dat);

    rpagup_init();

    /* count some */
    for (tile_count = 0; tiles[tile_count].pic >= 0; tile_count++);
    for (ammo_count = 0; ammos[ammo_count].pic >= 0; ammo_count++);

    /* go */
    map_edit();

    /* bye bye */ 
    rpagup_shutdown();
    destroy_bitmap(dbuf);
    dbuf = NULL;
    	       
    return 0;  
}	       
