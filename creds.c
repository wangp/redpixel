#include <allegro.h>
#include "common.h"
#include "blood.h"


extern BITMAP *dbuf;
extern DATAFILE *dat;


char *text[] =
{
"", "", "", "", "", "", "", "", "", "",
"", "", "", "", "", "", "", "", "", "",
"", "", "", "", "", "", "",
"rRED PIXEL",
"",
"",
"COPYRIGHT PSYK SOFTWARE 1998",
"ALL RIGHTS RESERVED",
"",
"",
"",
"rCODE + ADDITIONAL GRAPHICS",
"",
"",
"PETER WANG",
"",
"",
"",
"rGAME GRAPHICS",
"",
"",
"DAVID WANG",
"",
"",
"",
"rSOUNDS",
"",
"",
"STOLEN FROM POSTAL",
"(BY RUNNING WITH SCISSORS)",
"",
"",
"",
"rSPECIAL THANKS",
"",
"",
"DJ DELORIE FOR DJGPP",
"",
"SHAWN HARGREAVES FOR ALLEGRO",
"",
"ANDRE LAMOTHE FOR BASIS OF SERIAL CODE",
"",
"JOHN MCCAWLEY FOR JONIPX",
"",
"PRZEMYSLAW PODSIADLY FOR SEER",
"", "", "", "", "", "", "", "", "", "",
"", "", "", "", "", "", "", "", "", "",
"", "", "", "", "", "", "",
"x"
};

void credits()
{
    int line = 0;
    int offset = 0, h;
    int out = 0;
    int i;

    clear_keybuf();
    show_mouse(NULL);

    h = text_height(dat[MINI].dat);
    while (!keypressed() && !mouse_b && !out)
    {
	if (--offset<0)
	{
	    offset = h;
	    line++;
	}

	clear(dbuf);
	for (i=-1; i<200/h; i++)
	{
	    if (!out)
	    {
		if (text[line+i][0]=='x')
		    out = 1;
		else if (text[line+i][0]=='r')
		    textout_centre(dbuf, dat[MINI].dat, text[line+i]+1, 160, i*h + offset, RED);
		else
		    textout_centre(dbuf, dat[MINI].dat, text[line+i], 160, i*h + offset, WHITE);
	    }
	}
	blit(dbuf, screen, 0, 0, 0, 0, 320, 200);

	rest(40);
    }

    fade_out(6);
    clear(screen);
    set_palette(dat[GAMEPAL].dat);

    while (key[KEY_ESC] || mouse_b);
}