/*  mapper : three hour job
 *  Tjaden 06 June, 1998 
 */

#include "common.h"



extern DATAFILE *dat;
extern BITMAP *dbuf;



int tile_count;
int ammo_count;



enum {
    m_tile,
    m_player,
    m_ammo,
    m_num
} mode;

#define SEL_DRAW    0
#define SEL_CHOOSE  1
#define SEL_EDIT    2

void (*selector)();

int index[m_num];
int chosen[m_num];



int mx, my;



void selector_tiles(int command)
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
		if (index[m_tile]+i < tile_count &&
		    tiles[index[m_tile]+i].pic != -1)
		    draw_sprite(dbuf, dat[tiles[index[m_tile]+i].pic].dat, i*16, SCREEN_H-16);
	    }
	    break;

	case SEL_CHOOSE:
	    if (index[m_tile] + kx < tile_count)
		chosen[m_tile] = tiles[index[m_tile]+kx].pic;
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

void selector_ammo(int command)
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
		if (index[m_ammo]+i < ammo_count &&
		    ammos[index[m_ammo]+i].pic != -1)
		    draw_sprite(dbuf, dat[ammos[index[m_ammo]+i].pic].dat, i*16, SCREEN_H-16);
	    }
	    break;

	case SEL_CHOOSE:
	    if (index[m_ammo] + kx < ammo_count)
		chosen[m_ammo] = ammos[index[m_ammo]+kx].pic;
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

void selector_players(int command)
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



void map_edit()
{
    int u, v;
    int keepy, keepx;

    mode = m_tile;
    selector = selector_tiles;
    reset_map();

    while (!key[KEY_ESC])
    {
	if (key[KEY_PGUP] && index[mode]) 
	    index[mode]--;
	if (key[KEY_PGDN]) 
	    index[mode]++;

	if (key[KEY_MINUS_PAD] && map.h>32) { map.h--; rest(50); }
	if (key[KEY_PLUS_PAD] && map.h<127) { map.h++; rest(50); }
	if (key[KEY_SLASH2] && map.w>64) { map.w--; rest(50); }
	if (key[KEY_ASTERISK] && map.w<127) { map.w++; rest(50); }

	if (key[KEY_UP] && my) my--;
	if (key[KEY_LEFT] && mx) mx--;
	if (key[KEY_RIGHT] && mx < map.w - SCREEN_W/16) mx++;
	if (key[KEY_DOWN] && my < map.h - (SCREEN_H-16)/16) my++;

	if (key[KEY_1]) {
	    mode = m_tile;
	    selector = selector_tiles;
	}
	else if (key[KEY_2]) {
	    mode = m_ammo;
	    selector = selector_ammo;
	}
	else if (key[KEY_3]) {
	    mode = m_player;
	    selector = selector_players;
	}

	if (key[KEY_S])
	{
	    char path[128] = "";
	    if (file_select("Save As...", path, "wak"))
		save_map(path);
	    text_mode(-1);
	}
	else if (key[KEY_L])
	{
	    char path[128] = "";
	    if (file_select("Load...", path, "wak"))
		load_map(path);
	    text_mode(-1);
	    mx = my = 0;
	}
	else if (key[KEY_N])
	{
	    reset_map();
	    mx = my = 0;
	}

	if (mouse_b)
	{
	    if (mouse_y > SCREEN_H-16)
		selector(SEL_CHOOSE);
	    else 
		selector(SEL_EDIT);
	} 

	/* drawing */
	clear(dbuf);

	for (v=0; v<(SCREEN_H-16)/16; v++)
	    for (u=0; u<SCREEN_W/16; u++)
	    {
		if (my+v < map.h && mx+u < map.w)
		{ 
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

	textprintf(dbuf, font, 0, 0, WHITE, "x: %2d  y: %2d  w: %2d  h: %2d", mouse_x/16+mx, mouse_y/16+my, map.w, map.h); 

	/* flicker free update */
	freeze_mouse_flag = 1;
	keepx = mouse_x; 
	keepy = mouse_y;
	blit(dbuf, screen, 0, 0, 0, 0, SCREEN_W, keepy);
	blit(dbuf, screen, 0, keepy+16, 0, keepy+16, SCREEN_W, SCREEN_H-keepy-16);
	vsync();
	show_mouse(NULL);
	blit(dbuf, screen, 0, keepy, 0, keepy, SCREEN_W, 16);
	show_mouse(screen);
	freeze_mouse_flag = 0;
    }
}



int mapper()
{
    int i;

    /* init screen etc */
    set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0);
    dbuf = create_bitmap(SCREEN_W, SCREEN_H);
    clear(dbuf);

    text_mode(-1);
    set_mouse_sprite_focus(0,0);

    set_palette(dat[GAMEPAL].dat);

    gui_fg_color = 0;
    gui_bg_color = WHITE;
    gui_mg_color = GREY;

    /* count some */
    i = 0;
    tile_count = 1;
    while (tiles[i++].pic != -1)
	tile_count++;

    i = 0;
    ammo_count = 1;
    while (ammos[i++].pic != -1)
	ammo_count++; 

    /* go */
    map_edit();

    /* bye bye */ 
    destroy_bitmap(dbuf);
    return 0;
}