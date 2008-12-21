/*
 * st_xxx need to be copied into weaps[xxx]
 */


#include <allegro.h>
#include "engine.h"
#include "stats.h"
#include "statlist.h"
#include "weapon.h"
#include "setweaps.h"


#define set_weaps(x) 				\
    weaps[w_##x].anim = st_##x##_anim; 		\
    weaps[w_##x].reload = st_##x##_reload; 	\
    weaps[w_##x].dmg = st_##x##_damage;


void set_weapon_stats(void) 
{
    set_weaps(knife);			
    set_weaps(pistol);			
    set_weaps(bow);				
    set_weaps(shotgun);			
    set_weaps(uzi);				
    set_weaps(m16);				
    set_weaps(minigun);			
    set_weaps(bazooka);			
    set_weaps(mine);			
}
