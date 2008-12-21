/* 
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Video modes and blitting to screen.
 */


#include <allegro.h>
#include "suicide.h"
#include "vidmode.h"


int desired_video_mode;
int want_scanlines;
static BITMAP *scanlined_screen;


static void prepare_scanlines(void)
{
    int y, off;
    
    if ((!want_scanlines) || (SCREEN_H < 400))
	return;

    /* don't support banked bitmaps */
    if (!gfx_driver->linear)
	return;

#ifdef ALLEGRO_WITH_XWINDOWS
    /* The current X windows graphics driver currently doesn't like
     * our scanline trick with the sub-bitmap. I think it maybe should
     * be classified as a "banked" driver. */
    if ((gfx_driver->id == GFX_XWINDOWS_FULLSCREEN) ||
        (gfx_driver->id == GFX_XWINDOWS))
	return;
#endif
#ifdef ALLEGRO_WINDOWS
    /* The Windows windowed drivers don't like our scanline trick either
     * (well, some of them at least). */
    if (gfx_driver->windowed)
	return;
#endif

    scanlined_screen = create_sub_bitmap(screen, 0, (SCREEN_H == 400) ? 0 : 20,
					 SCREEN_W, 200);
    if (!scanlined_screen)
	suicide("Out of memory");
    
    off = (SCREEN_H == 400) ? 0 : 20;
    for (y = 0; y < scanlined_screen->h; y++)
	scanlined_screen->line[y] = screen->line[off + (y*2)];
}


static void unprepare_scanlines(void)
{
    if (scanlined_screen) {
	destroy_bitmap(scanlined_screen);
	scanlined_screen = NULL;
    }
}


static int set_desired_video_mode(void)
{
    switch (desired_video_mode) {
	case VID_320x200_FULLSCREEN:
	    return set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, 320, 200, 0, 0);
	    
        case VID_320x240_FULLSCREEN:
	    return set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, 320, 240, 0, 0);
	    
	case VID_640x400_FULLSCREEN:
	    return set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, 640, 400, 0, 0);
	
	case VID_640x480_FULLSCREEN:
	    return set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, 640, 480, 0, 0);
	    
	case VID_640x400_WINDOWED:
        default:
	    return set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 400, 0, 0);
	    
	case VID_320x200_WINDOWED:
	    return set_gfx_mode(GFX_AUTODETECT_WINDOWED, 320, 200, 0, 0);	    
    }
}


static void clip_to_size(void)
{
    if ((SCREEN_W == 320) && (SCREEN_H == 240))
	set_clip(screen, 0, 20, 319, 219);
    else if ((SCREEN_W == 640) && (SCREEN_H == 480))
	set_clip(screen, 0, 40, 640, 439);
}


int set_desired_video_mode_or_fallback(void)
{
    int width, height;

    unprepare_scanlines();
    
    if (set_desired_video_mode() < 0) {
	if (get_desktop_resolution(&width, &height) == 0) {
	    /* Probably a windowed environment: choose 640x400 so we
	     * don't end up in a tiny window. */
	    desired_video_mode = VID_640x400_FULLSCREEN;
	    if (set_gfx_mode(GFX_SAFE, 640, 400, 0, 0) < 0)
		return -1;
	}
	else {
	    desired_video_mode = VID_320x200_FULLSCREEN;
	    if (set_gfx_mode(GFX_SAFE, 320, 200, 0, 0) < 0)
		return -1;
	}
    }
    
    clear_bitmap(screen);
    clip_to_size();
    prepare_scanlines();
    
    if (set_display_switch_mode(SWITCH_BACKAMNESIA) == -1)
	set_display_switch_mode(SWITCH_BACKGROUND);
    
    return 0;
}


inline void blit_to_screen_offset(BITMAP *buffer, int ox, int oy)
{
    if (scanlined_screen) {
	stretch_blit(buffer, scanlined_screen, 0, 0, 320, 200, ox, oy, 640, 200);
	return;
    }
    
    /* unscanlined */
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


void vidmode_init(void)
{
}


void vidmode_shutdown(void)
{
    unprepare_scanlines();
}
