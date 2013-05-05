/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Mouse sprite.
 */


#include "a4aux.h"
#include "mousespr.h"


static BITMAP *stretched_mouse_sprite;


void set_stretched_mouse_sprite(BITMAP *sprite, int factor, int hotx, int hoty)
{
    free_stretched_mouse_sprite();
    stretched_mouse_sprite = create_bitmap(sprite->w * factor, sprite->h * factor);
    clear_bitmap(stretched_mouse_sprite);
    stretch_blit(sprite, stretched_mouse_sprite, 0, 0, sprite->w, sprite->h,
		 0, 0, stretched_mouse_sprite->w, stretched_mouse_sprite->h);
    set_mouse_sprite(stretched_mouse_sprite);
    set_mouse_sprite_focus(hotx * factor, hoty * factor);
}


void free_stretched_mouse_sprite(void)
{
    if (stretched_mouse_sprite) {
	destroy_bitmap(stretched_mouse_sprite);
	stretched_mouse_sprite = NULL;
    }
}
