#ifndef __included_mine_h
#define __included_mine_h


void spawn_mine(int x, int y, int tag);
void update_mines();
void draw_mines();
void blow_mine(int i);
void touch_mines();
int mine_in_range(int i, int x1, int y1, int x2, int y2);

int mines_init();
void mines_shutdown();
void reset_mines();
void reset_mines_with_tag(int tag);
int max_mines();


#endif
