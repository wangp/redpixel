#ifndef __included_backpack_h
#define __included_backpack_h


void spawn_backpack(int x, int y, int b, int s, int r, int a, int m);
void update_backpacks();
void draw_backpacks();
void touch_backpacks();

int backpacks_init();
void backpacks_shutdown();
void reset_backpacks();


#endif
