/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 * 
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 * 
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Email: tjaden@psynet.net
 *  WWW:   http://www.psynet.net/tjaden/
 * 
 *  Game engine.
 */


#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include "fastsqrt/fastsqrt.h"
#include "sk.h"
#include "common.h"
#include "rg_rand.h"
#include "run.h"
#include "stats.h"
#include "statlist.h"
#include "setweaps.h"
#include "demo.h"
#include "demintro.h"


//---------------------------------------------------------------- maxs ------

#define AMBIENT_LIGHT   16

#define MINE_ANIM       10
#define EXPLO_ANIM      5
#define LEG_ANIM        3              /* used to be 1! :) */
#define HEART_ANIM      7
#define MSG_ANIM        140
#define CORPSE_ANIM     12

int max_bullets =       200;
int max_mines =         50;
int max_backpacks =     50;
#define MAX_EXPLOSIONS  100

#define MAX_PARTICLES   5000
#define MAX_BLODS       1000
#define MAX_CORPSES     50


//---------------------------------------------------------------- globals ---

DATAFILE    *dat;
BITMAP      *dbuf;
BITMAP      *light;

RGB_MAP     rgb_table;
COLOR_MAP   alpha_map;
COLOR_MAP   light_map;

comm_t comm;


//---------------------------------------------------------------- storage ---

WEAPON weaps[num_weaps];

PARTICLE    *bullets;
MINE        *mines;
BACKPACK    *backpacks;
PARTICLE    particles[MAX_PARTICLES];
EXPLOSION   explos[MAX_EXPLOSIONS];
BLOD        blods[MAX_BLODS];

CORPSE      corpses[MAX_CORPSES];
int         oldest_corpse;      // the oldest gets replaced

PLAYER      players[MAX_PLAYERS];
int         num_players;


//---------------------------------------------------------------- local -----

int local;          // local player

int mx, my,             // top left of map in tiles
    offsetx, offsety,   //  && the offset
    px, py;             // top left of map in pixels

int shake_factor;   // screen shakes

int heart_frame = 0, heart_anim = 0;
int blood_frame = 0, blood_anim = 0;

int record_demos = 0;


//------------------------------------------------------------ timer ---------

volatile int speed_counter = 0;

void speed_timer() 
{ 
    speed_counter++;
}

END_OF_FUNCTION(speed_timer);


//------------------------------------------------------------ fps counter ---

volatile int frame_counter = 0, last_fps = 0;

void fps_timer()
{
    last_fps = frame_counter;
    frame_counter = 0;
}

END_OF_FUNCTION(fps_timer);


//------------------------------------------------------------ cyanide -------

void killall()
{
    unload_datafile(dat);
    destroy_bitmap(dbuf);
    destroy_bitmap(light);
    free(bullets);
    free(mines);
    free(backpacks);
    allegro_exit();
}

void suicide(char *s)
{
    killall();
    skClose();
    allegro_message("%s\n", s);
    exit(1);
}


//----------------------------------------------- random number generator ----

static long _rnd;

int rnd()
{
    return (_rnd = longrand(_rnd));
}

void srnd(unsigned long seed)
{
    _rnd = slongrand(seed);
}


// this one is used for the more important stuff
// the stuff that has to be synchronised

static long _irnd;

int irnd()
{
    return (_irnd = longrand(_irnd));
}

void sirnd(unsigned long seed)
{
    _irnd = slongrand(seed);
}


//----------------------------------------------- misc little things ---------

inline int tile_collide(int x, int y) 
{
    // more like "tile_at"
    return map.tile[y/16][x/16];
}

inline int bb_collide(BITMAP *spr1, int x1, int y1, BITMAP *spr2, int x2, int y2)
{
    if ((x1 > x2 + spr2->w) || (x2 > x1 + spr1->w) ||
	(y1 > y2 + spr2->h) || (y2 > y1 + spr1->h))
	return 0;
    return 1;
}

inline fixed find_angle(int x1, int y1, int x2, int y2)
{
    return fatan2(itofix(y2-y1), itofix(x2-x1)) & 0x00ff0000;  // get rid of decimals
}

inline int find_distance(int x1, int y1, int x2, int y2)
{
    int a, b;
    a = x1-x2;
    b = y1-y2;
    a *= a;
    b *= b;

    /* fast sqrt, asm version -- fastest; does not work under Linux */
    // return (int)fast_fsqrt_asm((float)a + (float)b);

    /* fast sqrt, C version -- medium; works under DOS and Linux */
    return (int)fast_fsqrt((float)a + (float)b);
    
    /* libc version -- slowest; libc dependant */
    // return (int)sqrt((float)a + (float)b);
}

inline void draw_light(int bmp, int cx, int cy)
{
    BITMAP *mask = dat[bmp].dat;
    draw_trans_sprite(light, mask, cx-(mask->w/2), cy-(mask->h/2));
}

inline int num_ammo(int pl, int weapon)
{
    switch (weapon)
    {
	case w_pistol:
	case w_uzi:
	case w_m16:
	case w_minigun: return players[pl].num_bullets;
	case w_shotgun: return players[pl].num_shells;
	case w_bazooka: return players[pl].num_rockets;
	case w_bow:     return players[pl].num_arrows;
	case w_mine:    return players[pl].num_mines;
	default: return -1;
    }
}


//---------------------------------------------------------- sound -----------

inline void snd_local(int snd)
{
    play_sample(dat[snd].dat, 255, 128, 1000, 0);
}

void snd_3d(int snd, int maxvol, int sourcex, int sourcey)
{
    int dist, pan;

    dist = find_distance(players[local].x, players[local].y, sourcex, sourcey) /2;

    if (sourcex < players[local].x)
	pan = 128 - dist * 3;
    else
	pan = 128 + dist * 3;

    if (pan<0) pan =0;
    else if (pan>255) pan=255;

    maxvol-=dist;
    if (maxvol>0)
	play_sample(dat[snd].dat, maxvol, pan, 1000, 0);
}


//---------------------------------------------------------- messages --------

#define MAX_MSGS    5
#define MSG_LEN     40

char msg_queue[MAX_MSGS][MSG_LEN];
int num_msgs = 0, msg_tics = 0;

void add_msg(char *s, int to_player)
{
    int i;

    if (to_player!=local && to_player != -1)
	return;

    if (!num_msgs)
	msg_tics = MSG_ANIM;

    if (num_msgs==MAX_MSGS)
    {
	for (i=0; i<MAX_MSGS-1; i++)
	    memcpy(msg_queue[i], msg_queue[i+1], MSG_LEN);
    } 
    else
	num_msgs++; 

    strcpy(msg_queue[num_msgs-1], s);
}

void v_msg(int targ, char *fmt, ...)
{
    va_list va;
    char buf[200];
    va_start(va, fmt);
    vsprintf(buf, fmt, va);
    add_msg(buf, targ);
}

void update_msgs()
{
    int i;
    if (--msg_tics==0)
    {
	msg_tics = MSG_ANIM;
	if (num_msgs)
	{
	    for (i=0; i<MAX_MSGS-1; i++)
		memcpy(msg_queue[i], msg_queue[i+1], MSG_LEN);
	    memset(msg_queue[MAX_MSGS-1], 0, MSG_LEN);
	    num_msgs--;
	}
    }
}

void draw_msgs()
{
    int i;
    for (i=0; i<num_msgs; i++)
	textout(dbuf, dat[MINI].dat, msg_queue[i], SCREEN_W-16-text_length(dat[MINI].dat, msg_queue[i]), 3+6*i, GREEN - num_msgs*2 + i*2);
}


//---------------------------------------------------------- blods -----------

void spawn_blods(int x, int y, int num)
{
    int i;

    for (i=0; i<MAX_BLODS; i++)
    {
	if (!blods[i].life)
	{
	    blods[i].x = x + (rnd()%16*2) - 8;
	    blods[i].y = y + (rnd()%16*2) - 8;
	    blods[i].life = (rnd()%400) + 300;
	    blods[i].pic = BLOD0 + (rnd()%15);
	    if (--num<=0)
		return;
	}
    }

    add_msg("BLODS OVERFLOW <AAARRGGH!!>", local);
}

void update_blods()
{
    int i;
    for (i=0; i<MAX_BLODS; i++)
    {
	if (blods[i].life)
	{
	    blods[i].life--;
	    if (!tile_collide(blods[i].x, blods[i].y + ((BITMAP*)dat[blods[i].pic].dat)->h - 1))
		blods[i].y++;
	}
    }
}

void draw_blods()
{
    int i;
    for (i=0; i<MAX_BLODS; i++)
    {
	if (blods[i].life)
	    draw_sprite(dbuf, dat[blods[i].pic].dat, 
			blods[i].x-px, blods[i].y-py);
    }
}


//---------------------------------------------------------- particles -------

enum
{
    grad_red,
    grad_orange,

    //   0-31   black
    //  48-63   dark blue
    //  64-95   brown
    //  96-127  red
    // 128-159  darker brown 
    // 160-175  orange - yellow
    // 176-191  greyish
    // 192-233  light blue
    // 224-239  purple 
    // 240-255  green
};

void spawn_particles(int x, int y, int num, int grad)
{
    int i;
    int hi = 0, low=0;
    fixed angle, spd;

    switch (grad)
    {
	case grad_red:
	    low = 96;
	    hi = 127;
	    break;
	case grad_orange:
	    low = 160;
	    hi = 175;
	    break;
    };

    for (i=0; i<MAX_PARTICLES; i++)
    {
	if (!particles[i].life)
	{ 
	    spd = itofix((rnd()%40) + 1);
	    spd = fdiv(spd, itofix(10));
	    particles[i].x = itofix(x);
	    particles[i].y = itofix(y);
	    angle = itofix(rnd()%256);
	    particles[i].xv = fmul(fcos(angle), spd);
	    particles[i].yv = fmul(fsin(angle), spd);
	    if (grad==grad_red) { // blood lasts longer! 
		particles[i].life = ((rnd()%35)+15) * 5;
	    }
	    else 
		particles[i].life = (rnd()%40)+20;
	    particles[i].colour = (rnd()%(hi-low)) + low;
	    if (--num==0)
		return;
	}
    } 

    /* this isn't very bad, but warn anyway */
    add_msg("PARTICLE OVERFLOW", local);
}

