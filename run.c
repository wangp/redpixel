/*  7 june 1998 
    8 june 1998 
    Coding: Tjaden 
    Graphics: Air
 */


#include <stdio.h>
#include <time.h>
#include "common.h"


#define MAX_PLAYERS 8

#define MINE_TICS   10
#define EXPLO_TICS  5
#define LEG_TICS    1



DATAFILE *dat;
BITMAP *dbuf;
BITMAP *xbuf;


int crap_comp = 0;



RGB_MAP rgb_table;
COLOR_MAP light_table;

BITMAP *spotlight;
BITMAP *spotlight2;



int mx, my, offsetx, offsety, px, py;



#define GAME_SPEED  40


volatile int speed_counter = 0;

void increment_speed_counter()
{
    speed_counter++;
}

END_OF_FUNCTION(increment_speed_counter);



typedef struct
{
    fixed x, y;
    fixed xv, yv;
    unsigned char colour;
    unsigned char life;
    unsigned char alive;
    unsigned char tag, dmg;
    int bmp;
    fixed angle; 
} PARTICLE;

#define MAX_BULLETS     300
#define MAX_PARTICLES   500

PARTICLE bullets[MAX_BULLETS];
PARTICLE particles[MAX_PARTICLES];



typedef struct
{
    fixed x, y;
    int pic;
    char cur, frames, tics;
    char alive;
} EXPLOSION;

#define MAX_EXPLOSIONS  100

EXPLOSION explo[MAX_EXPLOSIONS];



typedef struct
{
    int x, y;
    char frame, tics;
    char unactive;
    char alive;
} MINE;

#define MAX_MINES   50

MINE mine[MAX_MINES];



enum {
    w_knife = 0,
    w_pistol,
    w_bow,
    w_shotgun,
    w_uzi,
    w_m16,
    w_minigun, 
    w_bazooka,
    w_mine,
    num_weaps
} w_types;

struct
{
    int anim;
    int reload;
    int dmg;
} weaps[] =
{
    { 3, 20, 10 },  // knife
    { 4, 20, 13 },  // pistol
    { 4, 28, 30 },  // bow
    { 6, 31, 5 }, // shotty 5xshells per shot
    { 3, 8, 10 },  // uzi
    { 2, 5, 12 },  // m16
    { 1, 1, 5 },  // mgun
    { 6, 50, 60 },  // zooka
    { 0, 50, 40 }  // mine
};



enum {
    left,
    right
};

typedef struct
{
    char alive;

    int x, y;
    char jump, yv;
    int health;
    int armour;
    fixed angle;
    char facing;

    char leg_frame, leg_tics;

    char firing, fire_frame, fire_tics, next_fire;

    int num_bullets;
    int num_shells;
    int num_rockets;
    int num_mines;
    int num_arrows;

    unsigned char up, down, left, right, fire, drop_mine;
    unsigned char select[num_weaps];

    unsigned char have[num_weaps];
    unsigned char cur_weap;
} PLAYER;

PLAYER player[MAX_PLAYERS];

int local;
int shake_factor;



/* just in case */

void killall()
{
    unload_datafile(dat);
    destroy_bitmap(spotlight);
    destroy_bitmap(spotlight2);
    destroy_bitmap(dbuf);
    destroy_bitmap(xbuf);
    allegro_exit();
}

void suicide(char *s)
{
    killall();
    printf("suicide(): %s\n", s);
    exit(666);
}



/* misc */

int inline tile_collide(int x, int y) 
{
    return map.tile[y/16][x/16];

}

fixed find_angle(int x1, int y1, int x2, int y2)
{
    int adj, opp;
    fixed angle;
    adj = x2 - x1;
    opp = y1 - y2;
    if (adj==0)
	adj=1;
    angle = fatan(ftofix((float)opp/(float)adj));
    if (adj<0)
	angle += itofix(128);
    return -angle;
}



/* particles */

enum
{
    grad_red,
    grad_orange
};

