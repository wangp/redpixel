/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Weapon stuff.
 */


#include <allegro.h>
#include "blood.h"
#include "player.h"
#include "weapon.h"


WEAPON weaps[num_weaps];


struct weapon_order weapon_order[] = {
    { W_MINE,    w_mine, 	KEY_ENTER, "\177" }, /* 127 == enter symbol */
    { W_KNIFE,   w_knife, 	KEY_1,     "1" },
    { W_PISTOL,  w_pistol, 	KEY_2,     "2" },
    { W_SHOTGUN, w_shotgun, 	KEY_3,     "3" },
    { W_UZI, 	 w_uzi, 	KEY_4,     "4" },
    { W_M16, 	 w_m16, 	KEY_5,     "5" },
    { W_MINI, 	 w_minigun, 	KEY_6,     "6" },
    { W_BOW,     w_bow, 	KEY_7,     "7" },
    { W_ROCKET,  w_bazooka, 	KEY_8, 	   "8" },
    { 0,	 0,		0,	   NULL }
};


