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
 *  Shared code between engine and map editor.
 */


#include <stdio.h>
#include <allegro.h>
#include "common.h"


// globals 

MAP map;    // jes the one


//------------------------------------------------------------------- lists --

LIST tiles[] =
{
    { T_BAR,    1 },
    { T_BLA,   10 },
    { T_BLU,   13 },
    { T_GRE,   16 },
    { T_PUR,   19 },
    { T_RED,   22 },
    { T_METAL, 27 },
    { T_WOOD,  28 },
    { T_CRATE, 30 },
    { T_LAD,   40 },
    { T_SPIKE, 50 },
    { -1 }
};

LIST ammos[] =
{
    { A_BULLET,  1 },
    { A_ARROW,   2 },
    { W_MINE,    3 },
    { A_ROCKET,  4 },
    { A_SHELL,   5 },
    { A_CHICKEN, 20 },
    { A_COKE,    21 },
    { A_ARMOUR,  50 }, 
    { A_GOGGLES, 60 },
    { A_BLOODLUST, 70 },
    { A_SCANNER, 80 },
    { W_BOW,     100 },
    { W_M16,     101 },
    { W_MINI,    102 },
    { W_PISTOL,  103 },
    { W_ROCKET,  104 },
    { W_SHOTGUN, 105 },
    { W_UZI,     106 },
    { -1 }
};

int num2pic(LIST *l, int num)
{
    int i = 0;
    if (num==0) return 0;
    while (l[i].num != num && l[i].pic != -1) 
	i++;
    if (l[i].pic<0)
	return 0;
    return l[i].pic;
}

int pic2num(LIST *l, int pic)
{
    int i = 0;
    if (pic==0) return 0;
    while (l[i].pic != pic && l[i].pic != -1) 
	i++;
    if (l[i].num<0)
	return 0;
    return l[i].num;
}


//---------------------------------------------------[ pickup respawn times ]-

int ammo_respawn_rate(int pic)
{
    /* yuck */
    switch (pic)
    {
	case A_CHICKEN:   return GAME_SPEED * st_chicken_respawn;
	case A_COKE:      return GAME_SPEED * st_coke_respawn;
	case A_ARMOUR:    return GAME_SPEED * st_armour_respawn;
	case A_GOGGLES:   return GAME_SPEED * st_goggles_respawn;
	case A_BLOODLUST: return GAME_SPEED * st_bloodlust_respawn;
	case A_SCANNER:   return GAME_SPEED * st_scanner_respawn;

	case A_BULLET:  return GAME_SPEED * st_bullet_respawn;
	case A_SHELL:   return GAME_SPEED * st_shell_respawn;
	case A_ARROW:   return GAME_SPEED * st_arrow_respawn;
	case W_MINE:    return GAME_SPEED * st_mine_respawn;
	case A_ROCKET:  return GAME_SPEED * st_rocket_respawn;

	case W_PISTOL:  return GAME_SPEED * st_pistol_respawn;
	case W_SHOTGUN: return GAME_SPEED * st_shotgun_respawn;
	case W_UZI:     return GAME_SPEED * st_uzi_respawn;
	case W_M16:     return GAME_SPEED * st_m16_respawn;
	case W_MINI:    return GAME_SPEED * st_minigun_respawn;
	case W_BOW:     return GAME_SPEED * st_bow_respawn;
	case W_ROCKET:  return GAME_SPEED * st_rocket_respawn;

	default:        return 0;
    }
}


//---------------------------------------------------------- file io ---------

char file_hdr[] = { 0xad, 'W', 'A', 'C', 'K', 'E', 'D', 0xad, '\0' };

void reset_map()
{
    int u, v;

    for (u=0; u<24; u++)
	map.start[u].x = 255;

    for (v=0; v<128; v++)
    {
	for (u=0; u<128; u++)
	{
	    map.tile[v][u] = 0;
	    map.ammo[v][u] = 0;
	}
    }

    map.w = 64;
    map.h = 32;
}

int save_map(char *fn)
{
    PACKFILE *fp;
    int u, v; 

    fp = pack_fopen(fn, F_WRITE_PACKED);
    if (!fp) return -1;

    pack_fputs(file_hdr, fp);

    pack_putc(map.w, fp);
    pack_putc(map.h, fp);

    for (u=0; u<24; u++)
    {
	pack_putc(map.start[u].x, fp);
	pack_putc(map.start[u].y, fp); 
    }

    for (v=0; v<map.h; v++)
	for (u=0; u<map.w; u++)
	{
	    pack_iputw(pic2num(tiles, map.tile[v][u]), fp);
	    pack_iputw(pic2num(ammos, map.ammo[v][u]), fp);
	}

    pack_fclose(fp);
    return 0;
}

int load_map(char *fn)
{ 
    PACKFILE *fp;
    int u, v;
    char test[9];
    
    fp = pack_fopen(fn, F_READ_PACKED);
    if (!fp) 
	return -1;
    
    reset_map();

    pack_fread(test, 8, fp);
    if (memcmp(test, file_hdr, 8)) {
	pack_fclose(fp);
	return -1;
    }

    map.w = pack_getc(fp);
    map.h = pack_getc(fp); 

    for (u=0; u<24; u++)
    {
	map.start[u].x = pack_getc(fp);
	map.start[u].y = pack_getc(fp);
    }

    for (v=0; v<map.h; v++)
	for (u=0; u<map.w; u++)
	{
	    map.tileorig[v][u] = map.tile[v][u] = num2pic(tiles, pack_igetw(fp));
	    map.ammoorig[v][u] = map.ammo[v][u] = num2pic(ammos, pack_igetw(fp));

	    if (map.tile[v][u]==T_WOOD || map.tile[v][u]==T_CRATE ||
		map.tile[v][u]==T_BAR)
		map.tiletics[v][u] = st_tile_health;
	    else
		map.tiletics[v][u] = 0;

	    if (map.ammo[v][u])
		map.ammotics[v][u] = ammo_respawn_rate(map.ammo[v][u]);
	    else
		map.ammotics[v][u] = 0;
	}

    pack_fclose(fp);
    return 0;
}
