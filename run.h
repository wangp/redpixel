#ifndef RUN_H
#define RUN_H


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



typedef struct
{
    fixed x, y;
    int pic;
    char cur, frames, tics;
    char alive;
} EXPLOSION;



typedef struct
{
    int x, y;
    char frame, tics;
    char unactive;
    char alive;
} MINE;



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
    char alive;
    int x, y;
    char jump, yv, xv;
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
    unsigned char select, respawn;

    unsigned char have[num_weaps];
    unsigned char cur_weap;
} PLAYER;



#endif