#ifndef RUN_H
#define RUN_H


typedef struct
{
    char life;
    fixed x, y;
    fixed xv, yv;
    char colour;

    // for bullets
    unsigned char bmp,  // the no. of the bitmap
		  tag,  // player who fired it
		  dmg;  // dmg it causes
    fixed angle; 
} PARTICLE;



typedef struct
{
    char alive;
    int x, y;
    char nopic; // if nopic, then it is just a light source
    unsigned char pic, cur, frames, anim; 
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
    unsigned char pic;
} BLOD;



typedef struct
{
    char alive;
    int x, y;
    char facing;
    unsigned char first_frame, num_frames;
    unsigned char cur, anim;
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



enum {
    w_knife = 1,
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



typedef struct
{
    char exist;
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
    char firing, fire_frame, fire_anim, next_fire;
    char facing;

    char up, down, left, right, fire, drop_mine;
    char select, respawn;

    char have[num_weaps];
    unsigned char cur_weap, pref_weap;
} PLAYER;



#endif