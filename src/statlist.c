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
 *  List of stats.
 */


#include "stats.h"

#ifndef NULL
#define NULL 0
#endif

#define XXX(x) int st_##x;	static int _st_##x;
#include "statlist.inc"
#undef XXX

STAT_VAR stat_block[] = {
#define XXX(a) { #a, ST_INT, &st_##a }, 
#include "statlist.inc"
#undef XXX    
    { NULL }
};


/* Used for pushing stat_block onto.  */
static STAT_VAR _stat_block[] = {
#define XXX(a) { #a, ST_INT, &_st_##a }, 
#include "statlist.inc"
#undef XXX    
    { NULL }
};


static void copy_block(STAT_VAR *dest, STAT_VAR *src)
{
    int i;
    
    for (i = 0; src[i].id; i++)
	*((int *)dest[i].p) = *((int *)src[i].p);
}

void push_stat_block() 
{
    copy_block(_stat_block, stat_block);
}

void pop_stat_block()
{
    copy_block(stat_block, _stat_block);
}
