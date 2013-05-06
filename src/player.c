/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Players.
 */


#include <string.h>
#include <allegro.h>
#include "backpack.h"
#include "blod.h"
#include "blood.h"
#include "bullet.h"
#include "corpse.h"
#include "engine.h"
#include "explo.h"
#include "gameloop.h"
#include "globals.h"
#include "options.h"
#include "packet.h"
#include "particle.h"
#include "player.h"
#include "map.h"
#include "message.h"
#include "mine.h"
#include "rnd.h"
#include "sound.h"
#include "weapon.h"


PLAYER players[MAX_PLAYERS];
int num_players;


/*----------------------------------------------------------------------*/

static char respawn_order[];

/* next respawn position (0-23) */
int next_position;

void respawn_player(int pl)
{
    PLAYER *guy;
    int i;

    while (1) {
	i = respawn_order[next_position++];
	if (next_position > 24 * 24)
	    next_position = 0;
	if (map.start[i].x != 255) {
	    guy = &players[pl];
	    guy->exist = 1;
	    guy->x = map.start[i].x * 16 + 4;
	    guy->y = map.start[i].y * 16;
	    guy->xv = guy->yv = guy->jump = 0;
	    guy->health = 100;
	    guy->armour = 0;
	    guy->angle = itofix(0);
	    guy->leg_tics = guy->leg_frame = 0;
	    guy->fire_frame = guy->fire_anim = guy->firing = guy->next_fire = 0;
	    guy->num_bullets =
		guy->num_shells =
		guy->num_rockets =
		guy->num_arrows =
		guy->num_mines = 0;
	    for (i = 0; i < num_weaps; i++) guy->have[i] = 0;
	    guy->have[w_knife] = 1;
	    guy->cur_weap = guy->pref_weap = w_knife;
	    guy->visor_tics = 0;
	    guy->bloodlust = guy->blood_tics = 0;
	    guy->scanner_tics = 0;

	    spawn_explo(guy->x, guy->y, RESPAWN0, 7);
	    return;
	}
    }
}


void spawn_players(void)
{
    int i;

    for (i = 0; i < num_players; i++)
	respawn_player(i);
}

/*----------------------------------------------------------------------*/

void hurt_player(int pl, int dmg, int protect, int tag, int deathseq)
{
    if (protect) {		
	/* armour protection */
	players[pl].armour -= dmg / 2;
	players[pl].health -= dmg / 2;
	if (players[pl].armour < 0) {
	    players[pl].health -= abs(players[pl].armour);
	    players[pl].armour = 0;
	}
    }
    else {
	players[pl].health -= dmg;
    }

    /* some nice graphics */
    spawn_particles(players[pl].x + 3, players[pl].y + 6, dmg * (family ? 10 : 20), grad_red);
    if (dmg >= 5)
	spawn_blods(players[pl].x - 4, players[pl].y, dmg * 2);

    /* are we dead yet? are we dead yet? are we dead yet? */
    if (players[pl].health <= 0) {
	if (deathseq == D_IMPALE000)	
	    spawn_corpse(players[pl].x + 3, players[pl].y + 31, players[pl].facing, D_IMPALE000, 20);
	else {
	    switch (irnd() % 5) {
		case 0:	/* arms */
		    spawn_corpse(players[pl].x + 3, players[pl].y + 15, players[pl].facing, D_ARM0, 12);
		    break;

		case 1:	/* drops backpack */
		    spawn_corpse(players[pl].x + 3, players[pl].y + 15, players[pl].facing, D_BACK0, 10);
		    break;
		
		case 2:	/* head */
		    spawn_corpse(players[pl].x + 3, players[pl].y + 15, players[pl].facing, D_HEAD0, 13);
		    break;
		
		case 3:	/* legs */
		    spawn_corpse(players[pl].x + 3, players[pl].y + 15, players[pl].facing, D_LEGS000, 18);
		    break;
		
		case 4:	/* fountain */
		    spawn_corpse(players[pl].x + 3, players[pl].y + 15, players[pl].facing, D_FOUNTAIN000, 19);
		    break;
	    }
	}

	/* drop some ammo */
	spawn_backpack(players[pl].x, players[pl].y, 
		       players[pl].num_bullets / 2,
		       players[pl].num_shells / 2,
		       players[pl].num_rockets / 2,
		       players[pl].num_arrows / 2, 
		       players[pl].num_mines / 2);

	players[pl].health = 0;

	if (tag == pl) {
	    players[tag].frags--;
	    if (deathseq == D_IMPALE000)
		add_msgf(-1, "%s was impaled", players[pl].name);
	    else
		add_msgf(-1, "%s committed suicide", players[pl].name);
	}
	else {
	    players[tag].frags++;
	    add_msgf(-1, "%s was fragged by %s",
		     players[pl].name, players[tag].name);
	}

	snd_3d(WAV_DEAD1 + (irnd() % 5), 255, players[pl].x, players[pl].y);
    }
    else {
	snd_3d(WAV_ARR + (irnd() % 5), 255, players[pl].x, players[pl].y);
    }
}

