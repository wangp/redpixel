/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Options part of menu.  The height of ugly.
 */


#include "a4aux.h"
#include <string.h>
#include "agup.h"
#include "blood.h"
#include "mousespr.h"
#include "globals.h"
#include "resource.h"
#include "rpagup.h"
#include "setweaps.h"
#include "sound.h"
#include "stats.h"
#include "statlist.h"
#include "suicide.h"
#include "vidmode.h"



int record_demos;
int mute_sfx;

static int sfx_volume;



static FONT *old_font;
static char stats_filename[1024];
static char stats_path[1024];


static int sfx_volume_callback(void *dp3, int d2)
{
    (void)dp3;
    set_volume(d2 * 32, -1);
    snd_local(WAV_DEAD1 + (rand() % (WAV_DEAD5 - WAV_DEAD1 + 1)));
    return D_O_K;
}


static int push_stats_button(DIALOG *d)
{
    char path[1024];
    FONT *save = font;
    (void)d;
    font = old_font;

    strncpy(path, get_resource(R_SHARE, "stats/"), sizeof path);

    if (file_select_ex("Use stats file...", path, "st", sizeof path, 0, 0)) {
	if (!read_stats(path, stat_block)) {
	    alert("Error reading", path, "Reverting to previous stats", "Ok", NULL, 13, 27);
	    pop_stat_block();
	}
	else {
	    strcpy(stats_filename, get_filename(path));
	    strcpy(stats_path, path);
	}
    }

    font = save;
    return D_O_K;
}


DIALOG config_dlg[] =
{
    /* proc                   x     y     w    h    fg    bg    key  flags        d1    d2  dp                dp2    dp3 */
    { d_clear_proc,           0,    0,    0,   0,   0,    0,    0,   0,           0,    0,  NULL,             NULL,  NULL },/* 0 */
    { d_yield_proc,           14,   5,    0,   0,   0,    0,    0,   0,           0,    0,  "DESIRED RESOLUTION", NULL,  NULL }, /* 1 */
    { d_yield_proc,           10,   15,   140, 46,  0,    0,    0,   0,           0,    0,  NULL,             NULL,  NULL }, /* 2 */

    { d_yield_proc,           165,  2,    130, 60,  0,    0,    0,   0,           0,    0,  NULL,             NULL,  NULL }, /* 3 */
    { d_yield_proc,           170,  7,    120, 16,  0,    0,    0,   D_SELECTED,  0,    0,  "SCANLINES (HI-RES)", NULL,  NULL }, /* 4 */
    { d_yield_proc,           170,  24,   120, 16,  0,    0,    0,   0,           0,    0,  "FILTERED",       NULL,  NULL }, /* 5 */
    { d_yield_proc,           170,  40,   120, 16,  0,    0,    0,   0,           0,    0,  "\"FAMILY\" MODE",NULL,  NULL }, /* 6 */

    { d_agup_box_proc,        10,   65,   300, 100, 0,    0,    0,   0,           0,    0,  NULL,             NULL,  NULL }, /* 7 */

    { d_agup_check_proc,      15,   70,   130, 20,  0,    0,    0,   0,           0,    0,  "MUTE SOUND EFFECTS", NULL,  NULL }, /* 8 */
    { d_yield_proc,           15,   95,   130, 20,  0,    0,    0,   0,           1,    0,  "NO MUSIC",       NULL,  NULL }, /* 9 */
    { d_yield_proc,           15,   115,  130, 20,  0,    0,    0,   0,           1,    0,  "PLAY MODULES",   NULL,  NULL }, /* 10 */
    { d_yield_proc,           15,   135,  130, 20,  0,    0,    0,   0,           1,    0,  "PLAY CD",        NULL,  NULL }, /* 11 */

    { d_agup_check_proc,      180,  70,   100, 20,  0,    0,    0,   0,           0,    0,  "RECORD DEMOS",   NULL,  NULL }, /* 12 */

    { d_yield_proc,           160,  95,   40,  8,   0,    0,    0,   0,           0,    0,  "MOUSE SPEED",    NULL,  NULL }, /* 13 */
    { d_yield_proc,           225,  93,   60,  12,  0,    0,    0,   0,           3,    0,  NULL,             NULL,  NULL }, /* 14 */

    { d_text_proc,            160,  117,  20,  8,   0,    0,    0,   0,           0,    0,  "SFX",            NULL,  NULL }, /* 15 */
    { d_agup_slider_proc,     185,  115,  100, 12,  0,    0,    0,   0,           8,    0,  NULL,             sfx_volume_callback,  NULL }, /* 16 */
    { d_yield_proc,           160,  132,  20,  8,   0,    0,    0,   0,           0,    0,  "MODS",           NULL,  NULL }, /* 17 */
    { d_yield_proc,           185,  130,  100, 12,  0,    0,    0,   0,           8,    0,  NULL,             NULL,  NULL }, /* 18 */
    { d_yield_proc,           160,  147,  20,  8,   0,    0,    0,   0,           0,    0,  "CD",             NULL,  NULL }, /* 19 */
    { d_yield_proc,           185,  145,  100, 12,  0,    0,    0,   0,           8,    0,  NULL,             NULL,  NULL }, /* 20 */

    { d_agup_push_proc,       20,   170,  130, 20,  0,    0,    0,   0,           0,    0,  stats_filename,   NULL,  push_stats_button }, /* 21 */

    { d_agup_button_proc,     170,  170,  60,  20,  0,    0,    0,   D_EXIT,      0,    0,  "ACCEPT",         NULL,  NULL }, /* 22 */
    { d_agup_button_proc,     245,  170,  60,  20,  0,    0,    27,  D_EXIT,      0,    0,  "REJECT",         NULL,  NULL }, /* 23 */

    { d_yield_proc,           0,    0,    0,   0,   0,    0,    0,   0,           0,    0,  NULL,             NULL,  NULL },
    { NULL,                   0,    0,    0,   0,   0,    0,    0,   0,           0,    0,  NULL,             NULL,  NULL },
};


