#ifndef __included_player_h
#define __included_player_h


#include "engine.h"
#include "weapon.h"


#define MAX_PLAYERS	2	/* too many assumptions already   
				 * made about this :-P  */

typedef struct {
    char exist;

    char name[40];
    int frags;

    int x, y, xv, yv, jump;
    int health, armour;
    fixed angle;

    int num_bullets;
    int num_shells;
    int num_rockets;
    int num_mines;
    int num_arrows;

    char leg_frame, leg_tics;
    int scanner_tics;
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


extern PLAYER players[];
extern int num_players;

extern int next_position;


void respawn_player(int pl);
void hurt_player(int pl, int dmg, int protect, int tag, int deathseq);
int gun_pic(int pl);	
void draw_players();
void draw_scanner(int haywire);

void auto_weapon(int pl, int new_weapon);
int num_ammo(int pl, int weapon);

void get_local_input();
int load_playerstat(char *packet);
void make_playerstat(char *packet, int pl);

void clean_player(int pl);
void retain_players();
void restore_players();

void spawn_players();
void reset_players();
void reset_player_frags();


/* plupdate.c */
void update_player(int pl);


#endif