/*----------------------------------------------------------------------*/

int gun_pic(int pl)
{
    switch (players[pl].cur_weap) {
	case w_bow: 	return P_BOW0;
	case w_knife: 	return P_KNIFE0;
	case w_m16:	return P_M160;
	case w_minigun: return P_MINI0;
	case w_pistol:  return P_PISTOL0;
	case w_bazooka: return P_ROCKET0;
	case w_shotgun: return P_SHOTGUN0;
	case w_uzi:	return P_UZI0;
	default: return 0;
    }
}


void draw_players(void)
{
    int i;
    int gun;
    int u, v, w, h;
    PLAYER *guy;

    for (i = 0; i < MAX_PLAYERS; i++) {
	guy = &players[i];
	if (!guy->health) 
	    continue;

	u = guy->x - px;
	v = guy->y - py;

	gun = gun_pic(i);
	if (guy->facing == left)
	    gun += 4;

	/* minigun is weird */
	if (guy->cur_weap == w_minigun && guy->fire_frame == 4)
	    gun += 2;
	else
	    gun += guy->fire_frame;

	w = ((BITMAP *) dat[gun].dat)->w / 2;
	h = ((BITMAP *) dat[gun].dat)->h / 2;

	if (guy->facing == right) {
	    draw_sprite(dbuf, dat[P_BODY].dat, u, v);
	    draw_sprite(dbuf, dat[P_LEG0 + guy->leg_frame].dat, u - 1, v);
	    rotate_sprite(dbuf, dat[gun].dat, u + 3 - w, v + 6 - h, guy->angle);
	}
	else {
	    draw_sprite_h_flip(dbuf, dat[P_BODY].dat, u, v);
	    draw_sprite_h_flip(dbuf, dat[P_LEG0 + guy->leg_frame].dat, u - 8, v);
	    rotate_sprite(dbuf, dat[gun].dat, u + 2 - w, v + 6 - h, guy->angle - itofix(128));
	}

	draw_light(L_SPOT, u + 3, v + 3);
    }
}


void draw_scanner(int haywire)
{
    PLAYER *p, *other;
    fixed angle;
    int cx, cy, x1, y1, x2, y2;

    if (haywire && (rnd() % 5) == 0)
	return;

    /* who's the lucky bastard with the radar? */
    p = &players[local];
    other = &players[1 - local];
    
    cx = p->x + 8 - px;
    cy = p->y + 8 - py;
    circle(dbuf, cx, cy, 25, 254);
    if (num_players < 2)
	return;

    if (haywire && (rnd() % 3) == 0)
	angle = (rnd() % 256) << 16;
    else
	angle = find_angle(p->x + 8, p->y + 8, other->x + 8, other->y + 8);

    x1 = cx + fixtoi(fixmul(fixcos(angle), 20 << 16));
    y1 = cy + fixtoi(fixmul(fixsin(angle), 20 << 16));
    x2 = cx + fixtoi(fixmul(fixcos(angle), 40 << 16));
    y2 = cy + fixtoi(fixmul(fixsin(angle), 40 << 16));
    line(dbuf, x1, y1, x2, y2, 250);
}