void spawn_casing(int x, int y, int facing)
{
    // specialised copy of spawn_particles
    int i;

    for (i=0; i<MAX_PARTICLES; i++)
    {
	if (!particles[i].life)
	{ 
	    particles[i].x = itofix(x);
	    particles[i].y = itofix(y);
	    particles[i].xv = (facing==left ? itofix(1) : itofix(-1));
	    particles[i].yv = itofix(-1);
	    particles[i].life = 20;
	    particles[i].colour = 175;
	    return;
	}
    } 

    add_msg("PARTICLE OVERFLOW (CASING)", local);
}

void update_particles()
{
    int i;
    int t;
    for (i=0; i<MAX_PARTICLES; i++)
    {
	if (particles[i].life)
	{
	    particles[i].life--;

	    particles[i].x += particles[i].xv;
	    particles[i].y += (particles[i].yv += ftofix(.15));

	    t = map.tile[fixtoi(particles[i].y)/16][fixtoi(particles[i].x)/16];
	    if (t && t!=T_LAD) {
		particles[i].xv *= -.5;
		particles[i].yv *= -.5; 
		if (abs(particles[i].xv) < ftofix(.0) && abs(particles[i].yv) < ftofix(.0) && particles[i].life < 10)
		    particles[i].life = 0;
	    }
	}
    }
}

void draw_particles()
{
    int i;
    for (i=0; i<MAX_PARTICLES; i++)
    {
	if (particles[i].life)
	    putpixel(dbuf, fixtoi(particles[i].x) - px, fixtoi(particles[i].y) - py, particles[i].colour);
    } 
}


//---------------------------------------------------------- explosions ------

void spawn_explo(int x, int y, int pic, int frames)
{
    int i;
    EXPLOSION *ex;
    for (i=0; i<MAX_EXPLOSIONS; i++)
    {
	if (!explos[i].alive)
	{
	    ex = &explos[i]; 
	    ex->alive = 1;
	    ex->x = x;
	    ex->y = y;
	    ex->pic = pic;
	    ex->cur = 0;
	    if (!pic)
		ex->nopic = 1;
	    else
		ex->nopic = 0;
	    ex->frames = frames;
	    ex->anim = EXPLO_ANIM;
	    return;
	}
    }

    // not bad, just warning
    add_msg("EXPLOSION OVERFLOW", local);
}

void update_explo()
{
    int i;
    for (i=0; i<MAX_EXPLOSIONS; i++)
    {
	if (explos[i].alive && --explos[i].anim==0)
	{
	    explos[i].anim = EXPLO_ANIM;
	    if (++explos[i].cur >= explos[i].frames) 
		explos[i].alive = 0;
	}
    }
}

void draw_explo()
{
    int i;
    for (i=0; i<MAX_EXPLOSIONS; i++)
    {
	if (explos[i].alive) {
	    if (!explos[i].nopic)
		draw_sprite(dbuf, dat[explos[i].pic+explos[i].cur].dat, explos[i].x-px, explos[i].y-py);
	    draw_light(L_EXPLO, explos[i].x-px, explos[i].y-py);
	}
    }
}


//---------------------------------------------------------- corpses ---------

void spawn_corpse(int x, int y, int facing, int first, int frames)
{
    int i;
    for (i=0; i<MAX_CORPSES; i++)
    {
	if (!corpses[i].alive)
	{
	    corpses[i].x = x - ((BITMAP *)dat[first].dat)->w / 2;
	    corpses[i].y = y - ((BITMAP *)dat[first].dat)->h;
	    corpses[i].facing = facing;
	    corpses[i].first_frame = first;
	    corpses[i].num_frames = frames;
	    corpses[i].cur = 0;
	    corpses[i].anim = CORPSE_ANIM;
	    corpses[i].alive = 1;
	    return;
	}
    }

    corpses[oldest_corpse].alive = 0;
    if (++oldest_corpse > MAX_CORPSES-1)
	oldest_corpse = 0;
    spawn_corpse(x, y, facing, first, frames);
}

void update_corpses()
{
    int i;
    for (i=0; i<MAX_CORPSES; i++)
    {
	if (corpses[i].alive)
	{
	    if (--corpses[i].anim==0)
	    {
		if (++corpses[i].cur >= corpses[i].num_frames) 
		{
		    corpses[i].cur = corpses[i].num_frames-1;
		    corpses[i].anim = 0xff;
		}
		else
		{
		    corpses[i].anim = CORPSE_ANIM;
		}
	    }

	    if (!tile_collide(corpses[i].x + ((BITMAP*)dat[corpses[i].first_frame].dat)->w/2, corpses[i].y + ((BITMAP*)dat[corpses[i].first_frame].dat)->h - 1))
		if (++corpses[i].y > map.h * 16)
		    corpses[i].alive = 0;
	}
    }
}

void draw_corpses()
{
    int i;
    for (i=0; i<MAX_CORPSES; i++)
    {
	if (corpses[i].alive)
	{
	    if (corpses[i].facing==right)
		draw_sprite(dbuf, dat[corpses[i].first_frame + corpses[i].cur].dat, corpses[i].x-px, corpses[i].y-py);
	    else
		draw_sprite_h_flip(dbuf, dat[corpses[i].first_frame + corpses[i].cur].dat, corpses[i].x-px, corpses[i].y-py);

	    if (corpses[i].cur < corpses[i].num_frames-1)
		draw_trans_sprite(light, dat[L_SPOT].dat, corpses[i].x-px-192/2, corpses[i].y-py-192/2);
	}
    }
}


//---------------------------------------------------------- backpacks -------

void reset_backpacks(int lower)
{
    int i;
    for (i=lower; i<max_backpacks; i++)
	backpacks[i].alive = 0;
}

void spawn_backpack(int x, int y, int b, int s, int r, int a, int m)
{
    int i;
    for (i=max_backpacks-1; i>=0; i--)
    {
	if (!backpacks[i].alive)
	{
	    backpacks[i].x = x;
	    backpacks[i].y = y;
	    backpacks[i].num_bullets = b;
	    backpacks[i].num_shells = s;
	    backpacks[i].num_rockets = r;
	    backpacks[i].num_arrows = a;
	    backpacks[i].num_mines = m;
	    backpacks[i].alive = 1;
	    // FIXME: this is just average time it takes
	    backpacks[i].unactive = 13*CORPSE_ANIM; 
	    return;
	}
    }

    max_backpacks += 5;
    backpacks = realloc(backpacks, max_backpacks);
    if (backpacks==NULL)
	suicide("Backpack overflow"); 
    reset_backpacks(max_backpacks-5);
    spawn_backpack(x, y, b, s, r, a, m);
    add_msg("ALLOCED EXTRA 5 BACKPACKS", local);
}

void update_backpacks()
{
    int i;
    for (i=max_backpacks-1; i>=0; i--)
    {
	if (backpacks[i].alive)
	{
	    if (backpacks[i].unactive) {
		--backpacks[i].unactive;
	    }
	    else 
	    {
		if (!tile_collide(backpacks[i].x + 8, backpacks[i].y+16)) {
		    if (++backpacks[i].y>map.h*16)
			backpacks[i].alive = 0; 
		}
	    }
	}
    } 
}

void draw_backpacks()
{
    int i;
    for (i=max_backpacks-1; i>=0; i--)
    {
	if (backpacks[i].alive && !backpacks[i].unactive) 
	{
	    draw_sprite(dbuf, dat[A_BACKPACK].dat, backpacks[i].x-px, backpacks[i].y-py);
	    draw_light(L_EXPLO, backpacks[i].x+6-px, backpacks[i].y+8-py);
	    // backpacks are 12x16
	}
    } 
}

void touch_backpacks()
{
    int i, j;
    for (i=max_backpacks-1; i>=0; i--)
    {
	if (backpacks[i].alive && !backpacks[i].unactive)
	{ 
	    for (j=0; j<MAX_PLAYERS; j++)
	    {
		if (players[j].health)
		{
		    if (bb_collide(dat[A_BACKPACK].dat, backpacks[i].x, backpacks[i].y, dat[P_BODY].dat, players[j].x-4, players[j].y))
		    {
			players[j].num_bullets += backpacks[i].num_bullets;
			players[j].num_shells += backpacks[i].num_shells;
			players[j].num_rockets += backpacks[i].num_rockets;
			players[j].num_arrows += backpacks[i].num_arrows;
			players[j].num_mines += backpacks[i].num_mines;
			if (players[j].num_mines)
			    players[j].have[w_mine] = 1;
			backpacks[i].alive = 0; 
			add_msg("YOU STOLE A BACKPACK", j);
			snd_local(WAV_PICKUP);
		    }
		}
	    }
	}
    }
}


//---------------------------------------------------------- blast -----------

void blow_mine(int num);
int hurt_tile(int u, int v, int dmg, int tag);
void hurt_player(int pl, int dmg, int protect, int tag, int deathseq);

void blast(int x, int y, int dmg, int tag)
{
    int u, v, d;
    int i, j;
    fixed angle;

    u = x/16;
    v = y/16;

    shake_factor += 10;

    // radius in pixels, 40 pixels around

    // barrels, wood, crates 
    for (j = -2; j<=2; j++) {
	for (i = -2; i<=2; i++) {
	    if (v+j>=0 && v+j<map.h && u+i>=0 && u+i<map.w)
	    {
		d = find_distance(u, v, u+i, v+j);
		if (d==0) d=1;
		hurt_tile(u+i, v+j, (int)dmg/d, tag);
	    } 
	}
    }

    // mines
    for (i=0; i<max_mines; i++)
    {
	if (mines[i].alive)
	{
	    u = mines[i].x+3;
	    v = mines[i].y;
	    if (u>=x-40 && u<=x+40 && v>=y-40 && v<=y+40)
		blow_mine(i);
	}
    }

    // players
    for (i=0; i<MAX_PLAYERS; i++)
    {
	if (players[i].health)
	{
	    u = players[i].x+8;
	    v = players[i].y+8;
	    if (u>=x-40 && u<=x+40 && v>=y-40 && v<=y+40) 
	    {
		d = find_distance(u, v, x, y) / 16;
		if (d==0) d=1;
		hurt_player(i, dmg / d, 1, tag, 0);

		angle = find_angle(x, y, u, v); 
		players[i].xv += fixtoi(fcos(angle) * 15) / d;
		players[i].yv += fixtoi(fsin(angle) * 15) / d - 1;

		if (players[i].yv>15)
		    players[i].yv = 15;
		else if (players[i].yv<-15)
		    players[i].yv = -15;

		if (players[i].xv>15)
		    players[i].xv = 15;
		else if (players[i].xv<-15)
		    players[i].xv = -15;

		if (players[i].yv > 0) 
		    players[i].jump = 0;
		else
		    players[i].jump = 1;
	    }
	}
    }
}


