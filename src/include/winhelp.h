/* 
 * Windows needs a little kick.
 */

#ifndef __included_winhelp_h
#define __included_winhelp_h

#ifdef TARGET_WINDOWS

/*----------------------------------------------------------------------
 *
 * findfirst, findnext interface
 * FIXME: Allegro should be providing this, but doesn't under Windows 
 *
 *----------------------------------------------------------------------*/

#include <io.h>

#define ffblk 		_finddata_t
#define ff_name(f) 	(f.name)

static int __find_handle;

static int findfirst(const char *pathname, struct ffblk *block, int attrib)
{
    __find_handle = _findfirst(pathname, block);
    return (__find_handle < 0) ? -1 : 0;
}
		     
static int findnext(struct ffblk *block)
{
    return _findnext(__find_handle, block);
}

static void findclose(void *x)
{
    _findclose(__find_handle);
}


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

#else

#define ff_name(f) 	(f.ff_name)

#endif /* TARGET_WINDOWS */


#ifdef TARGET_DJGPP

#define findclose(x)

#endif

#endif /* __included_winhelp_h */
