/* 
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 * 
 *  Game loop (including rendering)
 */


#include <stdio.h>
#include <allegro.h>
#include "gameloop.h"
#include "anim.h"
#include "blood.h"
#include "colours.h"
#include "engine.h"
#include "globals.h"
#include "input.h"
#include "map.h"
#include "packet.h"
#include "player.h"
#include "rnd.h"
#include "sk.h"

#include "backpack.h"
#include "blod.h"
#include "bullet.h"
#include "corpse.h"
#include "explo.h"
#include "message.h"
#include "mine.h"
#include "particle.h"
#include "tiles.h"


#define AMBIENT_LIGHT   16


comm_t comm;

int local;
int mx, my, offsetx, offsety, px, py;		

int shake_factor;
static int shakex, shakey;

static int heart_frame, heart_anim;
static int blood_frame, blood_anim;


static int show_fps = 0;


static void draw_spotlight()
{
    int u = 0, v = 0;

    if ((players[local].fire_frame || players[local].firing)
	&& (players[local].cur_weap != w_knife)
	&& (players[local].cur_weap != w_bow)
	&& (players[local].health)) {
	u = (rnd() % 10) - 5;
	v = (rnd() % 10) - 5;
	blit(light, light, 0, 0, u, v, SCREEN_W, SCREEN_H);
    }

    color_map = &light_map;
    draw_trans_sprite(dbuf, light, 0, 0);
}


static void calc()
{
    /* find top-left */
    mx = players[local].x / 16 - 10;
    my = players[local].y / 16 - 6;

    offsetx = players[local].x % 16;
    offsety = players[local].y % 16;

    if (mx < 0) mx = offsetx = 0;
    if (my < 0) my = offsety = 0;

    if (mx >= map.w - 20) {
	mx = map.w - 20;
	offsetx = 0;
    }

    if ((my > map.h - 13) || ((my == map.h - 13) && (offsety >= 8))) {
	my = map.h - 13;
	offsety = 8;
    }

    px = mx * 16 + offsetx;
    py = my * 16 + offsety;
}

/*----------------------------------------------------------------------*/

typedef struct {
    int frags;
    int no;
} TEMPSORT;

static TEMPSORT tempbuf[MAX_PLAYERS];

static int frag_sorter(const void *e1, const void *e2)
{
    return ((TEMPSORT *) e2)->frags - ((TEMPSORT *) e1)->frags;
}

/*----------------------------------------------------------------------*/

static void draw_status()
{
    int y, i;
    int ammo;
    char buf[10];

    if (show_fps)
	textprintf(dbuf, dat[MINI].dat, 270, 80, WHITE, "FPS: %2d", last_fps);

    /* list scores */
    if (key[KEY_TAB]) {
	textout(dbuf, dat[UNREAL].dat, "FRAG COUNT", 55, 5, -1);

	for (i = 0; i < MAX_PLAYERS; i++) {
	    tempbuf[i].frags = players[i].frags;
	    tempbuf[i].no = i;
	}

	qsort(tempbuf, MAX_PLAYERS, sizeof(TEMPSORT), frag_sorter);

	y = 30;
	for (i = 0; i < MAX_PLAYERS; i++) {
	    if (players[tempbuf[i].no].exist) {
		textprintf(dbuf, dat[UNREAL].dat, 55, y, -1,
			   "%3d %s", players[tempbuf[i].no].frags,
			   players[tempbuf[i].no].name);
		y += 20;
	    }
	}
	return;
    }

    /* dead, don't continue */
    if ((comm != demo) && (players[local].health == 0)) {
	textout(dbuf, dat[UNREAL].dat, "PRESS SPACE TO RESPAWN",
		20, SCREEN_H - 30, -1);
	return;
    }

    text_mode(-1);

    i = 0;
    y = SCREEN_H - 6 - 16;
    while (weapon_order[i].pic) {
	if (players[local].have[weapon_order[i].weap]) {
	    draw_sprite(dbuf, dat[weapon_order[i].pic].dat,
			6 + (players[local].cur_weap == weapon_order[i].weap ? 6 : 0), y);

	    textout(dbuf, dat[MINI].dat, weapon_order[i].s, 0, y + 10, RED);

	    ammo = num_ammo(local, weapon_order[i].weap);
	    if (ammo != -1) {
		sprintf(buf, "%d", ammo);
		textout(dbuf, dat[MINI].dat, buf, 24, y + 6, YELLOW);
		if (weapon_order[i].weap == players[local].cur_weap)
		    textout(dbuf, dat[UNREAL].dat, buf, SCREEN_W - 48, SCREEN_H - 24, -1);
	    }

	    y -= 14;
	}

	i++;
    }

    /* light amp powerup */
    if (players[local].visor_tics)
	draw_sprite(dbuf, dat[A_GOGGLES + (heart_frame > 2 ? 1 : heart_frame)].dat, 152, 9);

    /* bloodlust powerup */
    if (players[local].blood_tics)
	draw_sprite(dbuf, dat[A_BLOODLUST + blood_frame].dat, 152 + 18, 9);

    /* scanner powerup */
    if (players[local].scanner_tics)
	draw_sprite(dbuf, dat[A_SCANNER].dat, 152 - 18, 9);

    /* health and armour */
    draw_sprite(dbuf, dat[HEART1 + (heart_frame > 2 ? 1 : heart_frame)].dat, 6, 9);
    draw_sprite(dbuf, dat[A_ARMOUR].dat, 60, 9);
    textprintf(dbuf, dat[UNREAL].dat, 24, 3, -1, "%3d", players[local].health);
    textprintf(dbuf, dat[UNREAL].dat, 78, 3, -1, "%3d", players[local].armour);
}


