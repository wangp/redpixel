/* 
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  In-game messages.
 */


#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "a4aux.h"
#include "anim.h"
#include "blood.h"
#include "engine.h"
#include "colours.h"
#include "gameloop.h"
#include "globals.h"


#define MAX_MSGS    5
#define MSG_LEN     40

static char msg_queue[MAX_MSGS][MSG_LEN];
static int num_msgs, msg_tics;


static void scroll(void)
{
    int i;
    
    for (i = 0; i < MAX_MSGS - 1; i++)
	memcpy(msg_queue[i], msg_queue[i + 1], MSG_LEN);

    num_msgs--;
}


void add_msg(char *s, int to_player)
{
    if (to_player != local && to_player != -1)
	return;

    if (!num_msgs)
	msg_tics = MSG_ANIM;

    if (num_msgs == MAX_MSGS) 
	scroll();
     
    strcpy(msg_queue[num_msgs++], s);
}


void add_msgf(int target, char *fmt, ...)
{
    char buf[200];
    va_list va;
   
    va_start(va, fmt);
    vsprintf(buf, fmt, va);
    va_end(va);
    
    add_msg(buf, target);
}


void update_msgs(void)
{
    if (--msg_tics < 1) {
	if (num_msgs) scroll();
	msg_tics = MSG_ANIM;
    }
}


void draw_msgs(void)
{
    int i, len;

    for (i = 0; i < num_msgs; i++) {
	len = text_length(dat[MINI].dat, msg_queue[i]);
	
	textout_ex(dbuf, dat[MINI].dat, msg_queue[i],
		   320 - 16 - len, 3 + 7 * i,
		   GREEN - (num_msgs * 2) + (i * 2), -1);
    }
}


void reset_msgs(void)
{
    num_msgs = 0;
}
