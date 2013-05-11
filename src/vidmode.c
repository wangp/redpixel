/* 
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Video modes and blitting to screen.
 */


#include "a4aux.h"
#include "vidmode.h"


static void clip_to_size(void)
{
    set_clip_rect(screen, 0, 40, 640, 439);
}


int set_video_mode(void)
{
    if (set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0) < 0) {
	return -1;
    }
    
    clear_bitmap(screen);
    clip_to_size();
    
    if (set_display_switch_mode(SWITCH_BACKAMNESIA) == -1)
	set_display_switch_mode(SWITCH_BACKGROUND);
    
    return 0;
}


inline void blit_to_screen_offset(BITMAP *buffer, int ox, int oy)
{
    stretch_blit(buffer, screen, 0, 0, 320, 200, ox, 40+oy, 640, 400);
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
