#ifndef __included_engine_h
#define __included_engine_h


#define GAME_SPEED  	40	/* max FPS */


enum {
    left,
    right
};


extern volatile int speed_counter;
extern volatile int frame_counter, last_fps;


int bb_collide(BITMAP *spr1, int x1, int y1, BITMAP *spr2, int x2, int y2);
fixed find_angle(int x1, int y1, int x2, int y2);
int find_distance(int x1, int y1, int x2, int y2);
void draw_light(int bmp, int cx, int cy);

void engine_init();
void engine_shutdown();
void reset_engine();


#endif;
