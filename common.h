#ifndef COMMON_H
#define COMMON_H

#include <time.h>       // not good practice but anyway
#include <allegro.h> 
#include "blood.h"      // datafile


//---------------------------------------------------------------- stuff -----

#define VERSION_STR "v0.7"

#define MAX_PLAYERS         8

#define TILE_HEALTH         30
#define TILE_RESPAWN_RATE   700


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

//--------------------------------------------------------------- prototypes -

int num2pic(LIST *l, int num);
int pic2num(LIST *l, int pic);

int ammo_respawn_rate(int pic);

void reset_map();
void save_map(char *fn);
void load_map(char *fn);

#endif