//---------------------------------------------------------- tiles -----------

int hurt_tile(int u, int v, int dmg, int tag)
{
    int t = map.tile[v][u];
    if (t==T_BAR || t==T_CRATE || t==T_WOOD)
    {
	map.tiletics[v][u] -= dmg;
	if (map.tiletics[v][u] <= 0)    // dead?
	{
	    map.tile[v][u] = 0;
	    spawn_explo(u*16, v*16, X_EXPLO0, 2);
	    map.tiletics[v][u] = st_tile_respawn * GAME_SPEED;

	    if (t==T_BAR)   // boom!
	    {
		u = u*16+8;
		v = v*16+8;
		blast(u, v, st_barrel_damage, tag);
		snd_3d(WAV_EXPLODEBAR, 150, u, v);
	    }
	}

	return 2;   // ouch!
    }
    else if (t && t!=T_LAD)
	return 1;   // collision

    return 0;   // no collision
}

void respawn_tiles()
{
    int v, u;
    int i;
    for (v=map.h-1; v>=0; v--)
    {
	for (u=map.w-1; u>=0; u--)
	{
	    if (!map.tile[v][u] && map.tiletics[v][u])
	    {
		if (--map.tiletics[v][u]==0)    // time?
		{
		    map.tile[v][u] = map.tileorig[v][u];
		    map.tiletics[v][u] = st_tile_health;

		    // check no idiot's in the way
		    for (i=0; i<MAX_PLAYERS; i++)
		    {
			if (players[i].health &&
			    ((players[i].x/16==u && players[i].y/16==v) ||
			    ((players[i].x+5)/16==u && (players[i].y+15)/16==v)))
			{
			    map.tile[v][u] = 0;
			    map.tiletics[v][u] = st_tile_respawn * GAME_SPEED/2;
			    break;
			}
		    } 
		}
		else if (map.tiletics[v][u]==EXPLO_ANIM*7)
		{
		    // do that bright blue thing
		    spawn_explo(u*16, v*16, RESPAWN0, 7); 
		}
	    }
	}
    }
}

void draw_tiles_an_stuff()
{
    int u, v;

    /* draw tiles & pickups */
    for (v=0; v<14; v++)
	for (u=0; u<21; u++) 
	{
	    if (map.ammo[v+my][u+mx])
		draw_sprite(dbuf, dat[map.ammo[v+my][u+mx]].dat, u*16-offsetx, v*16-offsety);
	    if (map.tile[v+my][u+mx])
		draw_sprite(dbuf, dat[map.tile[v+my][u+mx]].dat, u*16-offsetx, v*16-offsety);
	}
}


//---------------------------------------------------------- players ---------

char respawn_order[] =
{
    /* 24x24 numbers, ranging from 0 to 23 */
    21,18,19,14, 7, 9, 6,22, 3,11,17, 0,23,10, 8, 1, 4,20,13,16, 2,15,12, 5,
    12, 0, 8, 7, 9,10,11,23,21, 3,15,13, 1, 4,20,22,16,17,18, 5,19, 2,14, 6,
    16, 8, 7, 4,12,18,14, 0, 3,21,10, 6,11,20,13, 1,19,17, 9, 2,22,23,15, 5,
    16,18,14,23,17, 2,19,11, 5, 9, 4,22, 7, 6,13, 1,12,20, 3,21, 8,10, 0,15,
     5, 3, 6,17, 0,23,20, 7,15, 4, 1,22,14,10,19, 2, 8,13,18,21,11, 9,16,12,
    21,18,16, 3, 7,12, 4, 2,19,17,10, 5, 0,11,22,14,13,15, 6, 1,23, 8,20, 9,
    12, 3, 4,14, 5,22,11,21,13,16,18,19,23, 0, 2, 6,15,17,20, 9, 1, 7,10, 8,
    16, 7,21,13, 1,10, 9, 6, 2,17, 3,12,19,18, 8,14,11,20,23,15, 0, 5,22, 4,
    19,18, 3,17, 9,15,23,10,16,22,13, 1, 2,11,14,21, 6,12, 8, 0,20, 4, 5, 7,
    18,23,20, 3,10,21,13,14, 6,12, 8,17, 1,15, 9,22,19,11,16, 0, 7, 2, 4, 5,
    22,17, 9, 4, 5,16,14, 6, 8,19,20, 3,13, 7,12,10,18, 2,23,15,11, 1,21, 0,
     4, 2, 7, 9,14,19,17,10,11, 1,22, 6,23, 5,13,20,12, 0,21, 3, 8,15,16,18,
    12,20,10,14,17, 1, 6, 5,13,11, 8,23,15,16,22,19, 9, 2, 4,21, 7,18, 0, 3,
     1, 8, 0,22, 9,23, 3,20,13, 2,19,18,12,16,21,17,11, 7,10, 5,14,15, 6, 4,
     6,18,12, 3,15, 0, 1, 9,22, 7,17,16,21,10,14,11, 2,20, 4,23, 5,19, 8,13,
    22,20, 1,10, 4,11,15, 2, 5,23,17, 6,18, 9,19, 0, 3, 8,14,21, 7,12,13,16,
    14, 8, 1,22, 4, 3,15,23, 5, 7,19, 0,11,20,10,18,21, 6, 2,16, 9,12,17,13,
    17,22, 0, 3,11, 4,10, 7, 8,23, 9,12,14, 2, 5,15,16,20,21,18, 1,13, 6,19,
     2, 9,23,20,10, 5,14,12, 0, 3,22,18,16, 1,11,15, 8, 4,13, 7,21,17, 6,19,
    16, 2, 6,14,23,22,20,17,15, 5,21, 4, 0, 9,12, 7,18,13,19,11, 8,10, 1, 3,
    16,18, 3,20, 8, 9,21,10,23, 6,17,22, 4, 5, 7, 2,15,11, 0,12,19, 1,14,13,
    19,21, 8,13, 4, 3,23,17, 2, 7,14, 5,18, 6, 0,22, 9,16,10,15,12, 1,20,11,
    15,23, 7, 1, 9,14, 6,12,10,20, 4, 3, 5,21, 2, 0, 8,19,22,13,16,11,18,17,
    10,20,19,18, 7,16,11, 0, 2,22,14, 9,15, 1, 8,23,13, 5, 4,12,21, 3, 6,17
};

int next_position;  // next respawn position (0-23)

