/*
 *  Wrap rg_rand.c with old function names.
 */

#include "rg_rand.h"
#include "rnd.h"


static long _rnd;

int rnd()
{
    return (_rnd = longrand(_rnd));
}

void srnd(unsigned long seed)
{
    _rnd = slongrand(seed);
}


/*
 * This one is used for the more important stuff
 * (the stuff that has to be synchronised)
 */

static long _irnd;

int irnd()
{
    return (_irnd = longrand(_irnd));
}

void sirnd(unsigned long seed)
{
    _irnd = slongrand(seed);
}


