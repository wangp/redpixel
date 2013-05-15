/* 
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Video modes and blitting to screen.
 */


#include "a4aux.h"
#include "blood.h"
#include "globals.h"
#include "mousespr.h"
#include "vidmode.h"


static int viewport_x;
static int viewport_y;
static int viewport_w;
static int viewport_h;

static bool rp_mouse_shown = true;

static ALLEGRO_EVENT_QUEUE *trapped_mouse_queue;
static int trapped_mouse_x;
static int trapped_mouse_y;


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
    if (set_gfx_mode(GFX_AUTODETECT, 3*320, 3*200, 0, 0) < 0) {
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

    set_stretched_mouse_sprite(dat[XHAIRLCD].dat, screen->w, 2, 2);
}


void rp_hide_mouse(void)
{
    if (rp_mouse_shown) {
	al_hide_mouse_cursor(al_get_current_display());
	rp_mouse_shown = false;
    }
}


void rp_show_mouse(void)
{
    if (!rp_mouse_shown) {
	rp_mouse_shown = true;
	if (!is_trapped_mouse()) {
	    al_show_mouse_cursor(al_get_current_display());
	}
    }
}


static void enter_trapped_mouse(void)
{
    if (trapped_mouse_queue)
	return;

    trapped_mouse_queue = al_create_event_queue();
    al_register_event_source(trapped_mouse_queue, al_get_mouse_event_source());
    trapped_mouse_x = mouse_x;
    trapped_mouse_y = mouse_y;
    al_hide_mouse_cursor(al_get_current_display());
}


void leave_trapped_mouse(void)
{
    ALLEGRO_DISPLAY *dpy;

    if (!trapped_mouse_queue)
	return;

    al_destroy_event_queue(trapped_mouse_queue);
    trapped_mouse_queue = NULL;

    dpy = al_get_current_display();
    al_set_mouse_xy(dpy,
	viewport_x + trapped_mouse_x,
	viewport_y + trapped_mouse_y);
    if (rp_mouse_shown) {
	al_show_mouse_cursor(dpy);
    }
}


void maybe_trapped_mouse(void)
{
    int flags = al_get_display_flags(al_get_current_display());
    if (flags & (ALLEGRO_FULLSCREEN | ALLEGRO_FULLSCREEN_WINDOW))
	leave_trapped_mouse();
    else
	enter_trapped_mouse();
}


bool is_trapped_mouse(void)
{
    return (trapped_mouse_queue) ? true : false;
}


void poll_trapped_mouse(void)
{
    ALLEGRO_EVENT ev;
    bool warp = false;
    ALLEGRO_DISPLAY *d = NULL;
    int w, h;

    if (!trapped_mouse_queue)
	return;

    while (al_get_next_event(trapped_mouse_queue, &ev)) {
	switch (ev.type) {
	    case ALLEGRO_EVENT_MOUSE_AXES:
		d = ev.mouse.display;
		w = al_get_display_width(ev.mouse.display);
		h = al_get_display_height(ev.mouse.display);
		trapped_mouse_x = MID(0, trapped_mouse_x + ev.mouse.dx, w-1);
		trapped_mouse_y = MID(0, trapped_mouse_y + ev.mouse.dy, h-1);
		if (ev.mouse.x < w/4 || ev.mouse.x > w*3/4 ||
		    ev.mouse.y < h/4 || ev.mouse.y > h*3/4)
		{
		    warp = true;
		}
		break;
	}
    }

    if (warp && d) {
	al_set_mouse_xy(d, al_get_display_width(d)/2,
	    al_get_display_height(d)/2);
    }
}


void get_game_mouse_pos(int *mx, int *my)
{
    int x0, y0;
    int x, y;

    if (is_trapped_mouse()) {
	x0 = trapped_mouse_x;
	y0 = trapped_mouse_y;
    }
    else {
	x0 = mouse_x;
	y0 = mouse_y;
    }

    x = (x0 - viewport_x) * GAME_W / viewport_w;
    y = (y0 - viewport_y) * GAME_H / viewport_h;

    *mx = MID(0, x, GAME_W-1);
    *my = MID(0, y, GAME_H-1);
}


inline void blit_to_screen_offset(BITMAP *buffer, int ox, int oy)
{
    double scale = (double)viewport_w / GAME_W;

    al_set_target_bitmap(screen->real);

    /* Ensure black borders are black. */
    al_clear_to_color(al_map_rgb(0, 0, 0));

    al_draw_scaled_bitmap(buffer->real, 0, 0, buffer->w, buffer->h,
	viewport_x + scale * ox, viewport_y + scale *oy,
	viewport_w, viewport_h,
	0);

    /* Draw mouse cursor manually if hardware cursor is hidden (because it is
     * being warped around).
     */
    if (rp_mouse_shown && is_trapped_mouse()) {
	const BITMAP *cursor = dat[XHAIRLCD].dat;
	const int hotx = 2;
	const int hoty = 2;
	al_draw_scaled_bitmap(cursor->real,
	    0, 0, cursor->w, cursor->h,
	    viewport_x + trapped_mouse_x - scale*hotx,
	    viewport_y + trapped_mouse_y - scale*hoty,
	    scale * cursor->w, scale * cursor->h,
	    0);
    }

    al_flip_display();
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
