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
#include "fblit.h"
#include "main.h"
#include "options.h"
#include "version.h"
#include "vidmode.h"


static volatile int timer;

static void timer_func(void)
{
    timer++;
} 


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
	"JEFFERY GUAN FOO WAH FOR JGMOD",		NL,
	"DJ DELORIE AND DJGPP CONTRIBUTORS",		NL,
	"BRENNAN UNDERWOOD FOR FAST SQRT ROUTINES",	NL,
	"FREE SOFTWARE PEOPLE IN GENERAL",
	FULLSCREEN, "x"
};


void credits(void)
{
    BITMAP *ftmp, *bmp;
    int line = 0;
    int offset = 0, h;
    int theend = 0;
    int i;

    /* preparation */
    ftmp = (filtered) ? create_bitmap(dbuf->w, dbuf->h) : 0;
    bmp  = (ftmp) ? ftmp : dbuf;
    
    LOCK_VARIABLE(timer);
    LOCK_FUNCTION(timer_func);
    install_int(timer_func, 40);

    /* begin */
    clear_keybuf();
    show_mouse(NULL);

    clear_bitmap(screen);
    clear_bitmap(dbuf);

    h = text_height(dat[MINI].dat);

    while (!keypressed() && !mouse_b && !theend) {
	timer = 0;
	
	if (--offset < 0) {
	    offset = h;
	    line++;
	}

	clear_bitmap(bmp);
	for (i = -1; i < 200 / h; i++) {
	    if (!theend) {
		if (text[line+i][0] == 'x')
		    theend = 1;
		else if (text[line+i][0] == 'r')
		    textout_centre_ex(bmp, dat[MINI].dat, text[line+i]+1, 160, i*h + offset, RED, -1);
		else
		    textout_centre_ex(bmp, dat[MINI].dat, text[line+i], 160, i*h + offset, WHITE, -1);
	    }
	}
	
	if (ftmp) 
	    fblit(ftmp, dbuf);
	blit_to_screen(dbuf);

	while (timer == 0)
	    rest(0);
    }

    rp_fade_out(dbuf, 6);

    while (key[KEY_ESC] || mouse_b)
	;

    /* clean up */
    remove_int(timer_func);
    
    if (ftmp)
	destroy_bitmap(ftmp);
}