void spawn_particles(int x, int y, int num, int grad)
{
    int i, hi = 0, low=0, spd;
    fixed angle;

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
	if (!particles[i].alive)
	{ 
	    spd = (random()%3)+1;
	    particles[i].x = itofix(x);
	    particles[i].y = itofix(y);
	    angle = itofix(random()%256);
	    particles[i].xv = fcos(angle) * spd;
	    particles[i].yv = fsin(angle) * spd;
	    particles[i].life = random()%20;
	    particles[i].colour = (random()%(hi-low)) + low;
	    particles[i].alive = 1;
	    if (--num==0)
		return;
	}
    } 

    //this is very bad, but we won't let it crash us
    //suicide("Particle overflow");
}

void move_particles()
{
    int i, t;
    for (i=0; i<MAX_PARTICLES; i++)
    {
	if (particles[i].alive)
	{
	    particles[i].x += particles[i].xv;
	    particles[i].y += (particles[i].yv += ftofix(.15));

	    t = map.tile[fixtoi(particles[i].y)/16][fixtoi(particles[i].x)/16];
	    if (t && t!=T_LAD) {
		particles[i].xv *= -.5;
		particles[i].yv *= -.5; 
		if (abs(particles[i].xv) < ftofix(.2) && abs(particles[i].yv) < ftofix(.2))
		    particles[i].alive = 0;
	    }

	    if (--particles[i].life<1)
		particles[i].alive = 0;
	}
    }
}

void draw_particles()
{
    int i;
    for (i=0; i<MAX_PARTICLES; i++)
    {
	if (particles[i].alive)
	    putpixel(dbuf, fixtoi(particles[i].x) - px, fixtoi(particles[i].y) - py, particles[i].colour);
    } 
}



/* explosions */

void spawn_explo(int x, int y, int pic, int frames)
{
    int i;
    EXPLOSION *ex;
    for (i=0; i<MAX_EXPLOSIONS; i++)
    {
	if (!explo[i].alive)
	{
	    ex = &explo[i]; 
	    ex->alive = 1;
	    ex->x = itofix(x);
	    ex->y = itofix(y);
	    ex->pic = pic;
	    ex->cur = 0;
	    ex->frames = frames;
	    ex->tics = EXPLO_TICS;
	    return;
	}
    }

    //this is very bad, but we won't let it crash us
    //suicide("Explosion overflow");
}

void update_explo()
{
    int i;
    for (i=0; i<MAX_EXPLOSIONS; i++)
    {
	if (explo[i].alive)
	{
	    if (--explo[i].tics < 0) {
		explo[i].tics = EXPLO_TICS;
		if (++explo[i].cur >= explo[i].frames) {
		    explo[i].alive = 0;
		    return;
		}
	    }
	}
    }
}

void draw_explo()
{
    int i;
    for (i=0; i<MAX_EXPLOSIONS; i++)
    {
	if (explo[i].alive)
	    draw_sprite(dbuf, dat[explo[i].pic+explo[i].cur].dat, fixtoi(explo[i].x)-px, fixtoi(explo[i].y)-py);
    }
}



/* player */

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

int next_position;

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
	    guy = &player[pl];
	    guy->alive = 1;
	    guy->x = map.start[i].x * 16;
	    guy->y = map.start[i].y * 16;
	    guy->yv = guy->jump = 0;
	    guy->health = 100;
	    guy->armour = 0;
	    guy->angle = itofix(0);
	    guy->facing = right;
	    guy->leg_tics = guy->leg_frame = 0;
	    guy->fire_frame = guy->fire_tics = guy->firing = guy->next_fire = 0;
	    guy->num_bullets = guy->num_shells = guy->num_rockets = guy->num_arrows = guy->num_mines = 0;
	    guy->cur_weap = w_knife;
	    for (i=0; i<num_weaps; i++, guy->have[i] = 0);
	    guy->have[w_knife] = 1; 
	    guy->cur_weap = w_knife;
	    return;
	}
    } 
}

void hurt_player(int pl, int dmg)
{
    player[pl].health-=dmg;
    spawn_particles(player[pl].x+3, player[pl].y+13, dmg, grad_red);
    if (player[pl].health < 1)
	respawn_player(pl);
}

