#ifndef __included_gameloop_h
#define __included_gameloop_h


typedef enum {
    single,
    peerpeer,
    demo,
    cpu
} comm_t;


extern comm_t comm;

extern int local;
extern int mx, my, offsetx, offsety, px, py;		
extern int shake_factor;


void game_loop(void);


#endif
