/* 
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Game loop (including rendering)
 */


#include <stdio.h>
#include "a4aux.h"
#include "gameloop.h"
#include "anim.h"
#include "blood.h"
#include "colours.h"
#include "cpu.h"
#include "demo.h"
#include "engine.h"
#include "globals.h"
#include "input.h"
#include "main.h"
#include "map.h"
#include "options.h"
#include "packet.h"
#include "player.h"
#include "rnd.h"
#include "sk.h"
#include "vidmode.h"

#include "backpack.h"
#include "blod.h"
#include "bullet.h"
#include "corpse.h"
#include "explo.h"
#include "message.h"
#include "mine.h"
#include "particle.h"
#include "tiles.h"


#define AMBIENT_LIGHT   16


comm_t comm;

int local;
int mx, my, offsetx, offsety, px, py;		

int shake_factor;
static int shakex, shakey;

static int heart_frame, heart_anim;
static int blood_frame, blood_anim;

static int show_fps = 0;


/*----------------------------------------------------------------------*/


static volatile int kb_enter;
static volatile int kb_f11;
static volatile int kb_f12;
static volatile int kb_f2;
static volatile int kb_f3;
static volatile int kb_p;
static volatile int want_quit;
static volatile int want_help, helping;

static void my_keyboard_lowlevel_callback(int scancode)
{
    int sc = scancode & 0x7f;
    int pressed = !(scancode & 0x80);
    switch (sc) {
	case KEY_ENTER: kb_enter = pressed; break;
        case KEY_F11: kb_f11 = pressed; break;
        case KEY_F12: kb_f12 = pressed; break;
	case KEY_F2: kb_f2 = pressed; break;
	case KEY_F3: kb_f3 = pressed; break;
	case KEY_P: kb_p = pressed; break;
        case KEY_ESC:
	    if ((pressed) && (!helping))
		want_quit = 1;
	    break;
        case KEY_F1:
	    want_help = pressed;
	    break;
        default:
	    break;
    }
}


static void install_my_keyboard_lowlevel_callback(void)
{
    static int first = 1;
    if (first) {
	LOCK_VARIABLE(kb_enter);
	LOCK_VARIABLE(kb_f12);
	LOCK_VARIABLE(kb_p);
	LOCK_VARIABLE(want_quit);
	LOCK_VARIABLE(want_help);
	LOCK_VARIABLE(helping);
	LOCK_FUNCTION(my_keyboard_lowlevel_callback);
	first = 0;
    }
    kb_enter = kb_f11 = kb_f12 = kb_p = 0;
    want_quit = 0;
    want_help = helping = 0;
    keyboard_lowlevel_callback = my_keyboard_lowlevel_callback;
}


static void uninstall_my_keyboard_lowlevel_callback(void)
{
    keyboard_lowlevel_callback = NULL;
}


/*----------------------------------------------------------------------*/


static void draw_spotlight(void)
{
    int u = 0, v = 0;

    if ((players[local].fire_frame || players[local].firing)
	&& (players[local].cur_weap != w_knife)
	&& (players[local].cur_weap != w_bow)
	&& (players[local].health)) {
	u = (rnd() % 10) - 5;
	v = (rnd() % 10) - 5;
    }

    /* 'light' has 5 pixel margin on all sides. */
    al_set_target_bitmap(dbuf->real);
    al_set_blender(ALLEGRO_ADD, ALLEGRO_DST_COLOR, ALLEGRO_ZERO);
    al_draw_bitmap(light, -5 + u, -5 + v, 0);
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
}


static void calc(void)
{
    /* find top-left */
    mx = players[local].x / 16 - 10;
    my = players[local].y / 16 - 6;

    offsetx = players[local].x % 16;
    offsety = players[local].y % 16;

    if (mx < 0) mx = offsetx = 0;
    if (my < 0) my = offsety = 0;

    if (mx >= map.w - 20) {
	mx = map.w - 20;
	offsetx = 0;
    }

    if ((my > map.h - 13) || ((my == map.h - 13) && (offsety >= 8))) {
	my = map.h - 13;
	offsety = 8;
    }

    px = mx * 16 + offsetx;
    py = my * 16 + offsety;
}

/*----------------------------------------------------------------------*/

typedef struct {
    int frags;
    int no;
} TEMPSORT;

static TEMPSORT tempbuf[MAX_PLAYERS];

static int frag_sorter(const void *e1, const void *e2)
{
    return ((TEMPSORT *) e2)->frags - ((TEMPSORT *) e1)->frags;
}

/*----------------------------------------------------------------------*/