void draw_players()
{
    int i, gun;
    int u, v, w, h;
    PLAYER *guy;

    for (i=0; i<MAX_PLAYERS; i++)
    {
	guy = &player[i];
	if (!guy->alive)
	    continue;

	u = guy->x-px;
	v = guy->y-py;

	switch (guy->cur_weap)
	{
	    case w_bow: gun = P_BOW0; break;
	    case w_knife: gun = P_KNIFE0; break;
	    case w_m16: gun = P_M160; break;
	    case w_minigun: gun = P_MINI0; break;
	    case w_pistol: gun = P_PISTOL0; break;
	    case w_bazooka: gun = P_ROCKET0; break;
	    case w_shotgun: gun = P_SHOTGUN0; break;
	    case w_uzi: gun = P_UZI0; break;
	    default: gun = 0; break;
	}

	if (mouse_x >= u+3)
	    guy->facing = right;
	else 
	    guy->facing = left;

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
    }
}



/* blast */

void blow_mine(int num);

void blast(int x, int y, int dmg)
{
    // blast affects:
    //  barrels, wood, crates
    //  mines
    //  players
    int u, v;
    int i, j, t;

    u = x/16;
    v = y/16;

    shake_factor += 10;

    // barrels, wood, crates 
    for (j=-1; j<2; j++) {
	for (i=-1; i<2; i++) {
	    if (v+j>=0 && v+j<map.h && u+i>=0 && u+i<map.h)
	    {
		t = map.tile[v+j][u+i];
		if (t==T_BAR || t==T_CRATE || t==T_WOOD)
		    map.tiletics[v+j][u+i] -= dmg;
	    } 
	}
    }

    // radius in pixels, 24 pixels around

    // mines
    for (i=0; i<MAX_MINES; i++)
    {
	if (mine[i].alive)
	{
	    u = mine[i].x+3;
	    v = mine[i].y;
	    if (u>=x-24 && u<=x+24 && v>=y-24 && v<=y+24)
		blow_mine(i);
	}
    }

    // players
    for (i=0; i<MAX_PLAYERS; i++)
    {
	if (player[i].alive)
	{
	    u = player[i].x;
	    v = player[i].y;
	    if (u>=x-24 && u<=x+24 && v>=y-24 && v<=y+24) 
	    {
		hurt_player(i, dmg);
		player[i].yv = -8;
		player[i].jump = 1;
	    }
	}
    }
}



/* mines */

void spawn_mine(int x, int y)
{
    int i;
    for (i=0; i<MAX_MINES; i++)
    {
	if (!mine[i].alive)
	{
	    mine[i].alive = 1;
	    mine[i].x = x;
	    mine[i].y = y;
	    mine[i].unactive = 40;
	    mine[i].frame = 0;
	    mine[i].tics = MINE_TICS;
	    return;
	}
    }

    suicide("Mine overflow");
}

void move_mine()
{
    int i;
    for (i=0; i<MAX_MINES; i++)
    {
	if (mine[i].alive)
	{
	    // drop
	    if (!tile_collide(mine[i].x + 3, mine[i].y+2) ||
		(mine[i].y%16)!=14)
		if (++mine[i].y>map.h*16)
		    mine[i].alive = 0; 

	    // activate
	    if (mine[i].unactive)
		mine[i].unactive--;

	    // anim
	    if (--mine[i].tics<0 && !mine[i].unactive)
	    {
		mine[i].tics = MINE_TICS;
		mine[i].frame = (mine[i].frame ? 0 : 1);
	    }
	}
    } 
}

void draw_mines()
{
    int i;
    for (i=0; i<MAX_MINES; i++)
    {
	if (mine[i].alive)
	{
	    draw_sprite(dbuf, dat[MINE1+mine[i].frame].dat, mine[i].x-px, mine[i].y-py);
	}
    } 
}

void blow_mine(int i)
{
    mine[i].alive = 0;
    spawn_explo(mine[i].x-2, mine[i].y-6, EXPLO1, 2);
    blast(mine[i].x+3, mine[i].y, weaps[w_mine].dmg);
}

void touch_mines()
{
    int i, j;
    int x, y, x2, y2;
    for (i=0; i<MAX_MINES; i++)
    {
	for (j=0; j<MAX_PLAYERS; j++)
	{
	    if (mine[i].alive && !mine[i].unactive)
	    {
		x = player[j].x+3;
		y = player[j].y+6;
		x2 = mine[i].x+3;
		y2 = mine[i].y;
		if (x>=x2-10 && x<=x2+10 && y>=y2-10 && y<=y2+10)
		    blow_mine(i);
	    }
	}
    }
}



