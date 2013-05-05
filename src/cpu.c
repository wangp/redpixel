/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Simple AI opponent.
 *  By Erno Kuusela, 29.11.2000.
 */


#include <math.h>
#include "a4aux.h"
#include "blood.h"
#include "cpu.h"
#include "gameloop.h"
#include "player.h"
#include "rnd.h"
#include "tiles.h"


void cpu_head_for_player();
void cpu_do_random_jump();
void cpu_climb_down_ladders();


int cpu_num;
int cpu_agressiveness = 3;	/* 1-5, 1 highest */
char *cpu_angst_str[5] = { "PSYKO!!", "ANGRY", "NORMAL", "LAME", "SHEEP" };


void do_cpu(void)
{
    int surround = 0;
    players[cpu_num].right = FALSE;
    players[cpu_num].left = FALSE;
    players[cpu_num].down = FALSE;
    players[cpu_num].up = FALSE;

    players[cpu_num].fire = FALSE;
    players[cpu_num].drop_mine = 0;
    players[cpu_num].select = 0;

    if (players[local].health <= 0)
	return;

    players[cpu_num].angle = find_angle(players[cpu_num].x - px + 3,
					players[cpu_num].y - py + 4,
					players[local].x - px + 3,
					players[local].y - py + 4);

    if (!players[cpu_num].health)
	players[cpu_num].respawn = TRUE;
    else
	players[cpu_num].respawn = FALSE;

    if (players[local].x > players[cpu_num].x)
	players[cpu_num].facing = right;
    else
	players[cpu_num].facing = left;

    if (cpu_agressiveness < 4) {
	if (players[cpu_num].have[w_bow])
	    players[cpu_num].cur_weap = w_bow;
	if (players[cpu_num].have[w_bazooka])
	    players[cpu_num].cur_weap = w_bazooka;
    }

    if (!players[cpu_num].next_fire && ((rnd() % cpu_agressiveness) == 0)) {
	int dx = players[cpu_num].x - players[local].x;
	int dy = players[cpu_num].y - players[local].y;
	int dist;
	int r;

	dist = hypot(abs(dx), abs(dy));
	if (dist <= 160) {
	    int tx = players[cpu_num].x + 3, ty = players[cpu_num].y + 4;

	    players[cpu_num].fire = TRUE;

	    /* check if there's wall between cpu and local */
	    for (r = 0; r < dist;
		 r += ((dist / 16) <= 0) ? 1 : (dist / 16)) {
		tx += (dx / 16);
		ty += (dy / 16);
		if (tile_collide_p(tx, ty))
		    players[cpu_num].fire = FALSE;
	    }

	    /* little hack. That checking algorithm doesn't work on
               tiles under cpu (why??) */
	    if (tile_collide_p(players[cpu_num].x + 3, players[cpu_num].y + 20)
		&& players[local].y < players[cpu_num].y
		&& (players[cpu_num].cur_weap == w_bazooka
		    || players[cpu_num].cur_weap == w_bow))
		players[cpu_num].fire = FALSE;
	}
    }
    
    /* check if tile at right side */
    if (tile_collide_p(players[cpu_num].x + 19, players[cpu_num].y + 8))
	surround |= 1;

    /* left side */
    if (tile_collide_p(players[cpu_num].x - 3, players[cpu_num].y + 8))
	surround |= 2;

    switch (surround) {
	case 0: /* seek and destroy */
	    if ((rnd() % cpu_agressiveness) == 0) {
		cpu_head_for_player();
		if (players[local].y < players[cpu_num].y) {
		    /* haa - ladders */
		    cpu_climb_down_ladders();
		    if (tile_collide_p(players[cpu_num].x + 3, players[cpu_num].y + 20) != T_LAD)
			cpu_do_random_jump();
		}
	    }
	    break;

	case 1:
	case 2:
	    players[cpu_num].up = TRUE;
	    cpu_head_for_player();
	    break;

	case 3: /* eek, trapped! Jump! */
	    cpu_do_random_jump();
	    break;
    }
}


void cpu_head_for_player(void)
{
    if (players[local].x > players[cpu_num].x) {
	if ((cpu_agressiveness == 5) && (rnd() % 3 == 0)) {	/* baa - please don't kill me! */
	    players[cpu_num].right = FALSE;
	    players[cpu_num].left = TRUE;
	    if (players[cpu_num].num_mines > 0)
		players[cpu_num].drop_mine = TRUE;
	}
	else {
	    players[cpu_num].right = TRUE;
	    players[cpu_num].left = FALSE;
	}
    }
    else {
	if ((cpu_agressiveness == 5) && ((rnd() % 3) == 0)) {
	    players[cpu_num].right = TRUE;
	    players[cpu_num].left = FALSE;
	    if (players[cpu_num].num_mines > 0)
		players[cpu_num].drop_mine = TRUE;
	}
	else {
	    players[cpu_num].right = FALSE;
	    players[cpu_num].left = TRUE;
	}
    }
}


void cpu_do_random_jump(void)
{
    if ((rnd() % cpu_agressiveness) != 0)
	return;

    players[cpu_num].up = TRUE;
    if ((rnd() % 4) > 1)
	players[cpu_num].left = TRUE;
    else
	players[cpu_num].right = TRUE;
}


void cpu_climb_down_ladders(void)
{
    if (tile_collide_p(players[cpu_num].x + 3, players[cpu_num].y + 20) == T_LAD) {
	players[cpu_num].down = TRUE;
	players[cpu_num].up = FALSE;
	players[cpu_num].right = FALSE;
	players[cpu_num].left = FALSE;
    }
}


void cpu_climb_up_ladders(void)
{
    if (tile_collide_p(players[cpu_num].x + 3, players[cpu_num].y - 4) == T_LAD) {
	players[cpu_num].up = TRUE;
	players[cpu_num].down = FALSE;
	players[cpu_num].right = FALSE;
	players[cpu_num].left = FALSE;
    }
}
