/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  jgmod wrapper.
 */


#ifndef NO_JGMOD_CODE


#include <allegro.h>
#include "jgmod.h"
#include "rnd.h"
#include "rpjgmod.h"


static int inited;
static JGMOD *current_mod;


static struct filename {
    struct filename *next;
    char *filename;
    int times_played;
} *filenames;

static int num_filenames;
static int num_played;
static int times_played_threshold;


static int is_module(char *filename)
{
    char *ext = get_extension(filename);
    return (ext && ((0 == ustricmp(ext, "xm")) ||
		    (0 == ustricmp(ext, "s3m")) ||
		    (0 == ustricmp(ext, "mod"))));
}


void rpjgmod_init(void)
{
    install_mod(16);
    set_mod_volume(128);

    /* create list of filenames */
    {
	struct al_ffblk ffblk;
	struct filename *fn;

	num_filenames = 0;
	num_played = 0;
	times_played_threshold = 0;

	if (al_findfirst("music/*", &ffblk, FA_RDONLY | FA_HIDDEN | FA_SYSTEM | FA_ARCH) == 0) {
	    do {
		if (is_module(ffblk.name)) {
		    if (!(fn = malloc(sizeof *fn)))
			goto end;
		    if (!(fn->filename = ustrdup(ffblk.name))) {
			free(fn);
			goto end;
		    }
		    fn->times_played = 0;
		    fn->next = filenames;
		    filenames = fn;
		    num_filenames++;
		}
	    } while (al_findnext(&ffblk) == 0);

	  end:
	    
	    al_findclose(&ffblk);
	}
    }

    inited = 1;
}


void rpjgmod_shutdown(void)
{
    while (filenames) {
	struct filename *next = filenames->next;
	free(filenames->filename);
	free(filenames);
	filenames = next;
    }

    rpjgmod_stop();

    inited = 0;
}


static struct filename *get_filename_by_index(int i)
{
    struct filename *f = filenames;
    while (i--)
	f = f->next;
    return f;
}


void rpjgmod_play_random_track(void)
{
    struct filename *f;
    int num_tried = 0;
    char buf[1024];

    if ((!inited) || (num_filenames == 0))
	return;

    while (num_tried < num_filenames) {	/* pathological case */
	if (num_played == num_filenames) {
	    num_played = 0;
	    times_played_threshold++;
	}

	f = get_filename_by_index(rnd() % num_filenames);
	while (f->times_played > times_played_threshold)
	    if (!(f = f->next))
		f = filenames;
	f->times_played++;
	num_played++;

	uszprintf(buf, sizeof buf, "music/%s", f->filename);
	if ((current_mod = load_mod(buf))) {
	    play_mod(current_mod, FALSE);
	    return;
	}
	else
	    num_tried++;
    }
}


void rpjgmod_poll(void)
{
    if (!is_mod_playing())
	rpjgmod_play_random_track();
}


void rpjgmod_stop(void)
{
    if (current_mod) {
	stop_mod();
	destroy_mod(current_mod);
	current_mod = NULL;
    }
}


#else


/*
 * Don't want jgmod.
 */

void rpjgmod_init(void) {}
void rpjgmod_shutdown(void) {}
void rpjgmod_play_random_track(void) {}
void rpjgmod_poll(void) {}
void rpjgmod_stop(void) {}


#endif /* NO_JGMOD_CODE */
