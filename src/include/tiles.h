#ifndef __included_tiles_h
#define __included_tiles_h


int tile_is_solid(int x);

int hurt_tile(int u, int v, int dmg, int tag);
void respawn_tiles();
void respawn_ammo();
void draw_tiles_and_stuff();

int tile_collide(int x, int y);
int tile_collide_p(int x, int y);
#define tile_at		tile_collide
#define tile_at_p	tile_collide_p

int ammo_collide(int x, int y);
#define ammo_at	ammo_collide


#endif
