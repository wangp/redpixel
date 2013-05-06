/* 
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Player update.
 */


#include <allegro.h>
#include "anim.h"
#include "blood.h"
#include "bullet.h"
#include "colours.h"
#include "gameloop.h"
#include "map.h"
#include "message.h"
#include "mine.h"
#include "particle.h"
#include "player.h"
#include "sound.h"
#include "stats.h"
#include "statlist.h"
#include "tiles.h"


static inline int tile_is_solid_at_p(int x, int y)
{
    return tile_is_solid(tile_at_p(x, y));
}


void update_player(int pl)
{
    PLAYER *guy;
    int kx, ky, a, x, y;
    int move_legs = 0;

    guy = &players[pl];
    
    /* respawning */
    if (guy->respawn)
	respawn_player(pl);

    if (!guy->health)
	return;

    /* degrading goggles */
    if (guy->visor_tics)
	--guy->visor_tics;

    /* degrading bloodlust */
    if (guy->blood_tics)
	if (--guy->blood_tics == 0)
	    guy->bloodlust = 0;

    /* degrading scanner */
    if (guy->scanner_tics)
	--guy->scanner_tics;

    /* selecting weapon */
    if (guy->select) {
	guy->cur_weap = guy->pref_weap = guy->select;
	if (pl == local)
	    snd_local(WAV_SELECT);
    }

    /* picking up stuff ie. pickups */
    kx = (guy->x + 3) / 16;
    ky = (guy->y + 6) / 16;
    
    a = ammo_at(kx, ky);
    if (a) {
	switch (a) {
	    case A_ARMOUR:
		if (guy->armour < st_max_armour) {
		    guy->armour += st_armour_give;
		    if (guy->armour > st_max_armour)
			guy->armour = st_max_armour;
		    map.ammo[ky][kx] = 0;
		    snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		}
		break;

	    case A_ARROW:
		guy->num_arrows += st_arrow_give;
		map.ammo[ky][kx] = 0;
		auto_weapon(pl, 0);
		snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		break;

	    case A_BULLET:
		guy->num_bullets += st_bullet_give;
		map.ammo[ky][kx] = 0;
		auto_weapon(pl, 0);
		snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		break;

	    case A_CHICKEN:
		if (guy->health < st_max_health) {
		    guy->health += st_chicken_heal;
		    if (guy->health > st_max_health)
			guy->health = st_max_health;
		    map.ammo[ky][kx] = 0;
		    add_msg("<Burp>", pl);
		}
		break;

	    case A_COKE:
		if (guy->health < st_max_health) {
		    guy->health += st_coke_heal;
		    if (guy->health > st_max_health)
			guy->health = st_max_health;
		    map.ammo[ky][kx] = 0;
		}
		break;

	    case W_MINE:
		guy->num_mines += st_mine_give;
		guy->have[w_mine] = 1;
		map.ammo[ky][kx] = 0;
		auto_weapon(pl, 0);
		add_msg("Got yerself a mine", pl);
		snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		break;

	    case A_ROCKET:
		guy->num_rockets += st_rocket_give;
		map.ammo[ky][kx] = 0;
		auto_weapon(pl, 0);
		snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		break;

	    case A_SHELL:
		guy->num_shells += st_shell_give;
		map.ammo[ky][kx] = 0;
		auto_weapon(pl, 0);
		snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		break;

	    case A_GOGGLES:
		guy->visor_tics = GAME_SPEED * st_goggles_timeout;
		map.ammo[ky][kx] = 0;
		add_msg("Light-amp enabled", pl);
		snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		break;

	    case A_BLOODLUST:
		guy->blood_tics = GAME_SPEED * st_bloodlust_timeout;
		guy->bloodlust = 1;
		map.ammo[ky][kx] = 0;
		add_msg("You are the spawn of Satan!", pl);
		add_msg("Go kill something!!", pl);
		snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		snd_local(WAV_LETSPARTY);
		break;

	    case A_SCANNER:
		guy->scanner_tics = GAME_SPEED * st_scanner_timeout;
		map.ammo[ky][kx] = 0;
		add_msg("Dead meat tracker ON", pl);
		snd_3d(WAV_INCOMING, 255, guy->x, guy->y);	/* dodgy! */
		break;

	    case W_BOW:
		guy->have[w_bow] = 1;
		guy->num_arrows += st_bow_give;
		map.ammo[ky][kx] = 0;
		add_msg("Dynamite on sticks!", pl);
		snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		break;

	    case W_M16:
		guy->num_bullets += st_m16_give;
		map.ammo[ky][kx] = 0;
		auto_weapon(pl, w_m16);
		add_msg("M16", pl);
		snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		break;

	    case W_MINI:
		guy->num_bullets += st_minigun_give;
		map.ammo[ky][kx] = 0;
		auto_weapon(pl, w_minigun);
		add_msg("Minigun! Find some meat!", pl);
		snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		break;

	    case W_PISTOL:
		guy->num_bullets += st_pistol_give;
		map.ammo[ky][kx] = 0;
		auto_weapon(pl, w_pistol);
		snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		break;

	    case W_ROCKET:
		guy->have[w_bazooka] = 1;
		guy->num_rockets += st_bazooka_give;
		map.ammo[ky][kx] = 0;
		add_msg("Baarrzooka!", pl);
		snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		break;

	    case W_SHOTGUN:
		guy->num_shells += st_shell_give;
		map.ammo[ky][kx] = 0;
		auto_weapon(pl, w_shotgun);
		snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		break;

	    case W_UZI:
		guy->num_bullets += st_uzi_give;
		map.ammo[ky][kx] = 0;
		auto_weapon(pl, w_uzi);
		snd_3d(WAV_PICKUP, 255, guy->x, guy->y);
		break;
	}
    }

    /* firing */
    if (guy->next_fire)
	--guy->next_fire;

    if (guy->fire) {
	int fired = 1;

	switch (guy->cur_weap) {
	    case w_minigun:
		if (--guy->num_bullets < 0) {
		    fired = guy->num_bullets = 0;
		    auto_weapon(pl, 0);
		}
		else {
		    spawn_bullet(pl, guy->angle, GREY, 0);
		    snd_3d(WAV_MINIGUN, 180, guy->x, guy->y);
		}
		break;

	    case w_m16:
		if (--guy->num_bullets < 0) {
		    fired = guy->num_bullets = 0;
		    auto_weapon(pl, 0);
		}
		else {
		    spawn_bullet(pl, guy->angle, GREY, 0);
		    snd_3d(WAV_M16, 255, guy->x, guy->y);
		}
		break;

	    case w_uzi:
		if (--guy->num_bullets < 0) {
		    fired = guy->num_bullets = 0;
		    auto_weapon(pl, 0);
		}
		else {
		    spawn_bullet(pl, guy->angle, GREY, 0);
		    snd_3d(WAV_UZI, 255, guy->x, guy->y);
		}
		break;

	    case w_pistol:
		if (--guy->num_bullets < 0) {
		    fired = guy->num_bullets = 0;
		    auto_weapon(pl, 0);
		}
		else {
		    spawn_bullet(pl, guy->angle, GREY, 0);
		    snd_3d(WAV_UZI, 255, guy->x, guy->y);
		}
		break;

	    case w_shotgun:
		if (--guy->num_shells < 0) {
		    fired = guy->num_shells = 0;
		    auto_weapon(pl, 0);
		}
		else {
		    spawn_bullet(pl, guy->angle - itofix(5), YELLOW, 0);
		    spawn_bullet(pl, guy->angle - itofix(3), YELLOW, 0);
		    spawn_bullet(pl, guy->angle, YELLOW, 0);
		    spawn_bullet(pl, guy->angle + itofix(2), YELLOW, 0);
		    spawn_bullet(pl, guy->angle + itofix(4), YELLOW, 0);
		    snd_3d(WAV_SHOTGUN, 255, guy->x, guy->y);
		}
		break;

	    case w_bazooka:
		if (--guy->num_rockets < 0) {
		    fired = guy->num_rockets = 0;
		    auto_weapon(pl, 0);
		}
		else {
		    spawn_bullet(pl, guy->angle, 0, J_ROCKET);
		    snd_3d(WAV_ROCKET, 255, guy->x, guy->y);
		}
		break;

	    case w_bow:
		if (--guy->num_arrows < 0) {
		    fired = guy->num_arrows = 0;
		    auto_weapon(pl, 0);
		}
		else {
		    spawn_bullet(pl, guy->angle, 0, J_ARROW);
		    snd_3d(WAV_BOW, 255, guy->x, guy->y);
		}
		break;

	    case w_knife:
		spawn_bullet(pl, guy->angle, 0, J_KNIFE);
		snd_3d(WAV_KNIFE, 255, guy->x, guy->y);
		break;
	}

	if (fired)
	    guy->next_fire = weaps[guy->cur_weap].reload;
	else {
	    guy->fire_frame = 0;
	    guy->firing = 0;
	}

	if (!guy->firing && fired) {
	    guy->firing = 1;
	    guy->fire_anim = 0;
	}
    }
    else {
	guy->firing = 0;
    }

    /* dropping mines */
    if (guy->drop_mine) {
	spawn_mine(guy->x, guy->y + 12, pl);
	guy->next_fire = weaps[w_mine].reload;
	if (--guy->num_mines < 1)
	    guy->have[w_mine] = 0;
	snd_3d(WAV_MINE, 255, guy->x, guy->y);
    }

    /* impaled on spikes ? */
    if (tile_at_p(guy->x + 3, guy->y + 16) == T_SPIKE)
	hurt_player(pl, 1, 0, pl, D_IMPALE000); /* armour doesn't affect spikes */

    /* climb down ladder */
    if (guy->down) {
	if (tile_at_p(guy->x + 3, guy->y + 17) == T_LAD ||
	    (tile_at_p(guy->x + 3, guy->y + 16) == T_LAD 
	     && tile_at_p(guy->x + 3, guy->y + 17) == 0)) {
	    guy->y += 2;
	    move_legs = 1;
	}
    }

    /* standing or falling */
    if (!guy->jump) {
	/* nothing under feet? */
	if (!tile_collide_p(guy->x + 3, guy->y + 16)) {
	    guy->y += guy->yv;	/* fall! */

	    if (guy->y > map.h * 16) {
		hurt_player(pl, players[pl].health, 0, pl, 0);
		return;
	    }
	    
	    /* hit solid ground? */
	    if (tile_collide_p(guy->x + 3, guy->y + 16)) {
		if (guy->y < -10)   /* Hack: don't know why. */
		    guy->y = -16;
		else
		    guy->y = ((int)guy->y / 16) * 16;
		guy->yv = 0;
	    }
	    else if (guy->yv < 6)  /* nope... increase gravitational pull */
		guy->yv++;
	}
	else if (guy->up) { 	/* jump or climb ladder */
	    if (!guy->left && !guy->right 
		&& tile_at_p(guy->x + 3, guy->y + 15) == T_LAD) {
		if (!tile_is_solid_at_p(guy->x + 3, guy->y - 1)) {
		    guy->y -= 2;
		    move_legs = 1;
		}
	    }
	    else {		/* jump */
		guy->jump = 1;
		guy->yv = -11;
	    }
	}
    }
    else {			/* in process of jumping */
	guy->y += guy->yv;
	guy->yv++;

	if (tile_is_solid_at_p(guy->x + 3, guy->y)) {
	    guy->y = ((int)(guy->y + 16) / 16) * 16;
	    guy->jump = guy->yv = 0;
	}

	if (guy->yv == 0)
	    guy->jump = guy->yv = 0;
    }

    /* when getting blown back or something */
    if (guy->xv) {
	guy->x += guy->xv;

	if (tile_is_solid_at_p(guy->x + 3, guy->y)) {
	    guy->x = ((int)(guy->x + 3 - guy->xv) / 16) * 16;
	    guy->xv = 0;
	}

	if (guy->xv < 0)
	    guy->xv++;
	if (guy->xv > 0)
	    guy->xv--;
    }

    /* left */
    if (guy->left) {
	if (!tile_is_solid_at_p(guy->x, guy->y + 15))
	    guy->x -= 3;
	move_legs = 1;
    }

    /* right */
    if (guy->right) {
	if (!tile_is_solid_at_p(guy->x + 7, guy->y + 15))
	    guy->x += 3;
	move_legs = 1;
    }

    /* squirting blood */
    if (guy->health <= 20)
	spawn_particles(guy->x + 3, guy->y + 6, 1, grad_red);

    /* anims */
    if (move_legs) {
	if (--guy->leg_tics < 0) {
	    guy->leg_tics = LEG_ANIM;
	    if (++guy->leg_frame > 3)
		guy->leg_frame = 0;
	}
    }

    if (guy->firing || guy->fire_frame) {
	if (--guy->fire_anim <= 0) {
	    ++guy->fire_frame;

	    if (guy->fire_frame > 4 || ((guy->cur_weap != w_minigun) &&
					(guy->fire_frame > 3))) {
		guy->fire_frame = 0;

		if (guy->firing && (guy->cur_weap == w_m16 ||
				    guy->cur_weap == w_uzi ||
				    guy->cur_weap == w_minigun))
		    guy->fire_frame = 1;
	    }

	    /* eject casings */
	    if ((guy->cur_weap == w_shotgun && guy->fire_frame == 3) ||
		(guy->cur_weap == w_m16 && guy->fire_frame == 3) ||
		(guy->cur_weap == w_pistol && guy->fire_frame == 2) ||
		(guy->cur_weap == w_uzi && guy->fire_frame == 3) ||
		(guy->cur_weap == w_minigun && guy->fire_frame == 2)) {

		x = guy->x + 3 + fixtoi(fixcos(guy->angle)) * 5;
		y = guy->y + 3 + fixtoi(fixsin(guy->angle)) * 5;
		spawn_casing(x, y, guy->facing);
	    }

	    guy->fire_anim = weaps[guy->cur_weap].anim;
	}
    }
}
