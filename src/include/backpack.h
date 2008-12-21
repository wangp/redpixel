#ifndef __included_backpack_h
#define __included_backpack_h


void spawn_backpack(int x, int y, int b, int s, int r, int a, int m);
void update_backpacks(void);
void draw_backpacks(void);
void touch_backpacks(void);

int backpacks_init(void);
void backpacks_shutdown(void);
void reset_backpacks(void);


#endif
