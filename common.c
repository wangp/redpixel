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
    switch (pic)
    {
	case A_CHICKEN: return 40 * 120;
	case A_COKE:    return 40 * 60;
	case A_ARMOUR:  return 40 * 180;
	case A_GOGGLES: return 40 * 180;
	case A_BLOODLUST: return 40 * 180;

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


//---------------------------------------------------------- file io ---------

char file_hdr[] = "­WACKED­";

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

void save_map(char *fn)
{
    PACKFILE *fp;
    int u, v; 

    fp = pack_fopen(fn, "wp");
    if (!fp) return;

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
}

void load_map(char *fn)
{ 
    PACKFILE *fp;
    int u, v;
    char test[9];

    fp = pack_fopen(fn, "rp");
    if (!fp)
	return;

    reset_map();

    pack_fgets(test, 9, fp);
    if (memcmp(test, file_hdr, 8))
    {
	pack_fclose(fp);
	return;
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
		map.tiletics[v][u] = TILE_HEALTH;
	    else
		map.tiletics[v][u] = 0;

	    if (map.ammo[v][u])
		map.ammotics[v][u] = ammo_respawn_rate(map.ammo[v][u]);
	    else
		map.ammotics[v][u] = 0;
	}

    pack_fclose(fp);
}