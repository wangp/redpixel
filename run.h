#ifndef RUN_H
#define RUN_H

#include "w_types.h"


#define GAME_SPEED  40



typedef struct
{
    char life;
    fixed x, y;
    fixed xv, yv;
    char colour;

    // for bullets
    short           bmp;  // the no. of the bitmap
    unsigned char   tag,  // player who fired it
		    dmg;  // dmg it causes
    fixed angle; 
} PARTICLE;



typedef struct
{
    char alive;
    int x, y;
    char nopic; // if nopic, then it is just a light source
    short pic;
    unsigned char cur, frames, anim; 
} EXPLOSION;



typedef struct
{
    char alive;
    int x, y;
    char frame, anim;
    char tag, unactive;
} MINE;



typedef struct
{
    char life;
    int x, y;
    short pic;
} BLOD;



typedef struct
{
    char alive;
    int x, y;
    char facing;
    short first_frame;
    unsigned char num_frames, cur, anim;
} CORPSE;



typedef struct
{
    char alive, unactive;
    int x, y;
    int num_bullets, num_shells, num_rockets, num_arrows, num_mines;
} BACKPACK;



enum {
    left,
    right
};



typedef struct
{
    char exist;

    char name[40];
    int colour;

    int x, y, xv, yv, jump;
    int health, armour;
    fixed angle;

    int num_bullets;
    int num_shells;
    int num_rockets;
    int num_mines;
    int num_arrows;

    int frags;

    char leg_frame, leg_tics;
    int visor_tics;
    int blood_tics;
    int bloodlust;
    char firing, fire_frame, fire_anim, next_fire;
    char facing;

    char up, down, left, right, fire, drop_mine;
    char select, respawn;

    char have[num_weaps];
    unsigned char cur_weap, pref_weap;
} PLAYER;



// globals

extern DATAFILE    *dat;
extern BITMAP      *dbuf;
extern BITMAP      *light;

extern RGB_MAP     rgb_table;
extern COLOR_MAP   alpha_map;
extern COLOR_MAP   light_map;


extern time_t seed;


typedef enum {
    single,     // boring...
    serial,     // done
    ipx,        // not yet..
    udp         // keep dreaming!
} comm_t;

extern comm_t comm;


extern int local;          // local player
extern int num_players;

extern int next_position;
extern int rnd_index;
extern int irnd_index;

extern PLAYER players[];


extern volatile int speed_counter;



// prototypes

void send_long(long);
long recv_long();

void no_germs();
void spawn_players();
void game_loop();

#endif