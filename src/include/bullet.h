#ifndef __included_bullet_h
#define __included_bullet_h


void spawn_bullet(int pl, fixed angle, int c, int bmp);
void update_bullets();
void draw_bullets();
void touch_bullets();

int bullets_init();
void bullets_shutdown();
void reset_bullets();
void reset_bullets_with_tag(int tag);


#endif
