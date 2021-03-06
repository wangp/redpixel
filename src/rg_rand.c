/*
 *  Taken from the Snippets collection.
 * 
 *  20/05/2000 - modified slightly to support multiple instances.
 */

#include "rg_rand.h"

/* +++Date last modified: 05-Jul-1997 */

/*
**  longrand() -- generate 2**31-2 random numbers
**
**  public domain by Ray Gardner
** 
**  based on "Random Number Generators: Good Ones Are Hard to Find",
**  S.K. Park and K.W. Miller, Communications of the ACM 31:10 (Oct 1988),
**  and "Two Fast Implementations of the 'Minimal Standard' Random
**  Number Generator", David G. Carta, Comm. ACM 33, 1 (Jan 1990), p. 87-88
**
**  linear congruential generator f(z) = 16807 z mod (2 ** 31 - 1)
**
**  uses L. Schrage's method to avoid overflow problems
*/

#define a 16807         /* multiplier */
#define m 2147483647L   /* 2**31 - 1 */
#define q 127773L       /* m div a */
#define r 2836          /* m mod a */

static int32_t nextlongrand(int32_t seed)
{
      uint32_t lo, hi;

      lo = a * (int32_t)(seed & 0xFFFF);
      hi = a * (int32_t)((uint32_t)seed >> 16);
      lo += (hi & 0x7FFF) << 16;
      if (lo > m)
      {
            lo &= m;
            ++lo;
      }
      lo += hi >> 15;
      if (lo > m)
      {
            lo &= m;
            ++lo;
      }
      return (int32_t)lo;
}

int32_t longrand(int32_t randomnum)           /* return next random long */
{
      return nextlongrand(randomnum);
}

int32_t slongrand(uint32_t seed)      /* to seed it */
{
      return (seed ? (seed & m) : 1);   /* nonzero seed */
}


#ifdef TEST

#undef a m q r

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
      int32_t reps, k, num;
      uint32_t seed;

      reps = 10000;
      seed = 1;

      /*
      ** correctness test: after 10000 reps starting with seed 1,
      ** result should be 1043618065
      */
    
      if (argc > 1)
            reps = atol(argv[1]);
      if (argc > 2)
            seed = atol(argv[2]);

      printf("seed %ld for %ld reps...\n", seed, reps);
      num = slongrand(seed);
      for (k = 0; k < reps; ++k)
            num = longrand(num);
      printf("%ld\n", num);

      return 0;
}

#endif /* TEST */
