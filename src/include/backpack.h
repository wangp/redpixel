#ifndef __included_backpack_h
#define __included_backpack_h


typedef struct {
    char alive, unactive;
    int x, y;
    int num_bullets, num_shells, num_rockets, num_arrows, num_mines;
} BACKPACK;


void spawn_backpack(int x, int y, int b, int s, int r, int a, int m);
void update_backpacks();
void draw_backpacks();
void touch_backpacks();

int backpacks_init();
void backpacks_shutdown();
void reset_backpacks();


#endif
