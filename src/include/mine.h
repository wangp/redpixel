#ifndef __included_mine_h
#define __included_mine_h


typedef struct {
    char alive;
    int x, y;
    char frame, anim;
    char tag, unactive;
} MINE;


void spawn_mine(int x, int y, int tag);
void update_mines();
void draw_mines();
void blow_mine(int i);
void touch_mines();
int mine_in_range(int i, int x1, int y1, int x2, int y2);

int mines_init();
void mines_shutdown();
void reset_mines();
int max_mines();


#endif
