#include "blood.h"
#include "w_types.h"

export respawn;
export damage;
export reload;
export anim;



/* respawn rates */

int respawn(int item)
{
    switch (item)
    {
	case A_CHICKEN: return 40 * 120;
	case A_COKE:    return 40 * 60;
	case A_ARMOUR:  return 40 * 360;
	case A_GOGGLES: return 40 * 360;

	case A_BULLET:  return 40 * 30;
	case A_SHELL:   return 40 * 20;
	case A_ARROW:   return 40 * 50;
	case W_MINE:    return 40 * 90;
	case A_ROCKET:  return 40 * 70;

	case W_PISTOL:  return 40 * 60;
	case W_SHOTGUN: return 40 * 75;
	case W_UZI:     return 40 * 90;
	case W_M16:     return 40 * 105;
	case W_MINI:    return 40 * 150;
	case W_BOW:     return 40 * 130;
	case W_ROCKET:  return 40 * 150;

	default:        return 0;
    }
}


/* weapon stats */

int damage(int weapon)
{
    switch (weapon)
    {
	case w_knife:   return 12;
	case w_pistol:  return 14;
	case w_bow:     return 40;
	case w_shotgun: return 7;
	case w_uzi:     return 12;
	case w_m16:     return 15;
	case w_minigun: return 5;
	case w_bazooka: return 65;
	case w_mine:    return 45;
	default:        return 0;
    }
}

int reload(int weapon)
{
    switch (weapon)
    {
	case w_knife:   return 20;
	case w_pistol:  return 20;
	case w_bow:     return 28;
	case w_shotgun: return 24;
	case w_uzi:     return 8;
	case w_m16:     return 5;
	case w_minigun: return 1;
	case w_bazooka: return 40;
	case w_mine:    return 25;
	default:        return 0;
    }
}

int anim(int weapon)
{
    switch (weapon)
    {
	case w_knife:   return 3;
	case w_pistol:  return 4;
	case w_bow:     return 4;
	case w_shotgun: return 6;
	case w_uzi:     return 3;
	case w_m16:     return 2;
	case w_minigun: return 1;
	case w_bazooka: return 6;
	case w_mine:    return 0;
	default:        return 0;
    }
}