/* 
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 * 
 *  Highly-specialised filtered blit.
 */


#include <allegro.h>
#include "globals.h"


static unsigned char rval[256], gval[256], bval[256];


void fblit(BITMAP *src, BITMAP *dest)
{
    int x, y;
    int c1, c3, c4, c5, c7;
    
    /*
     * 		c0 c1 c2	
     *		c3 c4 c5
     *		c6 c7 c8
     *	    (make sense now?)
     */

    #define getpixel(src, x, y)	(src->line[y][x])
    #define getr8(x)		(rval[x])
    #define getg8(x)		(gval[x])
    #define getb8(x)		(bval[x])
    #define makecol8(r, g, b)	(rgb_table.data[r >> 3][g >> 3][b >> 3])
    
    for (y = src->h - 2; y > 0; y--) {
	for (x = src->w - 2; x > 0; x--) {

	    c1 = getpixel(src, x  , y-1);
	    c3 = getpixel(src, x-1, y  );
	    c4 = getpixel(src, x  , y  );
	    c5 = getpixel(src, x+1, y  );
	    c7 = getpixel(src, x  , y+1);

	    dest->line[y][x] = makecol8(/* red */
					(((getr8(c1) + getr8(c3)
					   + getr8(c5) + getr8(c7)) >> 3) 
					 + (getr8(c4) >> 1)),
			      
					/* green */
					(((getg8(c1) + getg8(c3) 
					   + getg8(c5) + getg8(c7)) >> 3)
					 + (getg8(c4) >> 1)),
			      
					/* blue */
					(((getb8(c1) + getb8(c3) 
					   + getb8(c5) + getb8(c7)) >> 3)
					 + (getb8(c4) >> 1)));
	}
    }
    
    #undef getpixel getr8 getg8 getb8 makecol8
}


void fblit_init(RGB *pal)
{
    int i;
    
    select_palette(pal);
    
    for (i = 0; i < 256; i++) {
	rval[i] = getr(i);
	gval[i] = getg(i);
	bval[i] = getb(i);
    }    
    
    unselect_palette();
}
