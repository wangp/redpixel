#ifndef COMMON_H
#define COMMON_H

#include <time.h>       // not good practice but anyway
#include <allegro.h>
#include "version.h"
#include "blood.h"      // datafile
#include "stats.h"
#include "statlist.h"


//---------------------------------------------------------------- stuff -----

#define MAX_PLAYERS         2	       /* serial only */

#define GAME_SPEED  40		   /* FPS */


//---------------------------------------------------------------- colours ---

#define GREY    16
#define WHITE   31
#define YELLOW  175
#define RED     127
#define LBLUE   233
#define GREEN   255


//--------------------------------------------------------------- structs ----

typedef struct {
    int tile[128][128];
    int tileorig[128][128];
    int tiletics[128][128];

    int ammo[128][128];
    int ammoorig[128][128];
    int ammotics[128][128];

    int w, h;

    struct 
    {
	int x, y;
    } start[24];
} MAP;

typedef struct
{
    int pic;
    int num;
} LIST;


//--------------------------------------------------------------- globals ----

extern MAP map;

extern LIST tiles[];
extern LIST ammos[];

extern char game_path[1024], *game_path_p;

//--------------------------------------------------------------- prototypes -

int num2pic(LIST *l, int num);
int pic2num(LIST *l, int pic);

int ammo_respawn_rate(int pic);

void reset_map();
void save_map(char *fn);
void load_map(char *fn);

#endif
