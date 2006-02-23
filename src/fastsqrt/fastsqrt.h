#ifndef _FASTSQRT_H
#define _FASTSQRT_H

/*
 * A High Speed, Low Precision Square Root
 * by Paul Lalonde and Robert Dawson
 * from "Graphics Gems", Academic Press, 1990

 * Modified by Javier Arevalo to avoid some unnecessary shifts.
 * Seems like this adds some precision too, but I'm unsure.

 * Ported to DJGPP/GCC environment by Brennan Underwood (brennan@rt66.com)

 * NO WARRANTY!
 */

/* function prototypes */
void build_sqrt_table(void); /* you MUST call this first! */
float fast_fsqrt(float n) __attribute__((const));

#endif
