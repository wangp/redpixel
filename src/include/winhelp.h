/* 
 * Windows needs a little kick.
 */

#ifndef __included_winhelp_h
#define __included_winhelp_h

#ifdef TARGET_WINDOWS

/*----------------------------------------------------------------------
 *
 * getopt.c
 *
 *----------------------------------------------------------------------*/

extern char *optarg;
extern int   optind;
extern int   opterr;

int getopt(int argc, char *argv[], char *opstring);


/*----------------------------------------------------------------------
 *
 * sleep
 *
 *----------------------------------------------------------------------*/

/* FIXME */
#define sleep(x)

#endif /* TARGET_WINDOWS */

#endif /* __included_winhelp_h */