/*----------------------------------------------------------------------*/

static int next_best[] = { w_minigun, w_m16, w_shotgun, w_uzi, w_pistol, 0 };

void auto_weapon(int pl, int new_weapon)
{
    PLAYER *guy = &players[pl];
    int i;

    /*
     * 1. NEVER select bow or bazooka for you
     * 2. If you get a NEW weapon that is better than your preferred weapon,
     *    then change to that, unless you are holding the bazooka or bow.
     * 3. If you get ammo for your preferred weapon, switch to preferred.
     * 4. If your preferred weapon runs out of ammo, choose the next best.
     */

    /* 2. If you get a NEW weapon that is better than your preferred weapon,
     *          then change to that, unless you are holding the bazooka or bow.  */
    if (players[pl].have[new_weapon])
	new_weapon = 0;

    players[pl].have[new_weapon] = 1;

    if (new_weapon) {
	if (players[pl].cur_weap == w_bow
	    || players[pl].cur_weap == w_bazooka)
	    return;

	i = 0;
	while (next_best[i]) {
	    if ((new_weapon == next_best[i] && num_ammo(pl, new_weapon)) ||
		(guy->cur_weap == next_best[i] && num_ammo(pl, guy->cur_weap))) {
		players[pl].cur_weap = players[pl].pref_weap = next_best[i];
		return;
	    }
	    i++;
	}
    }

    /* 3. If you get ammo for your preferred weapon, switch to preferred.  */
    if (num_ammo(pl, guy->pref_weap)) {
	guy->cur_weap = guy->pref_weap;
	return;
    }

    /* 
     * Otherwise no ammo, select next best weapon that you
     *  (a) are in possesion of 
     *  (b) have ammo for
     */
    i = 0;
    while (next_best[i]) {
	if (players[pl].have[next_best[i]] && num_ammo(pl, next_best[i])) {
	    players[pl].cur_weap = next_best[i];

	    if ((players[pl].pref_weap == w_bazooka)
		|| (players[pl].pref_weap == w_bow))
		players[pl].pref_weap = players[pl].cur_weap;
	    return;
	}
	i++;
    }

    /* Ha!  */
    players[pl].cur_weap = w_knife;
}


static int next_have_weapon_order(PLAYER * guy, int wo)
{
    if (!weapon_order[++wo].pic)
	return -1;
    while (!guy->have[weapon_order[wo].weap]) {
	if (!weapon_order[wo].pic)
	    return -1;
	wo++;
    }
    return wo;
}


static int prev_have_weapon_order(PLAYER * guy, int wo)
{
    /* well, I fucked this up good */
    if (weapon_order[wo].weap == w_mine)
	return -1;
    --wo;
    while (!guy->have[weapon_order[wo].weap]) {
	if (weapon_order[wo].weap == w_mine)
	    return -1;
	wo--;
    }
    return wo;
}


void select_weapon(void)
{
    static int next, prev;
    static int old_mouse_z = 0;
    PLAYER *guy = &players[local];
    int i = 0, select;

    /* hack */
    if (key[KEY_Q] || key[KEY_R] || (mouse_z < old_mouse_z)) {
	if (prev < 2)
	    prev++;
    }
    else
	prev = 0;

    if (key[KEY_E] || key[KEY_Y] || (mouse_z > old_mouse_z)) {
	if (next < 2)
	    next++;
    }
    else
	next = 0;

    if (prev && next)
	next = prev = 0;

    old_mouse_z = mouse_z;

    do {
	/* got the weapon?  */
	if (guy->have[weapon_order[i].weap]) {
	    select = 0;

	    if (mouse_b & 2)
		if (weapon_order[i].weap == w_mine)
		    select = 1;
	    if (key[weapon_order[i].shortcut])
		select = 1;

	    /* want it?  */
	    /* FIXME: this is a mess  */
	    if ((select && weapon_order[i].weap != guy->cur_weap) ||
		(next == 1 && weapon_order[i].weap != w_mine &&
		 weapon_order[prev_have_weapon_order(guy, i)].weap == guy->cur_weap) ||
		(prev == 1 && weapon_order[i].weap != w_mine &&
		 weapon_order[next_have_weapon_order(guy, i)].weap == guy->cur_weap)) {
		if (weapon_order[i].weap == w_mine && guy->num_mines > 0)
		    guy->drop_mine = 1;
		else
		    guy->select = weapon_order[i].weap;
		return;
	    }
	}

    } while (weapon_order[i++].pic);
}


