#include "a4aux.h"
#include "rpblend.h"

void opaque_blender(void)
{
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
}

void normal_blender(void)
{
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
}

void additive_blender(void)
{
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ONE);
}

void multiply_blender(void)
{
    al_set_blender(ALLEGRO_ADD, ALLEGRO_DST_COLOR, ALLEGRO_ZERO);
}
