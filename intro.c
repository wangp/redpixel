// converted from a seer script

#include <allegro.h>
#include "blood.h"

extern DATAFILE *dat;
extern BITMAP *dbuf;

int raster_words(char *s)
{
    BITMAP *txt1;
    BITMAP *txt2;
    int w, h;
    int x, y;

    w = text_length(dat[UNREAL].dat, s);
    h = text_height(dat[UNREAL].dat);
    if (w%2==0) w++;

    txt1 = create_bitmap(w, h);
    txt2 = create_bitmap(w, h);
    clear(txt1);
    clear(txt2);
    textout(txt1, dat[UNREAL].dat, s, 0, 0, -1);
    textout(txt2, dat[UNREAL].dat, s, 0, 0, -1);
    for (x=0; x<h; x+=2) {
	hline(txt1, 0, x, w-1, 0);
	hline(txt2, 0, x+1, w-1, 0);
    }

    x = -w;
    y = 100-h/2;

    do
    {
	clear(dbuf);
	draw_sprite(dbuf, txt1, x, y);
	draw_sprite(dbuf, txt2, 319-x-w, y);
	blit(dbuf, screen, 0, 0, 0, 0, 320, 200);
	x++;
	if (keypressed())
	    goto die;
    } while (x < 160-w/2);

    rest(500);

    do
    {
	clear(dbuf);
	draw_sprite(dbuf, txt1, x, y);
	draw_sprite(dbuf, txt2, 319-x-w, y);
	blit(dbuf, screen, 0, 0, 0, 0, 320, 200);
	x++;
	if (keypressed())
	    goto die;
    } while (x<320);

    die:    // gotos.. nooo!!
    destroy_bitmap(txt1);
    destroy_bitmap(txt2);
    if (keypressed())
	return 0;
    return 1;
}


int scan(int x, int y)
{
    int x2;
    x2 = x + 60;

    do
    {
	clear(dbuf);
	blit(dat[TITLE].dat, dbuf, x, y, 120, 60, 80, 80);
	blit(dbuf, screen, 0, 0, 0, 0, 320, 200);
	x++;
	rest(60);
	if (keypressed())
	    return 0;
    } while (x < x2);

    return 1;
}


void intro()
{
    int x, y, x2, y2;

    clear_keybuf();

    if (raster_words("1998") &&
	scan(10, 100) &&
	raster_words("PSYK SOFTWARE") &&
	scan(160, 90) &&
	raster_words("PRESENTS"))
    {
	x = 159; x2 = 160;
	y = 99;  y2 = 100;

	do
	{
	    clear(dbuf);
	    blit(dat[TITLE].dat, dbuf, x, y, x, y, x2-x, y2-y);
	    blit(dbuf, screen, 0, 0, 0, 0, 320, 200);

	    x--; x2++;
	    y--; y2++;
	}
	while (x>0);

	rest(1400);
    }

    fade_out(4);
    clear(screen);
    set_palette(dat[GAMEPAL].dat);

    clear_keybuf();
}