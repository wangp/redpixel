/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 *
 *  Blitting to screen.
 */


#include <allegro.h>
#include "scrblit.h"


/* see also main.c (set_video_mode) */


inline void blit_to_screen_offset(BITMAP *buffer, int ox, int oy)
{
    if ((SCREEN_W == 320) && (SCREEN_H == 240))
	blit(buffer, screen, 0, 0, ox, 20+oy, 320, 200);
    else if ((SCREEN_W == 640) && (SCREEN_H == 400))
	stretch_blit(buffer, screen, 0, 0, 320, 200, ox, oy, 640, 400);
    else if ((SCREEN_W == 640) && (SCREEN_H == 480))
	stretch_blit(buffer, screen, 0, 0, 320, 200, ox, 40+oy, 640, 400);
    else
	blit(buffer, screen, 0, 0, ox, oy, 320, 200);
}


void blit_to_screen(BITMAP *buffer)
{
    blit_to_screen_offset(buffer, 0, 0);
}