static void draw_status(void)
{
    int y, i;
    int ammo;
    char buf[10];

    if (show_fps)
	textprintf_ex(dbuf, dat[MINI].dat, 270, 80, WHITE, -1, "FPS: %2d", last_fps);

    /* list scores */
    if (key[KEY_TAB]) {
	textout_ex(dbuf, dat[UNREAL].dat, "FRAG COUNT", 55, 5, -1, -1);

	for (i = 0; i < MAX_PLAYERS; i++) {
	    tempbuf[i].frags = players[i].frags;
	    tempbuf[i].no = i;
	}

	qsort(tempbuf, MAX_PLAYERS, sizeof(TEMPSORT), frag_sorter);

	y = 30;
	for (i = 0; i < MAX_PLAYERS; i++) {
	    if (players[tempbuf[i].no].exist) {
		textprintf_ex(dbuf, dat[UNREAL].dat, 55, y, -1, -1,
			      "%3d %s", players[tempbuf[i].no].frags,
			      players[tempbuf[i].no].name);
		y += 20;
	    }
	}
	return;
    }

    /* dead, don't continue */
    if ((comm != demo) && (players[local].health == 0)) {
	textout_ex(dbuf, dat[UNREAL].dat, "PRESS SPACE TO RESPAWN",
		   20, 200 - 30, -1, -1);
	return;
    }

    i = 0;
    y = 200 - 6 - 16;
    while (weapon_order[i].pic) {
	if (players[local].have[weapon_order[i].weap]) {
	    draw_sprite(dbuf, dat[weapon_order[i].pic].dat,
			6 + (players[local].cur_weap == weapon_order[i].weap ? 6 : 0), y);

	    textout_ex(dbuf, dat[MINI].dat, weapon_order[i].s, 0, y + 8, RED, -1);

	    ammo = num_ammo(local, weapon_order[i].weap);
	    if (ammo != -1) {
		sprintf(buf, "%d", ammo);
		textout_ex(dbuf, dat[MINI].dat, buf, 24, y + 8, YELLOW, -1);
		if (weapon_order[i].weap == players[local].cur_weap)
		    textout_ex(dbuf, dat[UNREAL].dat, buf, 320 - 48, 200 - 24, -1, -1);
	    }

	    y -= 14;
	}

	i++;
    }

    /* light amp powerup */
    if (players[local].visor_tics)
	draw_sprite(dbuf, dat[A_GOGGLES + (heart_frame > 2 ? 1 : heart_frame)].dat, 152, 9);

    /* bloodlust powerup */
    if (players[local].blood_tics)
	draw_sprite(dbuf, dat[A_BLOODLUST + blood_frame].dat, 152 + 18, 9);

    /* scanner powerup */
    if (players[local].scanner_tics)
	draw_sprite(dbuf, dat[A_SCANNER].dat, 152 - 18, 9);

    /* health and armour */
    draw_sprite(dbuf, dat[HEART1 + (heart_frame > 2 ? 1 : heart_frame)].dat, 6, 9);
    draw_sprite(dbuf, dat[A_ARMOUR].dat, 60, 9);
    textprintf_ex(dbuf, dat[UNREAL].dat, 24, 3, -1, -1, "%3d", players[local].health);
    textprintf_ex(dbuf, dat[UNREAL].dat, 78, 3, -1, -1, "%3d", players[local].armour);

    /* if playing demo, show currently viewed player */
    if (comm == demo)
	textout_centre_ex(dbuf, dat[MINI].dat, players[local].name, 320/2, 200-10, GREEN, -1);
}


static void render(void)
{
    int bx, by;

    calc();

    /* tile backdrop */
    bx = -(px % 640) / 2;
    by = -(py % 400) / 2;

    /* XXX this was not necessary before; probably a bug in allegro4-to-5 */
    clear_bitmap(dbuf);

    blit(dat[BACKDROP].dat, dbuf, 0, 0, bx, by, 320, 200);
    blit(dat[BACKDROP].dat, dbuf, 0, 0, bx, by + 200, 320, 200);
    blit(dat[BACKDROP].dat, dbuf, 0, 0, bx + 320, by, 320, 200);
    blit(dat[BACKDROP].dat, dbuf, 0, 0, bx + 320, by + 200, 320, 200);

    /* ready light */
    al_set_target_bitmap(light);
    al_clear_to_color(al_map_rgb(AMBIENT_LIGHT, AMBIENT_LIGHT, AMBIENT_LIGHT));

    /* draw */
    draw_mines();
    draw_tiles_and_stuff();
    draw_backpacks();
    draw_bullets();
    draw_players();
    draw_particles();
    draw_blods();
    draw_explo();
    draw_corpses();

    if (!players[local].health ||
        !players[local].visor_tics ||
        ((players[local].visor_tics < (GAME_SPEED * 3) &&
         (players[local].visor_tics % 2) == 0)))
	draw_spotlight();

    if ((players[local].scanner_tics) && (players[local].health > 0)) {
	/* in the last five seconds it screws up */
	draw_scanner((players[local].scanner_tics < GAME_SPEED * 5));
    }

    draw_status();
    draw_msgs();

    show_mouse(NULL);
    blit_to_screen_offset(dbuf, shakex, shakey);
    if (players[local].health && (comm != demo))
	show_mouse(screen);

    frame_counter++;
}


