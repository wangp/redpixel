#ifndef __included_corpse_h
#define __included_corpse_h


typedef struct {
    char alive;
    int x, y;
    char facing;
    short first_frame;
    unsigned char num_frames, cur, anim;
} CORPSE;


void spawn_corpse(int x, int y, int facing, int first, int frames);
void update_corpses();
void draw_corpses();
void reset_corpses();


#endif