int num_ammo(int pl, int weapon)
{
    switch (weapon) {
	case w_pistol:
	case w_uzi:
	case w_m16:
	case w_minigun: return players[pl].num_bullets;
	case w_shotgun: return players[pl].num_shells;
	case w_bazooka: return players[pl].num_rockets;
	case w_bow: 	return players[pl].num_arrows;
	case w_mine: 	return players[pl].num_mines;
	default: return -1;
    }
}

/*----------------------------------------------------------------------*/

void get_local_input(void)
{
    int mx = mouse_x * 320.0/SCREEN_W;
    int my;

    if (SCREEN_H == 480)
	my = (mouse_y - 40) / 2;
    else if (SCREEN_H == 400)
	my = mouse_y / 2;
    else if (SCREEN_H == 240)
	my = mouse_y - 20;
    else
	my = mouse_y;

    players[local].angle = find_angle(players[local].x - px + 3,
				      players[local].y - py + 4,
				      mx, my);
    
    players[local].facing = (mx + px > players[local].x) ? right : left;

    players[local].up = key[KEY_UP] | key[KEY_T] | key[KEY_W];
    players[local].down = key[KEY_DOWN] | key[KEY_G] | key[KEY_S];
    players[local].left = key[KEY_LEFT] | key[KEY_F] | key[KEY_A];
    players[local].right = key[KEY_RIGHT] | key[KEY_H] | key[KEY_D];
    players[local].respawn = key[KEY_SPACE] & (!players[local].health);

    players[local].fire = 0;
    players[local].drop_mine = 0;
    players[local].select = 0;

    if (!players[local].next_fire) {
	players[local].fire = (mouse_b & 1);
	select_weapon();
    }
}


int load_playerstat(const char *packet)
{
    int pl, ch;

    pl = packet[0];

    ch = packet[1];
    players[pl].up     	  = ch & INPUT_UP; 
    players[pl].down  	  = ch & INPUT_DOWN;
    players[pl].left  	  = ch & INPUT_LEFT;
    players[pl].right 	  = ch & INPUT_RIGHT;
    players[pl].fire  	  = ch & INPUT_FIRE;
    players[pl].drop_mine = ch & INPUT_DROPMINE;
    players[pl].respawn   = ch & INPUT_RESPAWN;
    players[pl].facing 	  = (ch & INPUT_FACINGLEFT) ? left : right;

    players[pl].select = packet[2];
    players[pl].angle = packet[3] << 16;
    
    return pl;
}


void make_playerstat(char *packet, int pl)
{
    packet[0] = pl;

    packet[1] = 0;
    if (players[pl].up)			packet[1] |= INPUT_UP;
    if (players[pl].down) 		packet[1] |= INPUT_DOWN;
    if (players[pl].left)		packet[1] |= INPUT_LEFT;
    if (players[pl].right) 		packet[1] |= INPUT_RIGHT;
    if (players[pl].fire)		packet[1] |= INPUT_FIRE;
    if (players[pl].drop_mine)		packet[1] |= INPUT_DROPMINE;
    if (players[pl].respawn)		packet[1] |= INPUT_RESPAWN;
    if (players[pl].facing == left)	packet[1] |= INPUT_FACINGLEFT;	

    packet[2] = players[pl].select;
    packet[3] = (players[pl].angle >> 16) & 0xff;
}


/*----------------------------------------------------------------------*/

/* Removes player, and all mines and bullets he placed */
void clean_player(int pl)
{
    players[pl].health = players[pl].exist = players[pl].frags = 0;

    reset_bullets_with_tag(pl);
    reset_mines_with_tag(pl);
}


