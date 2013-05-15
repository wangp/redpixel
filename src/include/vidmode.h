#ifndef __include_vidmode_h
#define __include_vidmode_h


#define GAME_W	320
#define GAME_H	200

int set_video_mode(void);
void toggle_fullscreen_window(void);

void rp_hide_mouse(void);
void rp_show_mouse(void);

void maybe_trapped_mouse(void);
void leave_trapped_mouse(void);
bool is_trapped_mouse(void);
void poll_trapped_mouse(void);
void get_game_mouse_pos(int *mx, int *my);

void blit_to_screen_offset(BITMAP *buffer, int ox, int oy);
void blit_to_screen(BITMAP *buffer);

void rp_fade_in(BITMAP *bmp, int speed);
void rp_fade_out(BITMAP *bmp, int speed);

void vidmode_init(void);
void vidmode_shutdown(void);


#endif
