/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Game introduction.
 *  Code converted from a SeeR script.
 */


#include <allegro.h>
#include "blood.h"
#include "globals.h"
#include "scrblit.h"


static int raster_words(char *s)
{
    BITMAP *txt1;
    BITMAP *txt2;
    int w, h;
    int x, y;
    int nopress = 0;

    w = text_length(dat[UNREAL].dat, s);
    h = text_height(dat[UNREAL].dat);
    if ((w % 2) == 0) w++;

    txt1 = create_bitmap(w, h);
    txt2 = create_bitmap(w, h);
    clear_bitmap(txt1);
    clear_bitmap(txt2);
    textout(txt1, dat[UNREAL].dat, s, 0, 0, -1);
    textout(txt2, dat[UNREAL].dat, s, 0, 0, -1);
    for (x = 0; x < h; x += 2) {
	hline(txt1, 0, x,     w - 1, 0);
	hline(txt2, 0, x + 1, w - 1, 0);
    }

    x = -w;
    y = 100 - h / 2;

    do {
	clear_bitmap(dbuf);
	draw_sprite(dbuf, txt1, x, y);
	draw_sprite(dbuf, txt2, 319 - x - w, y);
	blit_to_screen(dbuf);
	x++;
	if (keypressed())
	    goto quit;
    } while (x < (160 - w / 2));

    rest(500);

    do { 
	clear_bitmap(dbuf);
	draw_sprite(dbuf, txt1, x, y);
	draw_sprite(dbuf, txt2, 319 - x - w, y);
	blit_to_screen(dbuf);
	x++;
	if (keypressed())
	    goto quit;
    } while (x < 320);

    nopress = 1;

  quit:
    
    destroy_bitmap(txt1);
    destroy_bitmap(txt2);

    return nopress;
}


static int scan(int x, int y)
{
    BITMAP *bmp;
    int x2, i, j = 0;
    x2 = x + 60;

    bmp = create_bitmap(320, 200);
    clear_bitmap(bmp);

    do {
	j = (j) ? 0 : 1;

	for (i = j; i < 80; i += 2)
	    blit(dat[TITLE].dat, bmp, x, y + i, 120, 60 + i, 80, 1);

	blit_to_screen(bmp);
	x++;
	rest(60);
    } while ((x < x2) && (!keypressed()));

    destroy_bitmap(bmp);

    return !(x < x2);
}


void intro()
{
    int x, y, x2, y2;

    /* Give monitor a chance to set the mode.  I'm serious.  
     * Only important because we want to see the year scroll in.  */
    rest(300);
    
    clear_keybuf();

    if (raster_words("1998")
	&& scan(10, 100) 
	&& raster_words("PSYK SOFTWARE") 
	&& scan(160, 90)
	&& raster_words("PRESENTS")) {

	x = 159; x2 = 160;
	y = 99;  y2 = 100;

	do {
	    clear_bitmap(dbuf);
	    blit(dat[TITLE].dat, dbuf, x, y, x, y, x2-x, y2-y);
	    blit_to_screen(dbuf);

	    x--; x2++;
	    y--; y2++;
	} while (x > 0);

	rest(1400);
    }

    fade_out(6);
    clear_bitmap(screen);
    set_palette(dat[GAMEPAL].dat);

    clear_keybuf();
}
