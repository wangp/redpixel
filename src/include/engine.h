#ifndef __included_run_h
#define __included_run_h


#define MAX_PLAYERS	2	/* too many assumptions already   
				 * made about this :-P  */

#define GAME_SPEED  	40	/* max FPS */


#define w_knife     1
#define w_pistol    2
#define w_bow       3
#define w_shotgun   4
#define w_uzi       5
#define w_m16       6
#define w_minigun   7
#define w_bazooka   8
#define w_mine      9
#define num_weaps   10


//---------------------------------------------------------------- structs ---

typedef struct 
{
    int anim;
    int reload;
    int dmg;
} WEAPON;

typedef struct
{
    int life;
    fixed x, y;
    fixed xv, yv;
    unsigned char colour;

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
    int life;
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


//--------------------------------------------------------- globals ----------

extern DATAFILE    *dat;
extern BITMAP      *dbuf;
extern BITMAP      *light;

extern RGB_MAP     rgb_table;
extern COLOR_MAP   alpha_map;
extern COLOR_MAP   light_map;

extern int local;          // local player
extern int num_players;

extern int next_position;
extern int rnd_index;
extern int irnd_index;

extern WEAPON weaps[];

extern PLAYER players[];

extern volatile int speed_counter;

extern int record_demos;
extern int com_port;


//-------------------------------------------------------- connection type ---

typedef enum {
    single,
    serial,
    demo
} comm_t;

extern comm_t comm;


//-------------------------------------------------------- prototypes --------

void send_long(long);
long recv_long();

void spawn_players();

void no_germs();
void retain_players();
void restore_players();

void game_loop();

void engine_init();
void engine_shutdown();


#endif;