/* bullets */

void spawn_bullet(int tag, int x, int y, fixed angle, int speed, int dmg, int c, int bmp)
{
    int i;
    for (i=0; i<MAX_BULLETS; i++)
    {
	if (!bullets[i].alive)
	{
	    bullets[i].x = itofix(x);
	    bullets[i].y = itofix(y);
	    bullets[i].xv = fcos(angle) * speed;
	    bullets[i].yv = fsin(angle) * speed;
	    bullets[i].dmg = dmg;
	    bullets[i].colour = c;
	    bullets[i].bmp = bmp;
	    bullets[i].angle = angle;
	    bullets[i].alive = 1;
	    return;
	}
    }

    suicide("Bullet overflow"); 
}

void move_bullets()
{
    int u, v, i;
    for (i=0; i<MAX_BULLETS; i++)
    {
	if (bullets[i].alive)
	{
	    int t;

	    bullets[i].x += bullets[i].xv;
	    bullets[i].y += bullets[i].yv;

	    if (bullets[i].bmp==J_KNIFE)
		bullets[i].angle += (bullets[i].xv>0) ? itofix(20) : -itofix(20);

	    if (bullets[i].x < itofix(0)     || bullets[i].y < itofix(0) ||
		bullets[i].x > itofix(map.w*16) || bullets[i].y > itofix(map.h*16))
	    {
		bullets[i].alive = 0;
	    }
	    else
	    {
		u = fixtoi(bullets[i].x)/16;
		v = fixtoi(bullets[i].y)/16;
		t = map.tile[v][u];
		if (t && t!=T_LAD) 
		{
		    bullets[i].alive = 0;
		    if (t==T_WOOD || t==T_CRATE || t==T_BAR)
			map.tiletics[v][u] -= bullets[i].dmg;

		    spawn_particles(fixtoi(bullets[i].x-bullets[i].xv), fixtoi(bullets[i].y-bullets[i].yv), 7, grad_orange);

		    if (bullets[i].bmp==J_ROCKET || bullets[i].bmp==J_ARROW) {
			spawn_explo(fixtoi(bullets[i].x), fixtoi(bullets[i].y), EXPLO1, 2);
			blast(fixtoi(bullets[i].x), fixtoi(bullets[i].y), bullets[i].dmg);
			    // fix my toy
		    }
		}
	    }
	}
    }
}

void draw_bullets()
{
    int i;
    for (i=0; i<MAX_BULLETS; i++)
    {
	if (bullets[i].alive) 
	{
	    if (bullets[i].bmp)
		rotate_sprite(dbuf, dat[bullets[i].bmp].dat, fixtoi(bullets[i].x) - px, fixtoi(bullets[i].y) - py, bullets[i].angle);
	    else
		putpixel(dbuf, fixtoi(bullets[i].x) - px, fixtoi(bullets[i].y) - py, bullets[i].colour);
	}
    } 
}



/* tiles */

void tiles_in_pain()
{
    int u, v, t;
    for (v=0; v<map.h; v++)
    {
	for (u=0; u<map.w; u++)
	{
	    t = map.tile[v][u];
	    if ((t==T_BAR || t==T_CRATE || t==T_WOOD) && map.tiletics[v][u]<=0)
	    {
		map.tile[v][u] = 0;
		spawn_explo(u*16, v*16, EXPLO1, 2);
		map.tiletics[v][u] = TILE_RESPAWN_RATE;
		if (t==T_BAR)
		    blast(u*16+8, v*16+8, 30);
	    }
	}
    }
}

