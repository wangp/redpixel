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
 *  Demo recording and playback helpers.
 */


#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <allegro.h>
#include "demo.h"
#include "run.h"
#include "stats.h"
#include "statlist.h"
#include "setweaps.h"


char demo_description[128] = "";


static PACKFILE *fp;

#define RECORDING	1
#define PLAYING		2

static int mode = 0;

static char header[] = { '.', 'r', 'p', 'd', 'e', 'm', 'o', '\0' };


/*----------------------------------------------------------------------
 * 
 *	Demo reading
 * 
 *----------------------------------------------------------------------*/

static int check_magic(PACKFILE *f)
{
    char test[sizeof header];
    pack_fread(test, sizeof test, f);
    return !memcmp(header, test, sizeof header);
}


int demo_read_open(char *filename)
{
    int ver;
    
    fp = pack_fopen(filename, F_READ_PACKED);
    if (fp && check_magic(fp)) {
	
	ver = pack_getc(fp);
	if (ver == 1) {
	    mode = PLAYING;
	    return 0;
	}
    }
    
    if (fp) pack_fclose(fp);
    fp = 0;
    return -1;
}


static int unsave_stat(STAT_VAR *sv)
{
    *((int *)sv->p) = pack_igetl(fp);
    return 0;
}

void demo_read_header()
{
    int i;

    /* Not a good place for stuff.  */

    /* player information  */
    num_players = pack_getc(fp);
    
    for (i = 0; i < num_players; i++) 
	demo_read_string(players[i].name, 40);

    /* restore stats used in demo recording  */
    for_every_stat(stat_block, unsave_stat);
    set_weapon_stats();
    
    /* description  */
    demo_read_string(demo_description, sizeof demo_description);
}


long demo_read_seed()
{
    return pack_igetl(fp);    
}


int demo_read_next_packet_type()
{
    return pack_getc(fp);
}


void demo_read_packet(void *packet, int len)
{
    pack_fread(packet, len, fp);
}


void demo_read_string(char *filename, int len)
{
    int i, c;
    
    for (i = 0; i < len-1; i++) {
	c = pack_getc(fp);
	if (!c) break;
	
	filename[i] = c;
    }
    
    filename[i] = 0;
}


void demo_read_close()
{
    if (fp) pack_fclose(fp);
    fp = NULL, mode = 0;
}



/*----------------------------------------------------------------------
 * 
 *	Demo writing
 * 
 *----------------------------------------------------------------------*/

static int save_stat(STAT_VAR *sv)
{
    pack_iputl(*(int *)sv->p, fp);
    return 0;
}

int demo_write_open(char *filename, int players, char *names[])
{
    int i;
    
    fp = pack_fopen(filename, F_WRITE_PACKED);
    if (!fp) return -1;
    
    /* Write header information.  */
    pack_fwrite(header, sizeof header, fp);   	/* magic */
    pack_putc(1, fp);				/* version number */

    /* player information  */
    pack_putc(players, fp);
    
    for (i = 0; i < players; i++) {
	pack_fputs(names[i], fp);
	pack_putc(0, fp);
    }

    /* save current stats */
    for_every_stat(stat_block, save_stat);
    
    /* description */
    {
	char d[sizeof demo_description];
	time_t t = time(0);
	
	strcpy(d, ctime(&t));
	d[strlen(d) - 1] = 0;	/* remove \n */
	strupr(d);
	
	pack_fputs(d, fp);
	pack_putc(0, fp);
    }
    
    mode = RECORDING;
    return 0;
}


int demo_is_recording()
{
    return (mode == RECORDING);
}


void demo_write_close()
{
    if (!fp) return;
    
    pack_putc(DEMO_END, fp);
    pack_fclose(fp);
    fp = NULL, mode = 0;    
}


void demo_write_set_rng_seed(long seed)
{
    if (!fp) return;
    
    pack_iputl(seed, fp);
}


void demo_write_change_map(char *filename)
{
    if (!fp) return;

    pack_putc(DEMO_MAP_CHANGE, fp);
    pack_fputs(filename, fp);
    pack_putc(0, fp);
}


void demo_write_frame_data(void *packet, int len)
{
    if (!fp) return;
    
    pack_putc(DEMO_FRAME_DATA, fp);
    pack_fwrite(packet, len, fp);
}



/*----------------------------------------------------------------------
 * 
 *	Make unique demo filename
 * 
 *----------------------------------------------------------------------*/

char *new_demo_filename(char *ppath)
{
    static char path[1024];
    int i;
    
    for (i = 0; i <= 9999; i++) {
	sprintf(path, "%s/demo%04d.rec", ppath, i);
	if (!exists(path)) 
	    return path;
    }
    
    /* Huh!  */
    return 0;
}
