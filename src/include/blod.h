#ifndef __included_blod_h
#define __included_blod_h


typedef struct {
    int life;
    int x, y;
    short pic;
} BLOD;


void spawn_blods(int x, int y, int num);
void update_blods();
void draw_blods();
void reset_blods();


#endif
