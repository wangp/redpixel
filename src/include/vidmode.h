#ifndef __include_vidmode_h
#define __include_vidmode_h


int set_video_mode(void);

void blit_to_screen_offset(BITMAP *buffer, int ox, int oy);
void blit_to_screen(BITMAP *buffer);

void rp_fade_in(BITMAP *bmp, int speed);
void rp_fade_out(BITMAP *bmp, int speed);

void vidmode_init(void);
void vidmode_shutdown(void);


#endif
