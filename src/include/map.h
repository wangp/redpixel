#ifndef __included_common_h
#define __included_common_h


typedef struct {
    int tile[128][128];
    int tileorig[128][128];
    int tiletics[128][128];

    int ammo[128][128];
    int ammoorig[128][128];
    int ammotics[128][128];

    int w, h;

    struct {
	int x, y;
    } start[24];
} MAP;


typedef struct {
    int pic;
    int num;
} LIST;


extern MAP map;

extern LIST tiles[];
extern LIST ammos[];


int num2pic(const LIST *l, int num);
int pic2num(const LIST *l, int pic);

int ammo_respawn_rate(int pic);

void reset_map(void);
int save_map(const char *fn);
int load_map(const char *fn);
int load_map_wrapper(const char *fn);


#endif