void game_loop(void)
{
    int frames_dropped;
    int update;
    int i;
    
    install_my_keyboard_lowlevel_callback();

    speed_counter = 0;

    show_mouse(screen);

    do {
	frames_dropped = 0;
	update = 0;

	while ((speed_counter > 0 && frames_dropped < 6) && (!want_quit)) {
	    update = 1;

	    calc();

	    if (comm != demo) {
		get_local_input();

		/* Hack: the X port of Allegro will "lightly" grab the
                   mouse in the window if you call get_mouse_mickeys.
                   You can still break out by moving the mouse faster.  */
 		{ int x, y; get_mouse_mickeys(&x, &y); }
	    }

	    switch (comm) {
		case peerpeer:
		    send_local_input();
		
		    while (!skReady()) {
			if (frames_dropped > 1) {
			    frames_dropped--;
			    if (!helping)
				render();
			}
		    }

		    if (recv_remote_inputs() < 0)
			goto quit;

		    break;

		case demo:
		    if (recv_demo_inputs() < 0)
			goto quit;
		
		    /* Special demo-mode keys.  */

		    if (key[KEY_S]) {
			rest(10);
			speed_counter = 0;
		    }
		    else if (key[KEY_F]) {
			if (speed_counter < 2)
			    speed_counter += 2;
		    }

		    if (kb_enter && num_players == 2) {
			local = 1 - local;
			/* add_msgf(-1, "< Now watching %s >", players[local].name); */

			kb_enter = 0;
		    }
		
		    /* screenshots: dodgy web design stuff */
		    {
			static char ss_name[80];
			static int ss_num = 0;

			if (kb_p) {
			    while (ss_num <= 9999) {
				sprintf(ss_name, "shot%04d.pcx", ss_num++);
				if (exists(ss_name))
				    continue;
				if (save_bitmap(ss_name, dbuf, dat[GAMEPAL].dat) != 0)
				    add_msgf(-1, "Error writing %s", ss_name);
				break;
			    }
			    
			    speed_counter = 1;
			    kb_p = 0;
			}
		    }

		    break;

		case cpu:
		    do_cpu();
		    break;
		    
		default:
		    break;
		
	    } /* end receive inputs */

	    if (comm != demo)
		demo_write_player_inputs();

	    respawn_tiles();
	    respawn_ammo();

	    update_mines();
	    update_bullets();
	    update_particles();
	    update_explo();
	    update_blods();
	    update_corpses();
	    update_backpacks();

	    update_msgs();

	    for (i = 0; i < MAX_PLAYERS; i++)
		if (players[i].exist)
		    update_player(i);

	    touch_mines();
	    touch_bullets();
	    touch_backpacks();

	    if (shake_factor > 20)
		shake_factor = 20;

	    if (shake_factor) {
		shake_factor--;
		shakex = (rnd() % (shake_factor * 2 + 1)) - shake_factor;
		shakey = (rnd() % (shake_factor * 2 + 1)) - shake_factor;
	    }

	    if (--heart_anim < 0) {
		heart_anim = HEART_ANIM;
		if (++heart_frame > 3)
		    heart_frame = 0;
	    }

	    if (--blood_anim < 0) {
		blood_anim = HEART_ANIM;	/* just borrow this */
		if (++blood_frame > 5)
		    blood_frame = 0;
	    }

	    if (kb_f11) {
		toggle_fullscreen_window();
		kb_f11 = 0;
	    }

	    if (kb_f12) {
		show_fps = !show_fps;
		kb_f12 = 0;
	    }

	    if (comm == cpu) {
		if (kb_f2) {
		    kb_f2 = 0;
		    if (cpu_agressiveness < 5) {
			cpu_agressiveness++;
			add_msgf(-1, "AI LEVEL: %s", cpu_angst_str[cpu_agressiveness-1]);
		    }
		}

		if (kb_f3) {
		    kb_f3 = 0;
		    if (cpu_agressiveness > 1) {
			cpu_agressiveness--;
			add_msgf(-1, "AI LEVEL: %s", cpu_angst_str[cpu_agressiveness-1]);
		    }
		}
	    }

	    speed_counter--;
	    frames_dropped++;

	} /* end logic section */

	if (update) {
	    if (want_help && !helping) {
		helping = 1;
		show_mouse(NULL);
		clear_bitmap(screen);
		set_palette(dat[Z_HELPPAL].dat);
		if (comm == demo)
		    blit_to_screen(dat[Z_HELPDEMO].dat);
		else
		    blit_to_screen(dat[Z_HELP].dat);
	    }
	    else if (!want_help && helping) {
		helping = 0;
		clear_bitmap(screen);
		set_palette(dat[GAMEPAL].dat);
		render();
		show_mouse(screen);
	    }
	    else if (!helping) {
		render();
	    }
	}

    } while (!want_quit);
    
    if (comm == peerpeer) {
	skSend(PACKET_QUITGAME);
	while (skRecv() != PACKET_QUITOK)
	    rest(10);
    }

    while (key[KEY_ESC] && mouse_b)
	rest(0);
    clear_keybuf();

  quit:
    
    uninstall_my_keyboard_lowlevel_callback();
}