#define I_RESLIST_	2
#define I_FILTERED	5
#define I_FAMILY	6
#define I_MUTESFX	8
#define I_NOMUSIC	9
#define I_PLAYMODULES_	10
#define I_PLAYCD_	11
#define I_RECORDREMOS	12
#define I_MOUSESPEED_	14
#define I_SFXVOLUME	16
#define I_MODVOLUME_	18
#define I_CDVOLUME_	20  /* removed */
#define I_STATS		21
#define I_ACCEPT	22
#define I_REJECT	23


static void set_D_SELECTED(DIALOG *d, int yes)
{
    if (yes)
	d->flags |= D_SELECTED;
    else
	d->flags &=~ D_SELECTED;
}


void options(void)
{
    int accepted;
    int old_sfx_volume = sfx_volume;
    
    old_font = font;
    font = dat[MINI].dat;

    push_stat_block();


    /* set up config_dlg */
    {
	set_D_SELECTED(config_dlg + I_MUTESFX, mute_sfx);

	set_D_SELECTED(config_dlg + I_RECORDREMOS, record_demos);

	config_dlg[I_SFXVOLUME].d2 = sfx_volume;

	strncpy(stats_filename, get_filename(current_stats), sizeof stats_filename);
	strncpy(stats_path, current_stats, sizeof stats_path);
    }

    
    /* run the dialog */
    set_dialog_color(config_dlg, agup_fg_color, agup_bg_color);
    centre_dialog(config_dlg);
    accepted = (do_dialog(config_dlg, -1) == I_ACCEPT);

    
    /* update settings */
    if (accepted) {
	
	mute_sfx = config_dlg[I_MUTESFX].flags & D_SELECTED;
	
	record_demos = config_dlg[I_RECORDREMOS].flags & D_SELECTED;
	
	sfx_volume = config_dlg[I_SFXVOLUME].d2;
	
	set_current_stats(stats_path);
    }
    else {
	sfx_volume = old_sfx_volume;
	pop_stat_block();
    }

    
    set_volume(sfx_volume * 32, -1);
    
    set_weapon_stats();
    
    font = old_font;

    /* In modes where not the full screen is used, we don't want the
     * top and bottom parts to be gray. */
    clear_bitmap(screen);
}


/* file settings */

static void open_cfg(void)
{
    push_config_state();
#ifdef TARGET_LINUX
    set_config_file(get_resource(R_HOME, ".redpixelrc"));
#else
    set_config_file(get_resource(R_HOME, "redpixel.cfg"));
#endif
}


static void close_cfg(void)
{
    pop_config_state();
}


static char *section = "redpixel";


void load_settings(void)
{
    open_cfg();
    
    mute_sfx = get_config_int(section, "mute_sfx", FALSE);
    record_demos = get_config_int(section, "record_demos", FALSE);
    set_current_stats((char *)get_config_string(section, "stats_file", "stats/default.st"));
    sfx_volume = get_config_int(section, "sfx_volume", 8); set_volume(sfx_volume * 32, -1);
    
    close_cfg();
}


void save_settings(void)
{
    open_cfg();

    set_config_int(section, "mute_sfx", mute_sfx);
    set_config_int(section, "record_demos", record_demos);
    set_config_string(section, "stats_file", current_stats);
    set_config_int(section, "sfx_volume", sfx_volume);

    close_cfg();
}