static void render()
{
    int bx, by;

    calc();

    /* tile backdrop */
    bx = -(px % 640) / 2;
    by = -(py % 400) / 2;

    blit(dat[BACKDROP].dat, dbuf, 0, 0, bx, by, 320, 200);
    blit(dat[BACKDROP].dat, dbuf, 0, 0, bx, by + 200, 320, 200);
    blit(dat[BACKDROP].dat, dbuf, 0, 0, bx + 320, by, 320, 200);
    blit(dat[BACKDROP].dat, dbuf, 0, 0, bx + 320, by + 200, 320, 200);

    /* ready light */
    clear_to_color(light, AMBIENT_LIGHT);
    color_map = &alpha_map;

    /* draw */
    draw_mines();
    draw_tiles_and_stuff();
    draw_backpacks();
    draw_bullets();
    draw_players();
    draw_particles();
    draw_blods();
    draw_explo();
    draw_corpses();

    if (!players[local].visor_tics
	|| ((players[local].visor_tics < (GAME_SPEED * 3)
	     && (players[local].visor_tics % 2) == 0)))
	draw_spotlight();

    if (players[local].scanner_tics) {
	/* in the last five seconds it screws up */
	draw_scanner((players[local].scanner_tics < GAME_SPEED * 5));
    }

    draw_status();
    draw_msgs();

    show_mouse(NULL);		/* FIXME: use scare mouse instead */
    blit(dbuf, screen, 0, 0, 0 + shakex, 0 + shakey, SCREEN_W, SCREEN_H);
    if (players[local].health && (comm != demo))
	show_mouse(screen);

    frame_counter++;
}


void game_loop()
{
    int frames_dropped;
    int update;
    int i;

    speed_counter = 0;

    while (1) {
	frames_dropped = 0;
	update = 0;

	while (speed_counter > 0 && frames_dropped < 6) {
	    update = 1;

	    calc();

	    if (comm != demo) {
		get_local_input();
		send_local_input();
	    }

	    switch (comm) {
		case peerpeer:
		    while (!skReady()) {
			if (frames_dropped > 1) {
			    frames_dropped--;
			    render();
			}

			if (key[KEY_ESC])
			    goto ask_quit;
		    }

		    if (recv_remote_inputs() < 0)
			return;

		    break;

		case demo:
		    if (recv_demo_inputs() < 0)
			return;
		
		    /* Special demo-mode keys.  */

		    if (key[KEY_S]) {
			rest(10);
			speed_counter = 0;
		    }
		    else if (key[KEY_F]) {
			if (speed_counter < 2)
			    speed_counter += 2;
		    }

		    if (key[KEY_ENTER] && num_players == 2) {
			local = 1 - local;
			add_msgv(-1, "< NOW WATCHING %s >", players[local].name);

			key[KEY_ENTER] = 0;	/* bleh */
		    }
		
		    /* screenshots: dodgy web design stuff */
		    {
			static char ss_name[80];
			static int ss_num = 0;

			if (key[KEY_P]) {
			    while (ss_num <= 9999) {
				sprintf(ss_name, "shot%04d.pcx", ss_num++);
				if (exists(ss_name))
				    continue;
				save_pcx(ss_name, dbuf, dat[GAMEPAL].dat);
				break;
			    }
			    
			    speed_counter = 1;
			    key[KEY_P] = 0;
			}
		    }

		    break;

		default:
		    break;
	    }

	    if (key[KEY_ESC])
		goto ask_quit;

	    respawn_tiles();
	    respawn_ammo();

	    update_mines();
	    update_bullets();
	    update_particles();
	    update_explo();
	    update_blods();
	    update_corpses();
	    update_backpacks();

	    update_msgs();

	    for (i = 0; i < MAX_PLAYERS; i++)
		if (players[i].exist)
		    update_player(i);

	    touch_mines();
	    touch_bullets();
	    touch_backpacks();

	    if (shake_factor > 20)
		shake_factor = 20;

	    if (shake_factor) {
		shake_factor--;
		shakex = (rnd() % (shake_factor * 2 + 1)) - shake_factor;
		shakey = (rnd() % (shake_factor * 2 + 1)) - shake_factor;
	    }

	    if (--heart_anim < 0) {
		heart_anim = HEART_ANIM;
		if (++heart_frame > 3)
		    heart_frame = 0;
	    }

	    if (--blood_anim < 0) {
		blood_anim = HEART_ANIM;	/* just borrow this */
		if (++blood_frame > 5)
		    blood_frame = 0;
	    }

	    if (key[KEY_F12]) {
		show_fps = (show_fps ? 0 : 1);
		key[KEY_F12] = 0;
	    }
	    
	    speed_counter--;
	    frames_dropped++;

	} /* end logic section */

	if (update)
	    render();

	if (key[KEY_ESC])
	    break;
    }

  ask_quit:

    if (comm == peerpeer) {
	skSend(PACKET_QUITGAME);
	while (skRecv() != PACKET_QUITOK)
	    rest(10);
    }

    while (key[KEY_ESC] && mouse_b) ;
    clear_keybuf();
}