void respawn_player(int pl)
{
    PLAYER *guy;
    int i;
    for (;;)
    {
	i = respawn_order[next_position++];
	if (next_position>24*24)
	    next_position = 0;
	if (map.start[i].x!=255)
	{
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
	    guy->num_bullets = guy->num_shells = guy->num_rockets = guy->num_arrows = guy->num_mines = 0;
	    for (i=0; i<num_weaps; i++, guy->have[i] = 0);
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

void spawn_players()
{
    int i;
    for (i=0; i<num_players; i++)
	respawn_player(i);
}

void hurt_player(int pl, int dmg, int protect, int tag, int deathseq)
{
    if (protect) {  // armour protection
	players[pl].armour -= dmg/2;
	players[pl].health -= dmg/2;
	if (players[pl].armour < 0) {
	    players[pl].health -= abs(players[pl].armour);
	    players[pl].armour = 0;
	}
    } else {    // otherwise, take the pain
	players[pl].health-=dmg;
    }

    // some nice graphics
    spawn_particles(players[pl].x+3, players[pl].y+6, dmg*10, grad_red);
    if (dmg>=5)
	spawn_blods(players[pl].x-4, players[pl].y, dmg*2);

    // are we dead yet? are we dead yet? are we dead yet?
    if (players[pl].health<=0) 
    {
	if (deathseq==D_IMPALE000)  // up yours!
	{
	    spawn_corpse(players[pl].x+3, players[pl].y+31, players[pl].facing, D_IMPALE000, 20);
	}
	else
	{
	    switch (irnd()%5)
	    {
		case 0: /* arms */
		    spawn_corpse(players[pl].x+3, players[pl].y+15, players[pl].facing, D_ARM0, 12);
		    break;
		case 1: /* drops backpack */
		    spawn_corpse(players[pl].x+3, players[pl].y+15, players[pl].facing, D_BACK0, 10);
		    break;
		case 2: /* head */
		    spawn_corpse(players[pl].x+3, players[pl].y+15, players[pl].facing, D_HEAD0, 13);
		    break;
		case 3: /* legs */
		    spawn_corpse(players[pl].x+3, players[pl].y+15, players[pl].facing, D_LEGS000, 18);
		    break;
		case 4: /* fountain */
		    spawn_corpse(players[pl].x+3, players[pl].y+15, players[pl].facing, D_FOUNTAIN000, 19);
		    break;
	    }
	}

	// drop some ammo
	spawn_backpack(players[pl].x, players[pl].y, players[pl].num_bullets/2,
	    players[pl].num_shells/2, players[pl].num_rockets/2, 
	    players[pl].num_arrows/2, players[pl].num_mines/2);

	players[pl].health = 0;

	// give credit to -->
	if (tag==pl)    // haha, suicide!
	{
	    players[tag].frags--;
	    if (deathseq==D_IMPALE000)
		v_msg(-1, "%s WAS IMPALED", players[pl].name);
	    else
		v_msg(-1, "%s COMMITTED SUICIDE", players[pl].name);
	}
	else            // nice job
	{
	    players[tag].frags++;
	    v_msg(-1, "%s WAS FRAGGED BY %s", players[pl].name, players[tag].name);
	}

	snd_3d(WAV_DEAD1+(irnd()%5), 255, players[pl].x, players[pl].y);  // close enough
    }
    else
    {
	// arrghh
	snd_3d(WAV_ARR+(irnd()%5), 255, players[pl].x, players[pl].y);  // close enough
    }
}

inline int gun_pic(int pl)
{
    switch (players[pl].cur_weap)
    {
	case w_bow: return P_BOW0;
	case w_knife: return P_KNIFE0;
	case w_m16: return P_M160;
	case w_minigun: return P_MINI0;
	case w_pistol: return P_PISTOL0;
	case w_bazooka: return P_ROCKET0;
	case w_shotgun: return P_SHOTGUN0;
	case w_uzi: return P_UZI0;
	//case w_flamer: return P_FLAMER0;
	//case w_bottle: return P_BOT0;
	default: return 0;
    }
}

void draw_players()
{
    int i;
    int gun;
    int u, v, w, h;
    PLAYER *guy;

    for (i=0; i<MAX_PLAYERS; i++)
    {
	guy = &players[i];
	if (!guy->health)
	    continue;

	u = guy->x-px;
	v = guy->y-py;

	gun = gun_pic(i);
	if (guy->facing == left)
	    gun += 4;

	// minigun is weird
	if (guy->cur_weap == w_minigun && guy->fire_frame==4) 
	    gun += 2;
	else
	    gun += guy->fire_frame;

	w = ((BITMAP *)dat[gun].dat)->w / 2;
	h = ((BITMAP *)dat[gun].dat)->h / 2;

	if (guy->facing==right)
	{
	    draw_sprite(dbuf, dat[P_BODY].dat, u, v);
	    draw_sprite(dbuf, dat[P_LEG0+guy->leg_frame].dat, u-1, v);
	    rotate_sprite(dbuf, dat[gun].dat, u+3-w, v+6-h, guy->angle);
	}
	else
	{
	    draw_sprite_h_flip(dbuf, dat[P_BODY].dat, u, v);
	    draw_sprite_h_flip(dbuf, dat[P_LEG0+guy->leg_frame].dat, u-8, v);
	    rotate_sprite(dbuf, dat[gun].dat, u+2-w, v+6-h, guy->angle - itofix(128)); 
	}

	draw_light(L_SPOT, u+3, v+3);
    }
}

void draw_scanner(int haywire)  /* draw for local */
{
    PLAYER *p, *other;
    fixed angle;
    int cx, cy, x1, y1, x2, y2;

    if (haywire && (rnd()%5)==0)
      return; 

    /* who's the lucky bastard with the radar? */
    p = &players[local];
    other = &players[1 - local];   /* not this guy.. well, maybe he has one too */

    cx = p->x+8 - px;
    cy = p->y+8 - py;
    circle(dbuf, cx, cy, 25, 254);
    if (num_players < 2)
      return;

    if (haywire && (rnd()%3)==0)
      angle = (rnd()%256) << 16;
    else
      angle = find_angle(p->x+8, p->y+8, other->x+8, other->y+8);

    x1 = cx + fixtoi(fmul(fcos(angle), 20<<16));
    y1 = cy + fixtoi(fmul(fsin(angle), 20<<16));
    x2 = cx + fixtoi(fmul(fcos(angle), 40<<16));
    y2 = cy + fixtoi(fmul(fsin(angle), 40<<16));
    line(dbuf, x1, y1, x2, y2, 250);
}


//---------------------------------------------------------- mines -----------

void reset_mines(int lower)
{
    int i;
    for (i=0; i<max_mines; i++)
	mines[i].alive = 0;
}

void spawn_mine(int x, int y, int tag)
{
    int i;
    for (i=0; i<max_mines; i++)
    {
	if (!mines[i].alive)
	{
	    mines[i].alive = 1;
	    mines[i].x = x;
	    mines[i].y = y;
	    mines[i].unactive = 30;
	    mines[i].frame = 0;
	    mines[i].anim = MINE_ANIM;
	    mines[i].tag = tag;
	    return;
	}
    }

    max_mines += 10;
    mines = realloc(mines, max_mines);
    if (mines==NULL)
	suicide("Mine overflow");
    reset_mines(max_mines-10);
    spawn_mine(x, y, tag);
    add_msg("ALLOCED EXTRA 10 MINES", local);
}

void update_mines()
{
    int i, t;
    for (i=max_mines-1; i>=0; i--)
    {
	if (mines[i].alive)
	{
	    // drop
	    if (!tile_collide(mines[i].x + 3, mines[i].y+2) ||
		(mines[i].y%16)!=14)
		if (++mines[i].y>map.h*16)
		    mines[i].alive = 0; 

	    // activate
	    if (mines[i].unactive)
		mines[i].unactive--;
	    else
	    {
		if (--mines[i].anim==0 && !mines[i].unactive)
		{
		    mines[i].anim = MINE_ANIM;
		    if (mines[i].frame==0)
		    {
			mines[i].frame = 1;
			// blink blink -create light
			t = tile_collide(mines[i].x+3, mines[i].y);
			if (!t || t==T_LAD)
			    spawn_explo(mines[i].x+3, mines[i].y, 0, 2);
		    }
		    else 
		    {
			mines[i].frame = 0;
		    }
		}
	    }
	}
    }
}

void draw_mines()
{
    int i;
    for (i=max_mines-1; i>=0; i--)
    {
	if (mines[i].alive)
	    draw_sprite(dbuf, dat[J_MINE1+mines[i].frame].dat, mines[i].x-px, mines[i].y-py);
    } 
}

void blow_mine(int i)
{
    mines[i].alive = 0;
    spawn_explo(mines[i].x-2, mines[i].y-6, X_EXPLO0, 2);
    snd_3d(WAV_EXPLODE, 255, mines[i].x, mines[i].y);
    blast(mines[i].x+3, mines[i].y, weaps[w_mine].dmg, mines[i].tag);
}

void touch_mines()
{
    int i, j;
    int x, y, x2, y2;
    for (i=max_mines-1; i>=0; i--)
    {
	if (!mines[i].alive || mines[i].unactive)
	    continue;

	for (j=0; j<MAX_PLAYERS; j++)
	{
	    if (!players[j].health)
		continue;

	    x = players[j].x+3;
	    y = players[j].y+6;
	    x2 = mines[i].x+3;
	    y2 = mines[i].y;

	    // idiot ... boom!
	    if (x>=x2-10 && x<=x2+10 && y>=y2-10 && y<=y2+10)
		blow_mine(i);
	}
    }
}


//---------------------------------------------------------- bullets ---------

void reset_bullets(int lower)
{
    int i;
    for (i=lower; i<max_bullets; i++)
	bullets[i].life = 0;
}

void spawn_bullet(int pl, fixed angle, int c, int bmp)
{
    int i, w;
    const int speed = 8;

    for (i=max_bullets-1; i>=0; i--)
    {
	if (!bullets[i].life)
	{
	    //speed = flame ? 4 : 8;
	    //speed = 8;

	    w = (((BITMAP *)dat[gun_pic(pl)].dat)->w / 4);

	    bullets[i].x = itofix(players[pl].x + 3) + fcos(players[pl].angle) * w;
	    bullets[i].y = itofix(players[pl].y + 4) + fsin(players[pl].angle) * w;
	    bullets[i].xv = fcos(angle) * speed;
	    bullets[i].yv = fsin(angle) * speed;
	    bullets[i].dmg = weaps[players[pl].cur_weap].dmg * (players[pl].bloodlust+1);
	    bullets[i].colour = c;
	    bullets[i].bmp = bmp;
	    bullets[i].tag = pl;
	    bullets[i].angle = angle;
	    bullets[i].life = 1;
	    //bullets[i].flame = flame * 30;
	    return;
	}
    }

    max_bullets += 50;
    bullets = realloc(bullets, max_bullets);
    if (bullets==NULL)
	suicide("Bullet overflow"); 
    reset_bullets(max_bullets-50);
    spawn_bullet(pl, angle, c, bmp);
    add_msg("ALLOCED EXTRA 50 BULLETS", local);
}

void update_bullets()
{
    int u, v, i;
    for (i=max_bullets-1; i>=0; i--)
    {
	if (bullets[i].life)
	{
	    bullets[i].x += bullets[i].xv;
	    bullets[i].y += bullets[i].yv;

	    // knife spins round and round
	    if (bullets[i].bmp==J_KNIFE)
		bullets[i].angle += (bullets[i].xv>0) ? itofix(20) : -itofix(20);

	    // off map
	    if (bullets[i].x < itofix(0)     || bullets[i].y < itofix(0) ||
		bullets[i].x > itofix(map.w*16) || bullets[i].y > itofix(map.h*16))
	    {
		bullets[i].life = 0;
	    }
	    else    // else still on map
	    {
		u = fixtoi(bullets[i].x)/16;
		v = fixtoi(bullets[i].y)/16;

		// hit tile?
		if (hurt_tile(u, v, bullets[i].dmg, bullets[i].tag))
		{
		    bullets[i].life = 0;

		    u = fixtoi(bullets[i].x - bullets[i].xv);
		    v = fixtoi(bullets[i].y - bullets[i].yv);
		    spawn_particles(u, v, 50, grad_orange); 

		    // explosion was caused
		    if (bullets[i].bmp==J_ROCKET || bullets[i].bmp==J_ARROW) 
		    {
			spawn_explo(u - 8, v - 8, X_EXPLO0, 2);
			blast(u+((BITMAP *)dat[bullets[i].bmp].dat)->w/2, v+1,
			      bullets[i].dmg, bullets[i].tag);
			snd_3d(WAV_EXPLODE, 255, u, v);
		    }
		    else    // just a ricochet
		    {
			spawn_explo(u, v, 0, 1);
			if ((irnd()%8)==0)
			    snd_3d(WAV_RIC+(irnd()%2), 255, u, v);
		    }
		}
	    }
	}
    }
}

void draw_bullets()
{
    int i;
    for (i=max_bullets-1; i>=0; i--)
    {
	if (bullets[i].life) 
	{
	    if (bullets[i].bmp)
	    {
		if (bullets[i].bmp==J_ROCKET || bullets[i].bmp==J_ARROW)
		    draw_light(L_EXPLO, fixtoi(bullets[i].x) - px, fixtoi(bullets[i].y) - py);
		rotate_sprite(dbuf, dat[bullets[i].bmp].dat, fixtoi(bullets[i].x) - px, fixtoi(bullets[i].y) - py, bullets[i].angle);
	    }
	    else
		putpixel(dbuf, fixtoi(bullets[i].x) - px, fixtoi(bullets[i].y) - py, bullets[i].colour);
	}
    } 
}

void touch_bullets()
{
    int i, j;
    int x, y;
    for (i=max_bullets-1; i>=0; i--)
    {
	if (bullets[i].life)
	{ 
	    for (j=0; j<MAX_PLAYERS; j++)
	    {
		if (j != bullets[i].tag && players[j].health)
		{
		    x = fixtoi(bullets[i].x);
		    y = fixtoi(bullets[i].y);

		    // got in my way.. you're gonna die
		    if (x>=players[j].x-4 && x<=players[j].x+8 &&
			y>=players[j].y && y<=players[j].y+15)
		    {
			bullets[i].life = 0;
			if (bullets[i].bmp==J_ROCKET || bullets[i].bmp==J_ARROW)
			{
			    // boom!
			    spawn_explo(x, y, X_EXPLO0, 2);
			    blast(x+((BITMAP *)dat[bullets[i].bmp].dat)->w/2, 
				  y+1, bullets[i].dmg, bullets[i].tag);
			    snd_3d(WAV_EXPLODE, 255, x, y);
			}
			else
			{
			    // pain agony ouch
			    hurt_player(j, bullets[i].dmg, 1, bullets[i].tag, 0);
			}
		    }
		}
	    }
	}
    }
}


//---------------------------------------------------------- pickups ----------

void respawn_ammo()
{
    int v, u;
    for (v=map.h-1; v>=0; v--)
    {
	for (u=map.w-1; u>=0; u--)
	{
	    if (!map.ammo[v][u] && map.ammotics[v][u]) 
	    {
		if (--map.ammotics[v][u]<1) 
		{
		    map.ammo[v][u] = map.ammoorig[v][u];
		    map.ammotics[v][u] = ammo_respawn_rate(map.ammo[v][u]);
		} 
		else if (map.ammotics[v][u]==EXPLO_ANIM*7)
		{
		    spawn_explo(u*16, v*16, RESPAWN0, 7);
		}
	    }
	}
    }
}


//---------------------------------------------------------- on-screen stats -

int show_fps = 0;

struct
{
    int pic;
    int weap;
    int shortcut;
    char *s;
} weapon_order[] = 
{
    { W_MINE,       w_mine,     KEY_ENTER,  "a" },  // 'a' actually enter symbol
    { W_KNIFE,      w_knife,    KEY_1,      "1" },
    { W_PISTOL,     w_pistol,   KEY_2,      "2" },
    { W_SHOTGUN,    w_shotgun,  KEY_3,      "3" },
    { W_UZI,        w_uzi,      KEY_4,      "4" },
    { W_M16,        w_m16,      KEY_5,      "5" },
    { W_MINI,       w_minigun,  KEY_6,      "6" },
    { W_BOW,        w_bow,      KEY_7,      "7" },
    { W_ROCKET,     w_bazooka,  KEY_8,      "8" },
    { 0 }
};

typedef struct {
    int frags;
    int no;
} TEMPSORT;

TEMPSORT tempbuf[MAX_PLAYERS];

int frag_sorter(const void *e1, const void *e2)
{
    return ((TEMPSORT *)e2)->frags - ((TEMPSORT *)e1)->frags;
}

void draw_status()
{
    int y, i;
    int ammo;
    char buf[10];

    if (show_fps)
	textprintf(dbuf, dat[MINI].dat, 270, 80, WHITE, "FPS: %2d", last_fps);

    // list scores
    if (key[KEY_TAB])
    {
	textout(dbuf, dat[UNREAL].dat, "FRAG COUNT", 55, 5, -1);

	for (i=0; i<MAX_PLAYERS; i++)
	{
	    tempbuf[i].frags = players[i].frags;
	    tempbuf[i].no = i;
	}

	qsort(tempbuf, MAX_PLAYERS, sizeof(TEMPSORT), frag_sorter);

	y = 30;
	for (i=0; i<MAX_PLAYERS; i++)
	{
	    if (players[tempbuf[i].no].exist) {
		textprintf(dbuf, dat[UNREAL].dat, 55, y, -1, "%3d %s", players[tempbuf[i].no].frags, players[tempbuf[i].no].name);
		y+=20;
	    }
	}
	return; 
    }

    /* dead, don't continue */
    if ((comm != demo) && (players[local].health == 0))
    {
	textout(dbuf, dat[UNREAL].dat, "PRESS SPACE TO RESPAWN", 20, SCREEN_H-30, -1);
	return;
    }

    text_mode(-1);

    i = 0;
    y = SCREEN_H-6-16;
    while (weapon_order[i].pic)
    {
	if (players[local].have[weapon_order[i].weap])
	{
	    draw_sprite(dbuf, dat[weapon_order[i].pic].dat, 
		6 + (players[local].cur_weap==weapon_order[i].weap ? 6 : 0), y);

	    textout(dbuf, dat[MINI].dat, weapon_order[i].s, 0, y+10, RED);

	    ammo = num_ammo(local, weapon_order[i].weap);
	    if (ammo != -1) {
		sprintf(buf, "%d", ammo); 
		textout(dbuf, dat[MINI].dat, buf, 24, y+6, YELLOW);
		if (weapon_order[i].weap == players[local].cur_weap)
		    textout(dbuf, dat[UNREAL].dat, buf, SCREEN_W-48, SCREEN_H-24, -1);
	    }

	    y-=14;
	}

	i++;
    }

    // light amp powerup
    if (players[local].visor_tics)
	draw_sprite(dbuf, dat[A_GOGGLES+(heart_frame>2 ? 1 : heart_frame)].dat, 152, 9);

    // bloodlust powerup
    if (players[local].blood_tics)
	draw_sprite(dbuf, dat[A_BLOODLUST+blood_frame].dat, 152+18, 9);

    // scanner powerup
    if (players[local].scanner_tics)
	draw_sprite(dbuf, dat[A_SCANNER].dat, 152-18, 9);

    // health and armour
    draw_sprite(dbuf, dat[HEART1+(heart_frame>2 ? 1 : heart_frame)].dat, 6, 9);
    draw_sprite(dbuf, dat[A_ARMOUR].dat, 60, 9);
    textprintf(dbuf, dat[UNREAL].dat, 24, 3, -1, "%3d", players[local].health);
    textprintf(dbuf, dat[UNREAL].dat, 78, 3, -1, "%3d", players[local].armour);
    //textprintf(dbuf, dat[UNREAL].dat, 16, 25, -1, "FRAGS: %d", players[local].frags);
}


//----------------------------------------------------------- input ----------

int next_best[] = { w_minigun, w_m16, w_shotgun, w_uzi, w_pistol, 0 };

void auto_weapon(int pl, int new_weapon)
{
    PLAYER *guy = &players[pl];
    int i;

    // theory
    // 1. NEVER select bow or bazooka for you
    // 2. if you get a NEW weapon that is better than your preferred weapon, 
    //      then change to that
    //  2.5. but only if you're not holding the bazooka or bow
    // 3. if you get ammo for your preferred weapon, then select your 
    //      preferred weapon
    // 4. if your preferred weapon runs out of ammo, choose next best

    // if you get a new weapon that is better than your preferred weapon,
    //  change to it

    if (players[pl].have[new_weapon])
	new_weapon = 0;

    players[pl].have[new_weapon] = 1;

    if (new_weapon)
    {
	if (players[pl].cur_weap == w_bow || 
	    players[pl].cur_weap == w_bazooka)
	  return;

	i = 0;
	while (next_best[i])
	{
	    if ((new_weapon==next_best[i] && num_ammo(pl, new_weapon)) ||
		(guy->cur_weap==next_best[i] && num_ammo(pl, guy->cur_weap)))
	    {
		players[pl].cur_weap = players[pl].pref_weap = next_best[i];
		return;
	    }
	    i++;
	}
    }

    // if you have ammo for your pref weapon, then use pref weapon
    if (num_ammo(pl, guy->pref_weap)) {
	guy->cur_weap = guy->pref_weap;
	return;
    }

    // otherwise no ammo, select next best weapon that you 
    //  (a) are in possesion of 
    //  (b) have ammo for
    i = 0;
    while (next_best[i])
    {
	if (players[pl].have[next_best[i]] && 
	    num_ammo(pl, next_best[i])) 
	{
	    players[pl].cur_weap = next_best[i];
	    if (players[pl].pref_weap==w_bazooka || players[pl].pref_weap==w_bow)
		players[pl].pref_weap = players[pl].cur_weap;
	    return;
	}
	i++;
    }

    // nothing, just this bit of shit
    players[pl].cur_weap = w_knife;
}

int next_have_weapon_order(PLAYER *guy, int wo)
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

int prev_have_weapon_order(PLAYER *guy, int wo)
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

void select_weapon()
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
	// got the weapon?
	if (guy->have[weapon_order[i].weap])
	{
	    select = 0;

	    if (mouse_b & 2) 
	      if (weapon_order[i].weap == w_mine)
		select = 1;
	    if (key[weapon_order[i].shortcut]) 
	      select = 1;

	    // want it? 
	    if ((select && weapon_order[i].weap != guy->cur_weap) ||
		(next == 1 && weapon_order[i].weap != w_mine &&
		 weapon_order[prev_have_weapon_order(guy, i)].weap == guy->cur_weap) ||
		(prev == 1 && weapon_order[i].weap != w_mine &&
		 weapon_order[next_have_weapon_order(guy, i)].weap == guy->cur_weap))
	    {
		if (weapon_order[i].weap==w_mine && guy->num_mines>0)
		  guy->drop_mine = 1;
		else
		  guy->select = weapon_order[i].weap;
		return; 
	    }
	}

    } while (weapon_order[i++].pic);
}

void get_local_input()
{
    // pointing gun at?
    players[local].angle = find_angle(players[local].x-px+3, players[local].y-py+4, mouse_x, mouse_y);

    // facing left or right? 
    if (mouse_x+px > players[local].x)
	players[local].facing = right;
    else
	players[local].facing = left;

    players[local].up = key[KEY_UP] | key[KEY_T] | key[KEY_W];
    players[local].down = key[KEY_DOWN] | key[KEY_G] | key[KEY_S];
    players[local].left = key[KEY_LEFT] | key[KEY_F] | key[KEY_A];
    players[local].right = key[KEY_RIGHT] | key[KEY_H] | key[KEY_D];
    players[local].respawn = key[KEY_SPACE] & (!players[local].health);

    players[local].fire = 0;
    players[local].drop_mine = 0;
    players[local].select = 0;

    if (!players[local].next_fire) {
	players[local].fire = mouse_b & 1;
	select_weapon();
    }
}

void update_player(int pl)
{
    //
    //  Be warned: This is huge.
    //

    PLAYER *guy;
    int kx, ky, a, t, x, y;
    int move_legs = 0;

    guy = &players[pl];

    /* respawning */
    if (guy->respawn)
	respawn_player(pl);

    if (!guy->health)
	return;

    /* degrading goggles */
    if (guy->visor_tics)
	--guy->visor_tics;

    /* degrading bloodlust */
    if (guy->blood_tics)
	if (--guy->blood_tics==0)
	    guy->bloodlust = 0;

    /* degrading scanner */
    if (guy->scanner_tics)
      --guy->scanner_tics;

    /* selecting weapon */
    if (guy->select)
    {
	guy->cur_weap = guy->pref_weap = guy->select;
	if (pl==local)
	    snd_local(WAV_SELECT);
    }

    /* picking up stuff ie. pickups */
    kx = (guy->x+3)/16;
    ky = (guy->y+6)/16; 
    a = map.ammo[ky][kx];
    if (a)
    {
	switch (a)
	{
	    case A_ARMOUR:
		if (guy->armour < st_max_armour) {
		    guy->armour += st_armour_give;
		    if (guy->armour > st_max_armour)
			guy->armour = st_max_armour;
		    map.ammo[ky][kx] = 0;
		    snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		} 
		break;

	    case A_ARROW:
		guy->num_arrows += st_arrow_give;
		map.ammo[ky][kx] = 0;
		auto_weapon(pl, 0);
		snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		break;

	    case A_BULLET:
		guy->num_bullets += st_bullet_give;
		map.ammo[ky][kx] = 0;
		auto_weapon(pl, 0);
		snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		break;

	    case A_CHICKEN:
		if (guy->health < st_max_health) {
		    guy->health += st_chicken_heal;
		    if (guy->health > st_max_health)
			guy->health = st_max_health;
		    map.ammo[ky][kx] = 0;
		    add_msg("<BURP>", pl);
		}
		break;

	    case A_COKE:
		if (guy->health < st_max_health) {
		    guy->health += st_coke_heal;
		    if (guy->health > st_max_health)
			guy->health = st_max_health;
		    map.ammo[ky][kx] = 0;
		}
		break;

	    case W_MINE:
		guy->num_mines += st_mine_give;
		guy->have[w_mine] = 1;
		map.ammo[ky][kx] = 0;
		auto_weapon(pl, 0);
		add_msg("GOT YERSELF A MINE", pl);
		snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		break;

	    case A_ROCKET:
		guy->num_rockets += st_rocket_give;
		map.ammo[ky][kx] = 0;
		auto_weapon(pl, 0);
		snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		break;

	    case A_SHELL:
		guy->num_shells += st_shell_give;
		map.ammo[ky][kx] = 0;
		auto_weapon(pl, 0);
		snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		break;

	    case A_GOGGLES:
		guy->visor_tics = GAME_SPEED * st_goggles_timeout;
		map.ammo[ky][kx] = 0;
		add_msg("LIGHT-AMP ENABLED", pl);
		snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		break;

	    case A_BLOODLUST:
		guy->blood_tics = GAME_SPEED * st_bloodlust_timeout;
		guy->bloodlust = 1;
		map.ammo[ky][kx] = 0;
		add_msg("YOU ARE THE SPAWN OF SATAN!", pl);
		add_msg("GO KILL SOMETHING!!", pl);
		snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		snd_local(WAV_LETSPARTY);
		break;

	    case A_SCANNER: 
		guy->scanner_tics = GAME_SPEED * st_scanner_timeout;
		map.ammo[ky][kx] = 0;
		add_msg("DEAD MEAT TRACKER ON", pl);
		snd_3d(WAV_INCOMING, 255, guy->x, guy->y);   /* dodgy! */
		break;

	    case W_BOW:
		guy->have[w_bow]=1;
		guy->num_arrows+=st_bow_give;
		map.ammo[ky][kx] = 0;
		add_msg("DYNAMITE ON STICKS!", pl);
		snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		break;

	    case W_M16:
		guy->num_bullets+=st_m16_give;
		map.ammo[ky][kx] = 0;
		auto_weapon(pl, w_m16);
		add_msg("M16", pl);
		snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		break;

	    case W_MINI:
		guy->num_bullets+=st_minigun_give;
		map.ammo[ky][kx] = 0;
		auto_weapon(pl, w_minigun);
		add_msg("MINIGUN! FIND SOME MEAT!", pl);
		snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		break;

	    case W_PISTOL:
		guy->num_bullets+=st_pistol_give;
		map.ammo[ky][kx] = 0;
		auto_weapon(pl, w_pistol);
		snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		break;

	    case W_ROCKET:
		guy->have[w_bazooka]=1;
		guy->num_rockets+=st_bazooka_give;
		map.ammo[ky][kx] = 0;
		add_msg("BAARRZOOKA!", pl);
		snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		break;

	    case W_SHOTGUN:
		guy->num_shells+=st_shell_give;
		map.ammo[ky][kx] = 0;
		auto_weapon(pl, w_shotgun);
		snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		break;

	    case W_UZI:
		guy->num_bullets+=st_uzi_give;
		map.ammo[ky][kx] = 0;
		auto_weapon(pl, w_uzi);
		snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		break;
	}
    }

    /* firing */
    if (guy->next_fire)
	--guy->next_fire;

    if (guy->fire)
    {
	int fired = 1;

	switch (guy->cur_weap)
	{
	    case w_minigun:
		if (--guy->num_bullets < 0) {
		    fired = guy->num_bullets = 0;
		    auto_weapon(pl, 0);
		} else {
		    spawn_bullet(pl, guy->angle, GREY, 0);
		    snd_3d(WAV_MINIGUN, 180, guy->x, guy->y);
		}
		break;

	    case w_m16:
		if (--guy->num_bullets < 0) {
		    fired = guy->num_bullets = 0;
		    auto_weapon(pl, 0);
		} else {
		    spawn_bullet(pl, guy->angle, GREY, 0);
		    snd_3d(WAV_M16, 255, guy->x, guy->y);
		}
		break;

	    case w_uzi:
		if (--guy->num_bullets < 0) {
		    fired = guy->num_bullets = 0;
		    auto_weapon(pl, 0);
		} else {
		    spawn_bullet(pl, guy->angle, GREY, 0);
		    snd_3d(WAV_UZI, 255, guy->x, guy->y);
		}
		break;

	    case w_pistol:
		if (--guy->num_bullets < 0) {
		    fired = guy->num_bullets = 0;
		    auto_weapon(pl, 0);
		} else {
		    spawn_bullet(pl, guy->angle, GREY, 0);
		    snd_3d(WAV_UZI, 255, guy->x, guy->y);
		}
		break;

	    case w_shotgun:
		if (--guy->num_shells < 0) {
		    fired = guy->num_shells = 0;
		    auto_weapon(pl, 0);
		}
		else
		{
		    spawn_bullet(pl, guy->angle - itofix(5), YELLOW, 0);
		    spawn_bullet(pl, guy->angle - itofix(3), YELLOW, 0);
		    spawn_bullet(pl, guy->angle,             YELLOW, 0);
		    spawn_bullet(pl, guy->angle + itofix(2), YELLOW, 0);
		    spawn_bullet(pl, guy->angle + itofix(4), YELLOW, 0);
		    snd_3d(WAV_SHOTGUN, 255, guy->x, guy->y);
		}
		break;

	    case w_bazooka:
		if (--guy->num_rockets < 0) {
		    fired = guy->num_rockets = 0;
		    auto_weapon(pl, 0);
		}
		else 
		{
		    spawn_bullet(pl, guy->angle, 0, J_ROCKET);
		    snd_3d(WAV_ROCKET, 255, guy->x, guy->y);
		}
		break;

	    case w_bow:
		if (--guy->num_arrows < 0) {
		    fired = guy->num_arrows = 0;
		    auto_weapon(pl, 0);
		}
		else
		{
		    spawn_bullet(pl, guy->angle, 0, J_ARROW);
		    snd_3d(WAV_BOW, 255, guy->x, guy->y);
		} 
		break;

	    case w_knife:
		spawn_bullet(pl, guy->angle, 0, J_KNIFE);
		snd_3d(WAV_KNIFE, 255, guy->x, guy->y);
		break;
	}

	if (fired) 
	    guy->next_fire = weaps[guy->cur_weap].reload;
	else {
	    guy->fire_frame = 0;
	    guy->firing = 0;
	}

	if (!guy->firing && fired)
	{
	    guy->firing = 1;
	    guy->fire_anim = 0;
	}
    }
    else 
    {
	guy->firing= 0;
    }

    /* dropping mines */
    if (guy->drop_mine)
    {
	spawn_mine(guy->x, guy->y+12, pl);
	guy->next_fire = weaps[w_mine].reload;
	if (--guy->num_mines<1)
	    guy->have[w_mine] = 0;
	snd_3d(WAV_MINE, 255, guy->x, guy->y);
    }

    /* impaled on spikes ? */
    if (tile_collide(guy->x+3, guy->y+16)==T_SPIKE)
	hurt_player(pl, 1, 0, pl, D_IMPALE000); // armour doesn't affect spikes

    /* climb down ladder */
    if (guy->down)
    {
	if (tile_collide(guy->x+3, guy->y+17)==T_LAD || 
	    (tile_collide(guy->x+3, guy->y+16)==T_LAD && tile_collide(guy->x+3, guy->y+17)==0)) 
	{
	    guy->y+=2;
	    move_legs = 1;
	}
    }

    /* standing or falling */
    if (!guy->jump) 
    {
	// nothing under feet?
	if (!tile_collide(guy->x+3, guy->y+16)) 
	{
	    guy->y += guy->yv;  // fall!

	    if (guy->y > map.h * 16)
	    {
		hurt_player(pl, players[pl].health, 0, pl, 0);
		return;
	    }

	    // hit solid ground?
	    if (tile_collide(guy->x+3, guy->y+16))
	    {
		guy->y = ((int)guy->y/16) * 16;
		guy->yv = 0;
	    }
	    else if (guy->yv<6) // nope... increase gravitational pull
		guy->yv++;
	}
	else if (guy->up)       // jump or climb ladder
	{
	    // climb
	    if (!guy->left && !guy->right && 
		tile_collide(guy->x+3, guy->y+15)==T_LAD)
	    {
		t = tile_collide(guy->x+3, guy->y-1);
		if (!t || t==T_LAD)
		{
		    guy->y-=2;
		    move_legs = 1;
		}
	    }
	    else    // jump
	    {
		guy->jump = 1;
		guy->yv = -11;
	    }
	}
    }
    else    /* in process of jumping */
    {
	guy->y += guy->yv;
	guy->yv++;

	t = tile_collide(guy->x+3, guy->y);
	if (t && t!=T_LAD)
	{
	    guy->y = ((int)(guy->y+16)/16) * 16;
	    guy->jump = guy->yv = 0;
	} 

	if (guy->yv==0)
	    guy->jump = guy->yv = 0; 
    }

    // when getting blown back or something
    if (guy->xv)
    {
	guy->x += guy->xv;

	t = tile_collide(guy->x+3, guy->y);
	if (t && t!=T_LAD)
	{
	    guy->x = ((int)(guy->x+3-guy->xv)/16) * 16;
	    guy->xv = 0;
	}

	if (guy->xv < 0)
	    guy->xv++;
	if (guy->xv > 0)
	    guy->xv--;
    }

    /* left */
    if (guy->left)
    {
	t = tile_collide(guy->x, guy->y+15);
	if (!t || t==T_LAD)
	    guy->x-=3;
	move_legs = 1;
    } 

    /* right */
    if (guy->right)
    {
	t = tile_collide(guy->x+7, guy->y+15);
	if (!t || t==T_LAD)
	    guy->x+=3; 
	move_legs = 1;
    }

    /* squirting blood */
    if (guy->health <= 20)
	spawn_particles(guy->x+3, guy->y+6, 1, grad_red);

    /* anims */
    if (move_legs)
    {
	if (--guy->leg_tics<0) {
	    guy->leg_tics=LEG_ANIM;
	    if (++guy->leg_frame>3)
		guy->leg_frame=0;
	}
    }

    if (guy->firing || guy->fire_frame)
    {
	if (--guy->fire_anim<=0)
	{
	    ++guy->fire_frame;
	    if (guy->fire_frame>4 || (guy->cur_weap!=w_minigun && guy->fire_frame>3))
	    {
		guy->fire_frame = 0;
		if (guy->firing && (guy->cur_weap==w_m16 || guy->cur_weap==w_uzi || guy->cur_weap==w_minigun))
		    guy->fire_frame = 1;
	    }

	    // eject casings
	    if ((guy->cur_weap==w_shotgun && guy->fire_frame==3) || 
		(guy->cur_weap==w_m16 && guy->fire_frame==3) ||
		(guy->cur_weap==w_pistol && guy->fire_frame==2) ||
		(guy->cur_weap==w_uzi && guy->fire_frame==3) ||
		(guy->cur_weap==w_minigun && guy->fire_frame==2))
	    { 
		x = guy->x + 3 + fixtoi(fcos(guy->angle)) * 5;
		y = guy->y + 3 + fixtoi(fsin(guy->angle)) * 5;
		spawn_casing(x, y, guy->facing);
	    } 

	    guy->fire_anim = weaps[guy->cur_weap].anim;
	}
    }
}

void clean_player(int pl)
{
    int i;

    // removes player, and all mines and bullets he placed
    // ... cleaned

    players[pl].health = players[pl].exist = players[pl].frags = 0;

    for (i=max_bullets-1; i>=0; i--)
    {
	if (bullets[i].tag==pl)
	    bullets[i].life = 0;
    }

    for (i=max_mines-1; i>=0; i--)
    {
	if (mines[i].tag==pl)
	    mines[i].alive = 0;
    }
}


//---------------------------------------------------------- serial ----------

// this stuff is peer-peer

#define SER_PLAYERSTAT  's'
#define SER_QUITGAME    'x'
#define SER_QUITOK      'X'

#define SER_UP          0x01    // bit flags
#define SER_DOWN        0x02
#define SER_LEFT        0x04
#define SER_RIGHT       0x08
#define SER_FIRE        0x10
#define SER_DROPMINE    0x20
#define SER_RESPAWN     0x40
#define SER_FACINGLEFT  0x80

char packet[20];

int end_game = 0;

void inline send_long(long val)
{
    skSend(val & 0xff);
    skSend((val>>8) & 0xff);
    skSend((val>>16) & 0xff);
    skSend((val>>24) & 0xff);
}

long inline recv_long()
{
    while (skReady() < 4);
    return skRecv() | (skRecv()<<8) | (skRecv()<<16) | (skRecv()<<24);
}

void send_local_input()
{
    if (num_players > 1 || demo_is_recording())
    {
	// build the packet
	packet[0] = SER_PLAYERSTAT;
	packet[1] = local;

	packet[2] = 0;
	if (players[local].up)          packet[2] |= SER_UP;
	if (players[local].down)        packet[2] |= SER_DOWN;
	if (players[local].left)        packet[2] |= SER_LEFT;
	if (players[local].right)       packet[2] |= SER_RIGHT;
	if (players[local].fire)        packet[2] |= SER_FIRE;
	if (players[local].drop_mine)   packet[2] |= SER_DROPMINE;
	if (players[local].respawn)     packet[2] |= SER_RESPAWN;
	if (players[local].facing==left)packet[2] |= SER_FACINGLEFT;    // otherwise facing right

	packet[3] = players[local].select;
	packet[4] = (players[local].angle>>16) & 0xff;

	if (comm == serial)
	    skWrite(packet, 5);
	
	demo_write_frame_data(packet + 1, 4);
    }
};

void recv_remote_inputs()
{
    int pl, ch;

    if (num_players>1)
    {
	for (;;)
	{
	    while (!skReady());
	    switch (skRecv())
	    {
		case SER_PLAYERSTAT:
		    while (skReady()<4);
		    skRead(packet, 4);

		    pl = packet[0];

		    ch = packet[1];
		    players[pl].up          = ch & SER_UP;
		    players[pl].down        = ch & SER_DOWN;
		    players[pl].left        = ch & SER_LEFT;
		    players[pl].right       = ch & SER_RIGHT;
		    players[pl].fire        = ch & SER_FIRE;
		    players[pl].drop_mine   = ch & SER_DROPMINE;
		    players[pl].respawn     = ch & SER_RESPAWN;
		    if (ch & SER_FACINGLEFT) players[pl].facing = left;
		    else players[pl].facing = right;

		    players[pl].select = packet[2];
		    players[pl].angle = packet[3] << 16;

		    demo_write_frame_data(packet, 4);
		    return;

		case SER_QUITGAME:
		    skSend(SER_QUITOK);
		    end_game = 1;
		    return;

		default:
		    add_msg("SYNC PROBLEM !", -1);
		    break;
	    }
	}
    }
}


//---------------------------------------------------------- demo ------------

int load_map_wrapper(char *fn);

void recv_demo_inputs()
{
    int pl, ch, x;
    int num = num_players;
    
    while (num) {
	x = demo_read_next_packet_type();

	if (x == DEMO_FRAME_DATA) {
	    demo_read_packet(packet, 4);
	    
	    pl = packet[0];

	    ch = packet[1];
	    players[pl].up          = ch & SER_UP;
	    players[pl].down        = ch & SER_DOWN;
	    players[pl].left        = ch & SER_LEFT;
	    players[pl].right       = ch & SER_RIGHT;
	    players[pl].fire        = ch & SER_FIRE;
	    players[pl].drop_mine   = ch & SER_DROPMINE;
	    players[pl].respawn     = ch & SER_RESPAWN;
	    if (ch & SER_FACINGLEFT) players[pl].facing = left;
	    else players[pl].facing = right;
	
	    players[pl].select = packet[2];
	    players[pl].angle = packet[3] << 16;
	    
	    num--;
	}
	else if (x == DEMO_MAP_CHANGE) {
	    char filename[1024], *f;
	    
	    demo_read_string(filename, sizeof filename);
	    if (load_map_wrapper(filename) < 0) {
		alert("Error loading map", filename, "", "Damn", NULL, 13, 27);
		end_game = 1;
		break;
	    }
	    
	    retain_players();
	    no_germs(); 
	    restore_players();
	    spawn_players();
	    
	    f = strstr(filename, ".WAK");
	    if (f) *f = 0;
	    introduce_map(filename);
	    
	    speed_counter = 0;
	}
	else if (x == DEMO_END) {
	    end_game = 1;
	    break;
	}
    }
}


//---------------------------------------------------------- mother loop -----

int shakex = 0, shakey = 0;

void draw_spotlight()
{
    // help draw the spotlights
    int u = 0, v = 0;

    if ((players[local].fire_frame || players[local].firing) && 
	players[local].cur_weap != w_knife && 
	players[local].cur_weap != w_bow && 
	players[local].health) 
    {
       u = (rnd()%10) - 5;
       v = (rnd()%10) - 5;
       blit(light, light, 0, 0, u, v, SCREEN_W, SCREEN_H);
    }

    color_map = &light_map;
    draw_trans_sprite(dbuf, light, 0, 0);
}

void calc()
{
    /* find top-left */
    mx = players[local].x / 16 - 10;
    my = players[local].y / 16 - 6;

    offsetx = players[local].x % 16;
    offsety = players[local].y % 16;

    if (mx<0) mx = offsetx = 0;
    if (my<0) my = offsety = 0;
    if (mx>=map.w-20) { mx=map.w-20; offsetx = 0; }
    if (my>map.h-13 || (my==map.h-13 && offsety>=8)) { my=map.h-13; offsety = 8; }

    px = mx * 16 + offsetx;
    py = my * 16 + offsety;
}

void render()
{
    static int bx, by;

    calc();

    /* tile backdrop */
    bx = -(px%640)/2;
    by = -(py%400)/2;

    blit(dat[BACKDROP].dat, dbuf, 0, 0, bx,     by,     320, 200);
    blit(dat[BACKDROP].dat, dbuf, 0, 0, bx,     by+200, 320, 200);
    blit(dat[BACKDROP].dat, dbuf, 0, 0, bx+320, by,     320, 200);
    blit(dat[BACKDROP].dat, dbuf, 0, 0, bx+320, by+200, 320, 200);

    // ready light
    clear_to_color(light, AMBIENT_LIGHT);
    color_map = &alpha_map;

    // now draw, boy!
    draw_mines();
    draw_tiles_an_stuff();
    draw_backpacks();
    draw_bullets();
    draw_players();
    draw_particles();
    draw_blods();
    draw_explo();
    draw_corpses();

    if (!players[local].visor_tics ||
	(players[local].visor_tics < GAME_SPEED*3 && (players[local].visor_tics%2)==0))
	draw_spotlight();

    if (players[local].scanner_tics) {
	/* in the last five seconds it screws up */
	draw_scanner((players[local].scanner_tics < GAME_SPEED * 5));
    }

    draw_status(); 
    draw_msgs();

    show_mouse(NULL);
    blit(dbuf, screen, 0, 0, 0 + shakex, 0 + shakey, SCREEN_W, SCREEN_H);
    if (players[local].health && (comm != demo))
	show_mouse(screen);

    frame_counter++;

    //-- screen shots --
#if 1				   /* dodgy web design stuff */
    {
	static char ss_name[80];
	static int ss_num = 0;

	if (key[KEY_PAUSE])
	{
	    sprintf(ss_name, "screen%02d.pcx", ss_num++);
	    save_pcx(ss_name, dbuf, dat[GAMEPAL].dat);
	}
    }
#endif
}

void game_loop()
{
    int frames_dropped;
    int update;
    int i;

    speed_counter = 0;

    end_game = 0;

    while (!end_game)
    {
	frames_dropped = 0;
	update = 0;
	while (speed_counter > 0 && frames_dropped < 6) 
	{
	    update = 1;

	    calc();
	    if (comm != demo) {
		get_local_input();
		send_local_input();
	    }
	    
	    if (key[KEY_ESC]) goto quit_pls;

	    if (comm == serial) {
		while (!skReady()) {
		    if (frames_dropped>1) {
			frames_dropped--;
			render();
		    }

		    if (key[KEY_ESC]) goto quit_pls;
		}

		recv_remote_inputs();
	    }
	    else if (comm == demo) {
		recv_demo_inputs();
		
		if (key[KEY_S]) {
		    rest(10);
		    speed_counter = 0;
		}
		
		if (key[KEY_ENTER] && num_players == 2) {
		    char tmp[128];
		    
		    local = 1 - local;
		    sprintf(tmp, "< NOW WATCHING %s >", players[local].name);
		    add_msg(tmp, -1);

		    key[KEY_ENTER] = 0;   /* bleh */
		}
	    }

	    if (end_game) 
		goto quit_pls;

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

	    for (i=0; i<MAX_PLAYERS; i++)
		if (players[i].exist)
		    update_player(i);

	    touch_mines();
	    touch_bullets();
	    touch_backpacks();

	    if (shake_factor>40) 
		shake_factor = 40;

	    if (shake_factor)
	    {
		shake_factor--;
		shakex = (rnd()%(shake_factor*2+1))-shake_factor;
		shakey = (rnd()%(shake_factor*2+1))-shake_factor;
	    }

	    if (--heart_anim<0)
	    {
		heart_anim = HEART_ANIM;
		if (++heart_frame>3)
		    heart_frame=0;
	    }

	    if (--blood_anim<0)
	    {
		blood_anim = HEART_ANIM;    // just borrow this
		if (++blood_frame>5)
		    blood_frame =0;
	    }

	    if (key[KEY_F12])
		show_fps = (show_fps ? 0 : 1);

	    speed_counter--;
	    frames_dropped++;
	}

	if (update)
	    render();

	if (key[KEY_ESC])
	{ 
	//-------------//
	    quit_pls:
	//-------------//
	    end_game = 1;
	    if (comm==serial)
	    {
		skSend(SER_QUITGAME);
		while (skRecv() != SER_QUITOK) rest(10);
	    }
	    break;
	}
    }


    // just in case
    while (key[KEY_ESC] && mouse_b);
    clear_keybuf();
}


//---------------------------------------------------------- soap ------------

void no_germs()
{
    // clean everything
    reset_bullets(0);
    reset_mines(0);
    reset_backpacks(0);
    memset(players, 0, MAX_PLAYERS * sizeof(PLAYER));
    memset(particles, 0, MAX_PARTICLES * sizeof(PARTICLE));
    memset(explos, 0, MAX_EXPLOSIONS * sizeof(EXPLOSION));
    memset(blods, 0, MAX_BLODS * sizeof(BLOD));
    memset(corpses, 0, MAX_CORPSES * sizeof(CORPSE));
    oldest_corpse = 0;
    num_msgs = 0;
}

char retain_names[MAX_PLAYERS][40];
int retain_frags[MAX_PLAYERS];

void retain_players()
{
    // save player names and frags
    int i;
    for (i=0; i<MAX_PLAYERS; i++)
    {
	strcpy(retain_names[i], players[i].name);
	retain_frags[i] = players[i].frags;
    }
}

void restore_players()
{
    // restore player names and frags
    int i;
    for (i=0; i<MAX_PLAYERS; i++)
    {
	strcpy(players[i].name, retain_names[i]);
	players[i].frags = retain_frags[i];
    }
}

//---------------------------------------------------------- main ------------

int mapper();   // mapper.c
void intro();   // intro.c
void menu();    // menu.c
void set_menu_message(char *msg);
void credits(); // creds.c

int com_port = COM2;

char game_path[1024], *game_path_p;


void usage(char *options)
{
    /* :) */
    allegro_message("See manual for help on these options: %s\n", options);
}


void show_version(int legal)
{
    allegro_message("Red Pixel " VERSION_STR " by Psyk Software " VERSION_YEAR ".\n"
		    "http://www.psynet.net/tjaden/\n%s",
		    (legal 
		     ? ("Licensed under the GNU Lesser General Public Licence.\n"
			"See COPYING for details.\n")
		     : ""));
}


int main(int argc, char *argv[])
{
    int skip_intro = 0;
    int lcd_cur = 0;
    int alt_stats = 0;
    int x, y;

    build_sqrt_table(); 

    /* init allegro */
    allegro_init();
    
    if (install_mouse() <= 0) {
#ifdef TARGET_DJGPP
	allegro_message("\nRed Pixel requires a mouse to play.\n"
			"Please install a mouse driver (e.g. mouse.com).\n\n");
#else
	allegro_message("\nRed Pixel requires a mouse to play.\n"
			"Please check your Allegro mouse port settings\n"
			"and make sure you have permissions to the device.\n\n");
#endif
	return 1;
    }
    
    install_timer();
    install_keyboard();
    install_sound(DIGI_AUTODETECT, MIDI_NONE, NULL);

    /* install timer interrupts */
    LOCK_VARIABLE(speed_counter);
    LOCK_FUNCTION(speed_timer);
    LOCK_VARIABLE(last_fps);
    LOCK_VARIABLE(frame_counter);
    install_int_ex(speed_timer, BPS_TO_TIMER(GAME_SPEED));
    install_int_ex(fps_timer, BPS_TO_TIMER(1));

    /* set up game path */
    strcpy(game_path, argv[0]);
    game_path_p = get_filename(game_path);

    /* load datafile */
    strcpy(game_path_p, "blood.dat");
    dat = load_datafile(game_path);
    if (!dat)
    {
	allegro_message("Error loading blood.dat\n");
	return 1;
    }
    
    /* Command line args.  */
    {
	int c;
	
	while (1) {
	    char *options = "hveqld1234s:";
	    
	    c = getopt(argc, argv, options);
	    if (c < 0) break;
	    
	    switch (c) {
		case 'h': usage(options); return 0;
		case 'v': show_version(1); return 0;
		case 'e': return mapper();
		case 'q': skip_intro = 1; break;
		case 'l': lcd_cur = 1; break;
		case 'd': record_demos = 1; break;
		case '1': com_port = COM1; break;
		case '2': com_port = COM2; break;
		case '3': com_port = COM3; break;
		case '4': com_port = COM4; break;
		case 's': 
		    if (!optarg) {
			allegro_message("No stats file specified!\n");
			return 1;
		    }
		    else if (!read_stats(optarg, stat_block)) {
			allegro_message("Error reading %s.\n", optarg);
			return 1;
		    }
		    
		    set_menu_message(get_filename(optarg));
		    alt_stats = 1;
	 	    break;
		
		case '?':
		default:
		    return 1;
	    }
	}
    }

    /* stats, if none read yet */
    if (!alt_stats) {
	strcpy(game_path_p, "stats");
	if (!read_stats(game_path, stat_block)) {
	    allegro_message("Error reading %s.\n", game_path);
	    return 1;
	}
    }

    set_weapon_stats();

    /* alloc */
    bullets = malloc(max_bullets * sizeof(PARTICLE));
    mines = malloc(max_mines * sizeof(MINE));
    backpacks = malloc(max_backpacks * sizeof(BACKPACK));
    if (!bullets || !mines || !backpacks)
	suicide("Bullets or mines or backpacks malloc failed");

    /* colour / lighting / translucency tables */
    create_rgb_table(&rgb_table, dat[GAMEPAL].dat, NULL);
    rgb_map = &rgb_table;

    create_light_table(&light_map, dat[GAMEPAL].dat, 0, 0, 0, NULL);

    for (x = 0; x < 256; x++)
	for (y = 0; y < 256; y++)
	    alpha_map.data[x][y] = MIN(x+y, 255);

    /* init screen etc */
    if (set_gfx_mode(GFX_AUTODETECT, 320, 200, 0, 0) < 0)
	suicide("Error setting 320x200 video mode.");
    set_window_title("Red Pixel");
    set_palette(dat[GAMEPAL].dat);
    dbuf = create_bitmap(SCREEN_W, SCREEN_H);
    light = create_bitmap(SCREEN_W, SCREEN_H);
    if (lcd_cur)
	set_mouse_sprite(dat[XHAIRLCD].dat);
    else
	set_mouse_sprite(dat[XHAIR].dat);
    set_mouse_sprite_focus(2, 2);
    set_mouse_speed(1, 1);

    if (!skip_intro)
	intro();

    menu();

    /* bye bye */
    killall();

    show_version(0);

    return 0;
}

END_OF_MAIN()