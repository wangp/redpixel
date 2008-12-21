#ifndef __included_bullet_h
#define __included_bullet_h


void spawn_bullet(int pl, fixed angle, int c, int bmp);
void update_bullets(void);
void draw_bullets(void);
void touch_bullets(void);

int bullets_init(void);
void bullets_shutdown(void);
void reset_bullets(void);
void reset_bullets_with_tag(int tag);


#endif
