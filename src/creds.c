/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 * 
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 * 
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Email: tjaden@psynet.net
 *  WWW:   http://www.psynet.net/tjaden/
 * 
 *  Game credits.
 */


#include <allegro.h>
#include "common.h"
#include "run.h"
#include "blood.h"


char *text[] =
{
    "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "",
    "rRED PIXEL",
    "",
    "",
    "COPYRIGHT PSYK SOFTWARE " VERSION_YEAR,
#if 0    
    "ALL RIGHTS RESERVED",		       /* what was I thinking? */
#else
    "LICENSED UNDER GNU LGPL", 
#endif
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
    "``FRAGS TABLE'' BACKDROP FOUND ON FTP.UFIES.ORG",
    "",
    "",
    "",
    "rSOUNDS STOLEN FROM",
    "",
    "",
    "POSTAL (RUNNING WITH SCISSORS)",
    "",
    "DOOM (ID SOFTWARE)",
    "",
    "MECHWARRIOR II (ACTIVISION)",
    "",
    "CYBERDOGS (RONNY WESTER)",
    "",
    "COMMANDO (ARNIE)",
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
    //"",
    //"PRZEMYSLAW PODSIADLY FOR SEER",
    "",
    "BRENNAN UNDERWOOD FOR FAST SQRT ROUTINES",
    "", "", "", "", "", "", "", "", "", "",                    /* :) */
    "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "",
    "x"
};


void credits()
{
    int line = 0;
    int offset = 0, h;
    int theend = 0;
    int i;

    clear_keybuf();
    show_mouse(NULL);

    h = text_height(dat[MINI].dat);

    while (!keypressed() && !mouse_b && !theend)
    {
	if (--offset<0)
	{
	    offset = h;
	    line++;
	}

	// draw me
	clear(dbuf);
	for (i=-1; i<200/h; i++)
	{
	    if (!theend)
	    {
		if (text[line+i][0]=='x')
		    theend = 1;
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
