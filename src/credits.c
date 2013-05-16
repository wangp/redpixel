/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Game credits.
 */


#include "a4aux.h"
#include "blood.h"
#include "colours.h"
#include "globals.h"
#include "main.h"
#include "options.h"
#include "version.h"
#include "vidmode.h"


#define FULLSCREEN	\
    "", "", "", "", "", "", "", "", "", "",	\
    "", "", "", "", "", "", "", "", "", "",	\
    "", "", ""

#define NL	""
#define BLANK2	"", ""
#define BLANK3	"", "", ""


static char *text[] = {
        FULLSCREEN,
	"rRED PIXEL ", NL,
	"VERSION " VERSION_STR " ", NL,
	"COPYRIGHT PSYK SOFTWARE " VERSION_YEAR,
	BLANK3,
	"rCODE + ADDITIONAL GRAPHICS",
	BLANK2,
	"PETER WANG",
	BLANK3,
	"rGAME GRAPHICS",
	BLANK2,
	"DAVID WANG",
	BLANK2,
	"`FRAGS TABLE' BACKDROP FOUND ON FTP.UFIES.ORG",
	BLANK3,
	"rINITIAL LINUX PORT", 
	BLANK2,
	"PETER SUNDLING",
	BLANK3,
	"rCPU OPPONENT CODE", 
	BLANK2,
	"ERNO KUUSELA",
	BLANK3,
	"rINTRO MUSIC", 
	BLANK2,
	"DAVID A.CAPELLO",
	BLANK3,
	"rSOUNDS STOLEN FROM",
	BLANK2,
	"               POSTAL   RUNNING WITH SCISSORS", NL,
	"                 DOOM   ID SOFTWARE          ", NL,
	"       MECHWARRIOR II   ACTIVISION           ", NL,
	"            CYBERDOGS   RONNY WESTER         ", NL,
	"             COMMANDO   ARNIE                ", NL,
	BLANK2,
	"rSPECIAL THANKS",
	BLANK2,
	"SHAWN HARGREAVES AND ALLEGRO CONTRIBUTORS",	NL,
	"GEORGE FOOT AND LIBNET CONTRIBUTORS",		NL,
	"DJ DELORIE AND DJGPP CONTRIBUTORS",		NL,
	"BRENNAN UNDERWOOD FOR FAST SQRT ROUTINES",	NL,
	"FREE SOFTWARE PEOPLE IN GENERAL",
	FULLSCREEN, "x"
};


static int keypressed_allow_fullscreen(void)
{
    if (keypressed()) {
	if (readkey() >> 8 == KEY_F11)
	    toggle_fullscreen_window();
	else
	    return TRUE;
    }
    return FALSE;
}


void credits(void)
{
    FONT *fnt;
    int h;
    double t0;
    int frame;
    int line = 0;
    int offset = 0;
    int theend = 0;
    int i;

    /* begin */
    clear_keybuf();
    rp_hide_mouse();

    clear_bitmap(screen);
    clear_bitmap(dbuf);

    fnt = dat[MINI].dat;
    h = text_height(fnt);
    t0 = al_get_time();
    frame = 0;

    while (!keypressed_allow_fullscreen() && !mouse_b && !theend) {
	if (--offset < 0) {
	    offset = h - 1;
	    line++;
	}

	clear_bitmap(dbuf);
	for (i = -1; i < 200 / h; i++) {
	    if (!theend) {
		if (text[line+i][0] == 'x')
		    theend = 1;
		else if (text[line+i][0] == 'r')
		    textout_centre_ex(dbuf, fnt, text[line+i]+1, 160, i*h + offset, RED, -1);
		else
		    textout_centre_ex(dbuf, fnt, text[line+i], 160, i*h + offset, WHITE, -1);
	    }
	}
	
	blit_to_screen(dbuf);

	frame++;
	al_rest(t0 + frame*1/25.0 - al_get_time());
    }

    rp_fade_out(dbuf, 6);

    while (key[KEY_ESC] || mouse_b)
	;
}
