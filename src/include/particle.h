#ifndef __included_part_h
#define __included_part_h


enum {
    grad_red,
    grad_orange,
};


typedef struct {
    int life;
    fixed x, y;
    fixed xv, yv;
    unsigned char colour;

    /* for bullets */
    short bmp;  
    unsigned char tag, dmg;
    fixed angle; 
} PARTICLE;


void spawn_particles(int x, int y, int num, int grad);
void spawn_casing(int x, int y, int facing);
void update_particles();
void draw_particles();
void reset_particles();
    

#endif