static char retain_names[MAX_PLAYERS][40];
static int retain_frags[MAX_PLAYERS];

void retain_players(void)
{
    int i;

    for (i = 0; i < MAX_PLAYERS; i++) {
	strcpy(retain_names[i], players[i].name);
	retain_frags[i] = players[i].frags;
    }
}

void restore_players(void)
{
    int i;

    for (i = 0; i < MAX_PLAYERS; i++) {
	strcpy(players[i].name, retain_names[i]);
	players[i].frags = retain_frags[i];
    }
}

void reset_players(void)
{
    memset(players, 0, MAX_PLAYERS * sizeof(PLAYER));
}

void reset_player_frags(void)
{
    int i;
    
    for (i = 0; i < MAX_PLAYERS; i++)
	players[i].frags = 0;
}

/*----------------------------------------------------------------------*/

static char respawn_order[] =
{
    /* 24x24 numbers, ranging from 0 to 23 */
    21,18,19,14,7,9,6,22,3,11,17,0,23,10,8,1,4,20,13,16,2,15,12,5,
    12,0,8,7,9,10,11,23,21,3,15,13,1,4,20,22,16,17,18,5,19,2,14,6,
    16,8,7,4,12,18,14,0,3,21,10,6,11,20,13,1,19,17,9,2,22,23,15,5,
    16,18,14,23,17,2,19,11,5,9,4,22,7,6,13,1,12,20,3,21,8,10,0,15,
    5,3,6,17,0,23,20,7,15,4,1,22,14,10,19,2,8,13,18,21,11,9,16,12,
    21,18,16,3,7,12,4,2,19,17,10,5,0,11,22,14,13,15,6,1,23,8,20,9,
    12,3,4,14,5,22,11,21,13,16,18,19,23,0,2,6,15,17,20,9,1,7,10,8,
    16,7,21,13,1,10,9,6,2,17,3,12,19,18,8,14,11,20,23,15,0,5,22,4,
    19,18,3,17,9,15,23,10,16,22,13,1,2,11,14,21,6,12,8,0,20,4,5,7,
    18,23,20,3,10,21,13,14,6,12,8,17,1,15,9,22,19,11,16,0,7,2,4,5,
    22,17,9,4,5,16,14,6,8,19,20,3,13,7,12,10,18,2,23,15,11,1,21,0,
    4,2,7,9,14,19,17,10,11,1,22,6,23,5,13,20,12,0,21,3,8,15,16,18,
    12,20,10,14,17,1,6,5,13,11,8,23,15,16,22,19,9,2,4,21,7,18,0,3,
    1,8,0,22,9,23,3,20,13,2,19,18,12,16,21,17,11,7,10,5,14,15,6,4,
    6,18,12,3,15,0,1,9,22,7,17,16,21,10,14,11,2,20,4,23,5,19,8,13,
    22,20,1,10,4,11,15,2,5,23,17,6,18,9,19,0,3,8,14,21,7,12,13,16,
    14,8,1,22,4,3,15,23,5,7,19,0,11,20,10,18,21,6,2,16,9,12,17,13,
    17,22,0,3,11,4,10,7,8,23,9,12,14,2,5,15,16,20,21,18,1,13,6,19,
    2,9,23,20,10,5,14,12,0,3,22,18,16,1,11,15,8,4,13,7,21,17,6,19,
    16,2,6,14,23,22,20,17,15,5,21,4,0,9,12,7,18,13,19,11,8,10,1,3,
    16,18,3,20,8,9,21,10,23,6,17,22,4,5,7,2,15,11,0,12,19,1,14,13,
    19,21,8,13,4,3,23,17,2,7,14,5,18,6,0,22,9,16,10,15,12,1,20,11,
    15,23,7,1,9,14,6,12,10,20,4,3,5,21,2,0,8,19,22,13,16,11,18,17,
    10,20,19,18,7,16,11,0,2,22,14,9,15,1,8,23,13,5,4,12,21,3,6,17
};