void respawn_tiles()
{
    int v, u, i;
    for (v=0; v<map.h; v++)
	for (u=0; u<map.w; u++)
	    if (!map.tile[v][u] && map.tiletics[v][u])
		if (--map.tiletics[v][u]<1) 
		{
		    for (i=0; i<2; i++)
		    {
			if ((player[i].x/16==u && player[i].y/16==v) ||
			    ((player[i].x+5)/16==u && (player[i].y+15)/16==v))
			{
			    map.tiletics[v][u] = TILE_RESPAWN_RATE/2;
			    break;
			}
		    } 
		    if (i==2)
		    { 
			map.tile[v][u] = map.tileorig[v][u];
			map.tiletics[v][u] = TILE_HEALTH;
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



/* ammo */

void respawn_ammo()
{
    int v, u;
    for (v=0; v<map.h; v++)
	for (u=0; u<map.w; u++)
	    if (!map.ammo[v][u] && map.ammotics[v][u])
		if (--map.ammotics[v][u]<1) {
		    map.ammo[v][u] = map.ammoorig[v][u];
		    map.ammotics[v][u] = ammo_respawn_rate(map.ammo[v][u]);
		} 
}



/* status */

struct
{
    int pic;
    int weap;
    int shortcut;
} status_order[] = 
{
    { W_MINE, w_mine, KEY_ENTER },
    { W_KNIFE, w_knife, KEY_TILDE },
    { W_PISTOL, w_pistol, KEY_1 },
    { W_SHOTGUN, w_shotgun, KEY_2 },
    { W_UZI, w_uzi, KEY_3 },
    { W_M16, w_m16, KEY_4 },
    { W_MINI, w_minigun, KEY_5 },
    { W_BOW, w_bow, KEY_6 },
    { W_ROCKET, w_bazooka, KEY_7 },
    { 0 }
};

void draw_status()
{
    int y = SCREEN_H-6-16, i = 0;
    int c;
    char buf[10];

    text_mode(-1);

    while (status_order[i].pic)
    {
	if (player[local].have[status_order[i].weap])
	{
	    draw_sprite(xbuf, dat[status_order[i].pic].dat, 
		6 + (player[local].cur_weap==status_order[i].weap ? 6 : 0), y);
	    y-=14;
	}
	i++;
    }

    switch (player[local].cur_weap)
    {
	case w_pistol:
	case w_uzi:
	case w_m16:
	case w_minigun:
	    c = player[local].num_bullets;
	    break;
	case w_shotgun:
	    c = player[local].num_shells;
	    break;
	case w_bazooka:
	    c = player[local].num_rockets;
	    break;
	case w_bow:
	    c = player[local].num_arrows;
	    break;
	default: 
	    c = -1;
	    break; 
    }

    if (c!=-1) {
	sprintf(buf, "%d", c); 
	textout(xbuf, font, buf, SCREEN_W-24-text_length(font, buf), SCREEN_H-24, RED);
    }

    textprintf(xbuf, font, 16, 6, RED, "%d", player[local].health);
}



/* handle player movement / input */

inline void select_weapon()
{
    int y = SCREEN_H-6-16, i = 0;
    PLAYER *guy = &player[local];

    while (status_order[i].pic)
    {
	if (guy->have[status_order[i].weap])
	{
	    if (((mouse_b & 2) && mouse_x >= 6 && mouse_x <= 6+16 &&
		mouse_y >= y && mouse_y <= y+16) ||
		key[status_order[i].shortcut])
	    {
		if (status_order[i].weap==w_mine && guy->num_mines>0)
		    guy->drop_mine = 1;
		else
		    guy->select[status_order[i].weap] = 1;
		return; 
	    }
	    y-=14; 
	}
	i++;
    }
}

void get_local_input()
{
    int i;
    for (i=0; i<num_weaps; i++)
	player[local].select[i] = 0;

    player[local].up = key[KEY_UP] | key[KEY_T] | key[KEY_W];
    player[local].down = key[KEY_DOWN] | key[KEY_G] | key[KEY_S];
    player[local].left = key[KEY_LEFT] | key[KEY_F] | key[KEY_A];
    player[local].right = key[KEY_RIGHT] | key[KEY_H] | key[KEY_D];

    player[local].fire = 0;
    player[local].drop_mine = 0;

    if (!player[local].next_fire) {
	player[local].fire = (mouse_b & 1) | key[KEY_CONTROL];
	select_weapon();
    }
}

void move_player(int pl)
{
    PLAYER *guy;
    int kx, ky, a, t;
    int move_legs = 0;

    guy = &player[pl];

    /* selecting weapon */
    for (a=0; a<num_weaps; a++)
    {
	if (guy->select[a])
	{
	    guy->cur_weap = a;
	    break;
	}
    }

    /* picking up stuff */
    kx = (guy->x+3)/16;
    ky = (guy->y+6)/16; 
    a = map.ammo[ky][kx];
    switch (a)
    {
	case A_ARMOUR:
	    guy->armour+=20;
	    map.ammo[ky][kx] = 0;
	    break;
	case A_ARROW:
	    guy->num_arrows+=7;
	    map.ammo[ky][kx] = 0;
	    break;
	case A_BULLET:
	    guy->num_bullets+=30;
	    map.ammo[ky][kx] = 0;
	    break;
	case A_CHICKEN:
	    if (guy->health<200) {
		guy->health+=50;
		if (guy->health>200)
		    guy->health = 200;
		map.ammo[ky][kx] = 0;
	    }
	    break;
	case A_COKE:
	    if (guy->health<200) {
		guy->health+=15;
		if (guy->health>200)
		    guy->health = 200;
		map.ammo[ky][kx] = 0;
	    }
	    break;
	case W_MINE:
	    guy->num_mines+=1;
	    guy->have[w_mine] = 1;
	    map.ammo[ky][kx] = 0;
	    break;
	case A_ROCKET:
	    guy->num_rockets+=3;
	    map.ammo[ky][kx] = 0;
	    break;
	case A_SHELL:
	    guy->num_shells+=8;
	    map.ammo[ky][kx] = 0;
	    break;
	case W_BOW:
	    guy->have[w_bow]=1;
	    guy->num_arrows+=3;
	    map.ammo[ky][kx] = 0;
	    break;
	case W_M16:
	    if (!guy->have[w_m16] && !guy->next_fire)
		guy->cur_weap = w_m16;
	    guy->have[w_m16]=1;
	    guy->num_bullets+=40;
	    map.ammo[ky][kx] = 0;
	    break;
	case W_MINI:
	    if (!guy->have[w_minigun] && !guy->next_fire)
		guy->cur_weap = w_minigun;
	    guy->have[w_minigun]=1;
	    guy->num_bullets+=50;
	    map.ammo[ky][kx] = 0;
	    break;
	case W_PISTOL:
	    if (!guy->have[w_pistol] && !guy->next_fire)
		guy->cur_weap = w_pistol;
	    guy->have[w_pistol]=1;
	    guy->num_bullets+=10;
	    map.ammo[ky][kx] = 0;
	    break;
	case W_ROCKET:
	    guy->have[w_bazooka]=1;
	    guy->num_rockets+=2;
	    map.ammo[ky][kx] = 0;
	    break;
	case W_SHOTGUN:
	    if (!guy->have[w_shotgun] && !guy->next_fire)
		guy->cur_weap = w_shotgun; 
	    guy->have[w_shotgun]=1;
	    guy->num_shells+=4;
	    map.ammo[ky][kx] = 0;
	    break;
	case W_UZI:
	    if (!guy->have[w_uzi] && !guy->next_fire)
		guy->cur_weap = w_uzi; 
	    guy->have[w_uzi]=1;
	    guy->num_bullets+=30;
	    map.ammo[ky][kx] = 0;
	    break;
	default: 
	    break;
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
	    case w_m16:
	    case w_uzi:
	    case w_pistol:
		if (--guy->num_bullets < 0)
		    fired = guy->num_bullets = 0;
		else
		    spawn_bullet(pl, guy->x+3, guy->y+4, guy->angle, 8, weaps[guy->cur_weap].dmg, GREY, 0);
		break;

	    case w_shotgun:
		if (--guy->num_shells < 0)
		    fired = guy->num_shells = 0;
		else
		{
		    spawn_bullet(pl, guy->x+3, guy->y+6, guy->angle - itofix(5), 8, weaps[w_shotgun].dmg, YELLOW, 0);
		    spawn_bullet(pl, guy->x+3, guy->y+6, guy->angle - itofix(3), 8, weaps[w_shotgun].dmg, YELLOW, 0);
		    spawn_bullet(pl, guy->x+3, guy->y+6, guy->angle,             8, weaps[w_shotgun].dmg, YELLOW, 0);
		    spawn_bullet(pl, guy->x+3, guy->y+6, guy->angle + itofix(2), 8, weaps[w_shotgun].dmg, YELLOW, 0);
		    spawn_bullet(pl, guy->x+3, guy->y+6, guy->angle + itofix(4), 8, weaps[w_shotgun].dmg, YELLOW, 0);
		}
		break;

	    case w_bazooka:
		if (--guy->num_rockets < 0)
		    fired = guy->num_rockets = 0;
		else
		    spawn_bullet(pl, guy->x+3, guy->y+3, guy->angle, 8, weaps[w_bazooka].dmg, 0, J_ROCKET);
		break;

	    case w_bow:
		if (--guy->num_arrows < 0)
		    fired = guy->num_arrows = 0;
		else
		    spawn_bullet(pl, guy->x+3, guy->y+3, guy->angle, 8, weaps[w_bow].dmg, 0, J_ARROW);
		break;

	    case w_knife:
		spawn_bullet(pl, guy->x+3, guy->y+6, guy->angle, 8, weaps[w_knife].dmg, 0, J_KNIFE);
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
	    guy->fire_tics = 0;
	}
    }
    else 
    {
	guy->firing= 0;
    }

    /* dropping mines */
    if (guy->drop_mine)
    {
	spawn_mine(guy->x, guy->y+12);
	guy->next_fire = weaps[w_mine].reload;
	if (--guy->num_mines<1)
	    guy->have[w_mine] = 0;
    }

    /* impaled on spikes ? */
    if (tile_collide(guy->x+3, guy->y+16)==T_SPIKE)
	hurt_player(pl, 3);

    /* climb down ladder */
    if (guy->down)
    {
	if (tile_collide(guy->x+3, guy->y+17)==T_LAD) {
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
		guy->y-=2;
		move_legs = 1;
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
	if (guy->yv==0)
	    guy->jump = guy->yv = 0; 

	t = tile_collide(guy->x+3, guy->y);
	if (t && t!=T_LAD)
	{
	    guy->y = ((int)(guy->y+16)/16) * 16;
	    guy->jump = guy->yv = 0;
	} 
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

    /* anims */
    if (move_legs)
    {
	if (--guy->leg_tics<0) {
	    guy->leg_tics=LEG_TICS;
	    if (++guy->leg_frame>3)
		guy->leg_frame=0;
	}
    }

    if (guy->firing || guy->fire_frame)
    {
	if (--guy->fire_tics<0)
	{
	    ++guy->fire_frame;
	    if (guy->fire_frame>4 || (guy->cur_weap!=w_minigun && guy->fire_frame>3))
	    {
		guy->fire_frame = 0;
		if ((guy->cur_weap==w_uzi || guy->cur_weap==w_minigun) && guy->firing)
		    guy->fire_frame = 1;
	    }
	    guy->fire_tics = weaps[guy->cur_weap].anim;
	}
    }
}



/* the loop / helpers */

void draw_spotlight()
{
    int u, v;
    int flash=0; 

    u = player[local].x-px-96;
    v = player[local].y-py-96;
    if (!crap_comp && (player[local].fire_frame || player[local].firing) && 
	player[local].cur_weap != w_knife && player[local].cur_weap != w_bow)
	flash = 1;
    if (flash) {
       u += (random()%10) - 5;
       v += (random()%10) - 5;
    }
    clear(xbuf);
    blit(dbuf, xbuf, u, v, u, v, 192, 192);

    if (!crap_comp)
    {
	if (flash)
	    draw_trans_sprite(xbuf, spotlight2, u, v);
	else
	    draw_trans_sprite(xbuf, spotlight, u, v);
    }
    else
	draw_sprite(xbuf, spotlight, u, v);
}

void game_loop()
{
    int shakex = 0, shakey = 0;
    int bx, by;

    speed_counter = 0;

    while (!key[KEY_ESC])
    {
	while (speed_counter > 0) 
	{
	    respawn_tiles();
	    respawn_ammo();
	    tiles_in_pain();

	    get_local_input();
	    //send_local_input();
	    //recv_remote_inputs();
	    move_player(local);
	    update_explo();
	    move_mine();
	    touch_mines();
	    move_bullets();
	    move_particles();

	    if (shake_factor>40) 
		shake_factor = 40;
	    else if (shake_factor)
		shake_factor--;
	    shakex = (random()%(shake_factor*2+1))-shake_factor;
	    shakey = (random()%(shake_factor*2+1))-shake_factor;

	    speed_counter--;
	}

	/* find top-left */
	mx = player[local].x / 16 - 10;
	my = player[local].y / 16 - 6;

	offsetx = player[local].x % 16;
	offsety = player[local].y % 16;

	if (mx<0) mx = offsetx = 0;
	if (my<0) my = offsety = 0;
	if (mx>=map.w-20) { mx=map.w-20; offsetx = 0; }
	if (my>map.h-13 || (my==map.h-13 && offsety>=8)) { my=map.h-13; offsety = 8; }

	px = mx * 16 + offsetx;
	py = my * 16 + offsety;

	// not a good place for this, but we need px and py
	player[local].angle = find_angle(player[local].x-px+3, player[local].y-py+4, mouse_x, mouse_y);

	/* tile backdrop */
	bx = -(px%640)/2;
	by = -(py%400)/2;
	blit(dat[BACKDROP].dat, dbuf, 0, 0, bx,     by,     320, 200);
	blit(dat[BACKDROP].dat, dbuf, 0, 0, bx,     by+200, 320, 200);
	blit(dat[BACKDROP].dat, dbuf, 0, 0, bx+320, by,     320, 200);
	blit(dat[BACKDROP].dat, dbuf, 0, 0, bx+320, by+200, 320, 200);

	// now draw, boy!
	draw_mines();
	draw_tiles_an_stuff();
	draw_bullets();
	draw_players();
	draw_particles();
	draw_explo();
	draw_spotlight();
	draw_status(); 

	//vsync();
	show_mouse(NULL);
	blit(xbuf, screen, 0, 0, 0 + shakex, 0 + shakey, SCREEN_W, SCREEN_H);
	show_mouse(screen);
    }
}



/* main */

int main(int argc, char **argv)
{
    int i;

    /* init allegro */
    allegro_init();
    if (install_mouse()==-1)
    {
	printf("Install a mouse driver.");
	exit(1);
    }
    install_timer();
    install_keyboard();

    /* lock stuff */
    LOCK_VARIABLE(speed_counter);
    LOCK_FUNCTION(increment_speed_counter);

    /* options */
    if (argc>1 && !strcmp("-crap", argv[1]))
    {
	crap_comp = 1;
	printf("You have a crap computer.\n");
    }

    /* load datafile */
    dat = load_datafile("blood.dat");
    if (!dat)
    {
	printf("Error loading blood.dat\n");
	return 1;
    }

    /* init screen etc */
    set_gfx_mode(GFX_VGA, 320, 200, 0, 0);
    dbuf = create_bitmap(SCREEN_W, SCREEN_H);
    xbuf = create_bitmap(SCREEN_W, SCREEN_H);
    set_palette(dat[GAMEPAL].dat);

    /* colour / translucency tables */
    create_rgb_table(&rgb_table, dat[GAMEPAL].dat, NULL);
    rgb_map = &rgb_table; 
    create_light_table(&light_table, dat[GAMEPAL].dat, 0, 0, 0, NULL);
    color_map = &light_table;

    /* spot lights */
    spotlight = create_bitmap(192,192);
    spotlight2 = create_bitmap(192,192);
    if (!crap_comp)
    {
	clear(spotlight);
	for (i=0; i<256; i++)
	    circlefill(spotlight, 96, 96, 96-i/4, i);

	clear(spotlight2);
	for (i=0; i<256; i++)
	    circlefill(spotlight2, 96, 96, 96-i/4, (i+48>255) ? 255 : i+48);
    }
    else
    {
	RGB rgb = { 0, 0, 0 };
	set_color(1, &rgb);
	clear_to_color(spotlight, 1);
	circlefill(spotlight, 96, 96, 96, 0);
    }

    /* misc */
    font = dat[UNREAL].dat;

    set_mouse_sprite(dat[XHAIR].dat);
    set_mouse_sprite_focus(1,1);

    /* temp */
    load_map("deathmat.wak");

    srandom(time(NULL));
    next_position = random()%(24*24);

    local = 0;
    respawn_player(local);

    /* ready */
    install_int_ex(increment_speed_counter, BPS_TO_TIMER(GAME_SPEED));

    /* set */
    //intro();

    /* go */
    game_loop();

    /* bye bye */
    killall();
    return 0;
}