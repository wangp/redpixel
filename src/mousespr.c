/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Mouse sprite.
 */


#include "a4aux.h"
#include "mousespr.h"


static ALLEGRO_MOUSE_CURSOR *cursor;


void set_stretched_mouse_sprite(BITMAP *sprite, int factor, int hotx, int hoty)
{
    ALLEGRO_BITMAP *old;
    ALLEGRO_BITMAP *bmp;
    int dw, dh;

    free_stretched_mouse_sprite();

    dw = sprite->w * factor;
    dh = sprite->h * factor;
    bmp = al_create_bitmap(dw, dh);

    old = al_get_target_bitmap();
    al_set_target_bitmap(bmp);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));
    al_draw_scaled_bitmap(sprite->real, 0, 0, sprite->w, sprite->h,
	0, 0, dw, dh, 0);
    al_set_target_bitmap(old);

    cursor = al_create_mouse_cursor(bmp, hotx * factor, hoty * factor);
    al_set_mouse_cursor(al_get_current_display(), cursor);

    al_destroy_bitmap(bmp);
}


void free_stretched_mouse_sprite(void)
{
    if (cursor) {
	al_set_system_mouse_cursor(al_get_current_display(),
	    ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
	al_destroy_mouse_cursor(cursor);
	cursor = NULL;
    }
}
