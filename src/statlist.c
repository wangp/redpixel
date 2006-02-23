/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
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
    { NULL, ST_INT, NULL }
};


/* Used for pushing stat_block onto.  */
static STAT_VAR _stat_block[] = {
#define XXX(a) { #a, ST_INT, &_st_##a }, 
#include "statlist.inc"
#undef XXX    
    { NULL, ST_INT, NULL }
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
