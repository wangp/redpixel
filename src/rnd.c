/*
 * Wrap rg_rand.c with old function names.
 */

#include "rg_rand.h"
#include "rnd.h"


static int32_t _rnd;

int rnd()
{
    return (_rnd = longrand(_rnd));
}

void srnd(uint32_t seed)
{
    _rnd = slongrand(seed);
}


/*
 * This one is used for the more important stuff
 * (the stuff that has to be synchronised)
 */

static int32_t _irnd;

int irnd()
{
    return (_irnd = longrand(_irnd));
}

void sirnd(uint32_t seed)
{
    _irnd = slongrand(seed);
}


