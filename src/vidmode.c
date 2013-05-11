/* 
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Video modes and blitting to screen.
 */


#include "a4aux.h"
#include "vidmode.h"


static int viewport_x;
static int viewport_y;
static int viewport_w;
static int viewport_h;


static void setup_scaling(int w, int h)
{
    const double preferred_aspect = (double)GAME_W / GAME_H;
    const double aspect = (double)w / h;

    if (aspect < preferred_aspect - 0.001) {
	viewport_w = w;
	viewport_h = w / preferred_aspect;
	viewport_x = 0;
	viewport_y = (h - viewport_h) / 2;
    }
    else if (aspect > preferred_aspect + 0.001) {
	viewport_w = h * preferred_aspect;
	viewport_h = h;
	viewport_x = (w - viewport_w) / 2;
	viewport_y = 0;
    }
    else {
	viewport_x = 0;
	viewport_y = 0;
	viewport_w = w;
	viewport_h = h;
    }
}


int set_video_mode(void)
{
    if (set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0) < 0) {
	return -1;
    }
    
    setup_scaling(SCREEN_W, SCREEN_H);
    
    if (set_display_switch_mode(SWITCH_BACKAMNESIA) == -1)
	set_display_switch_mode(SWITCH_BACKGROUND);
    
    return 0;
}


void toggle_fullscreen_window(void)
{
    ALLEGRO_DISPLAY *dpy = al_get_current_display();
    int flags = al_get_display_flags(dpy);
    al_set_display_flag(dpy, ALLEGRO_FULLSCREEN_WINDOW,
	!(flags & ALLEGRO_FULLSCREEN_WINDOW));

    gfx_driver->w = screen->w = al_get_display_width(dpy);
    gfx_driver->h = screen->h = al_get_display_height(dpy);
    setup_scaling(SCREEN_W, SCREEN_H);
}


void get_game_mouse_pos(int *mx, int *my)
{
    int x, y;

    x = (mouse_x - viewport_x) * GAME_W / viewport_w;
    y = (mouse_y - viewport_y) * GAME_H / viewport_h;

    *mx = MID(0, x, GAME_W-1);
    *my = MID(0, y, GAME_H-1);
}


inline void blit_to_screen_offset(BITMAP *buffer, int ox, int oy)
{
    double scale = (double)viewport_w / GAME_W;

    /* Ensure black borders are black. */
    al_set_target_bitmap(screen->real);
    al_clear_to_color(al_map_rgb(0, 0, 0));

    stretch_blit(buffer, screen, 0, 0, GAME_W, GAME_H,
	viewport_x + scale * ox, viewport_y + scale *oy,
	viewport_w, viewport_h);
}


void blit_to_screen(BITMAP *buffer)
{
    blit_to_screen_offset(buffer, 0, 0);
}


static void do_fade(ALLEGRO_BITMAP *bmp, int speed, int fadein)
{
    ALLEGRO_STATE state;
    int c, i;

    al_store_state(&state, ALLEGRO_STATE_TARGET_BITMAP|ALLEGRO_STATE_BLENDER);
    al_set_target_bitmap(screen->real);
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);

    for (c=0; c<256; c+=2*speed) {
        i = (fadein) ? MID(0, c, 255) : MID(0, 255-c, 255);
        al_draw_tinted_scaled_bitmap(bmp, al_map_rgb(i, i, i),
            0, 0, al_get_bitmap_width(bmp), al_get_bitmap_height(bmp),
            viewport_x, viewport_y, viewport_w, viewport_h, 0);
        al_flip_display();
    }

    al_restore_state(&state);
}


void rp_fade_in(BITMAP *bmp, int speed)
{
    do_fade(bmp->real, speed, TRUE);
}


void rp_fade_out(BITMAP *bmp, int speed)
{
    do_fade(bmp->real, speed, FALSE);
}


void vidmode_init(void)
{
}


void vidmode_shutdown(void)
{
}
