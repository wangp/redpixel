#ifndef __included_explo_h
#define __included_explo_h


typedef struct {
    char alive;
    int x, y;
    char nopic;		/* if nopic, then it is just a light source */
    short pic;
    unsigned char cur, frames, anim; 
} EXPLOSION;


void spawn_explo(int x, int y, int pic, int frames);
void update_explo();
void draw_explo();
void reset_explo();


#endif
