#ifndef __included_weapon_h
#define __included_weapon_h


typedef struct {
    int anim;
    int reload;
    int dmg;
} WEAPON;


struct weapon_order {
    int pic;
    int weap;
    int shortcut;
    char *s;
};


/* weapon types */
#define w_knife     1
#define w_pistol    2
#define w_bow       3
#define w_shotgun   4
#define w_uzi       5
#define w_m16       6
#define w_minigun   7
#define w_bazooka   8
#define w_mine      9
#define num_weaps   10


extern WEAPON weaps[num_weaps];

extern struct weapon_order weapon_order[];


int num_ammo(int pl, int weapon);


#endif
