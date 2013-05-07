#ifndef __include_vidmode_h
#define __include_vidmode_h


#define VID_320x200_FULLSCREEN	0
#define VID_320x240_FULLSCREEN	1
#define VID_640x400_FULLSCREEN	2
#define VID_640x480_FULLSCREEN	3
#define VID_640x400_WINDOWED	4
#define VID_320x200_WINDOWED	5

extern int desired_video_mode;

int set_desired_video_mode_or_fallback(void);

void blit_to_screen_offset(BITMAP *buffer, int ox, int oy);
void blit_to_screen(BITMAP *buffer);

void rp_fade_in(BITMAP *bmp, int speed);
void rp_fade_out(BITMAP *bmp, int speed);

void vidmode_init(void);
void vidmode_shutdown(void);


#endif
