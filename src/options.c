/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Options part of menu.
 */


#include <allegro.h>
#include "agup.h"
#include "blood.h"
#include "mousespr.h"
#include "music.h"
#include "globals.h"
#include "rpagup.h"
#include "suicide.h"
#include "vidmode.h"


static FONT *old_font;


static char *res_list(int index, int *list_size)
{
    /* see include/vidmode.h */
    static char *res[] =
    {
	"320x200 FULLSCREEN",
	"320x240 FULLSCREEN",
	"640x400 FULLSCREEN",
	"640x480 FULLSCREEN",
	"640x400 WINDOWED",
	"320x200 WINDOWED",	
    };
    if (index >= 0)
	return res[index];
    else {
	*list_size = sizeof(res) / sizeof(res[0]);
	return NULL;
    }
}


static int mouse_speed_callback(void *dp3, int d2)
{
    set_mouse_speed(d2, d2);
    return D_O_K;
}


static int push_stats(DIALOG *d)
{
    char path[1024] = "";
    FONT *save = font;
    (void)d;
    font = old_font;
    file_select_ex("Use stats file...", path, NULL, sizeof path, 0, 0);
    font = save;
    return D_O_K;
}


DIALOG config_dlg[] =
{
    /* proc                   x     y     w    h    fg    bg    key  flags        d1    d2  dp                dp2    dp3 */
    { d_clear_proc,           0,    0,    0,   0,   0,    0,    0,   0,           0,    0,  NULL,             NULL,  NULL },/* 0 */
    { d_text_proc,            10,   10,   0,   0,   0,    0,    0,   0,           0,    0,  "DESIRED RESOLUTION", NULL,  NULL }, /* 1 */
    { d_agup_list_proc,       10,   20,   140, 36,  0,    0,    0,   0,           0,    0,  res_list,         NULL,  NULL }, /* 2 */

    { d_agup_box_proc,        165,  10,   130, 46,  0,    0,    0,   0,           0,    0,  NULL,             NULL,  NULL }, /* 3 */
    { d_agup_check_proc,      170,  15,   120, 16,  0,    0,    0,   D_SELECTED,  0,    0,  "SCANLINES (HI-RES)", NULL,  NULL }, /* 4 */
    { d_agup_check_proc,      170,  32,   120, 16,  0,    0,    0,   0,           0,    0,  "FILTERED",       NULL,  NULL }, /* 5 */

    { d_agup_box_proc,        10,   60,   300, 100, 0,    0,    0,   0,           0,    0,  NULL,             NULL,  NULL }, /* 6 */

    { d_agup_check_proc,      15,   65,   130, 20,  0,    0,    0,   0,           0,    0,  "MUTE SOUND EFFECTS", NULL,  NULL }, /* 7 */
    { d_agup_radio_proc,      15,   90,   130, 20,  0,    0,    0,   0,           1,    0,  "NO MUSIC",       NULL,  NULL }, /* 8 */
    { d_agup_radio_proc,      15,   110,  130, 20,  0,    0,    0,   0,           1,    0,  "PLAY MODULES",   NULL,  NULL }, /* 9 */
    { d_agup_radio_proc,      15,   130,  130, 20,  0,    0,    0,   0,           1,    0,  "PLAY CD",        NULL,  NULL }, /* 10 */

    { d_agup_check_proc,      180,  65,   100, 20,  0,    0,    0,   0,           0,    0,  "RECORD DEMOS",   NULL,  NULL }, /* 11 */

    { d_text_proc,            160,  90,   40,  8,   0,    0,    0,   0,           10,   0,  "MOUSE SPEED",    NULL,  NULL }, /* 12 */
    { d_agup_slider_proc,     225,  88,   60,  12,  0,    0,    0,   0,           3,    0,  NULL,             mouse_speed_callback,  NULL }, /* 13 */

    { d_text_proc,            160,  112,  20,  8,   0,    0,    0,   0,           10,   0,  "SFX",            NULL,  NULL }, /* 14 */
    { d_agup_slider_proc,     185,  110,  100, 12,  0,    0,    0,   0,           10,   0,  NULL,             NULL,  NULL }, /* 15 */
    { d_text_proc,            160,  127,  20,  8,   0,    0,    0,   0,           10,   0,  "MODS",           NULL,  NULL }, /* 16 */
    { d_agup_slider_proc,     185,  125,  100, 12,  0,    0,    0,   0,           10,   0,  NULL,             NULL,  NULL }, /* 17 */
    { d_text_proc,            160,  142,  20,  8,   0,    0,    0,   0,           10,   0,  "CD",             NULL,  NULL }, /* 18 */
    { d_agup_slider_proc,     185,  140,  100, 12,  0,    0,    0,   0,           10,   0,  NULL,             NULL,  NULL }, /* 19 */

    { d_agup_push_proc,       20,   170,  130, 20,  0,    0,    0,   0,           0,    0,  "stats.09",       NULL,  push_stats }, /* 20 */

    { d_agup_button_proc,     170,  170,  60,  20,  0,    0,    0,   D_EXIT,      0,    0,  "ACCEPT",         NULL,  NULL }, /* 21 */
    { d_agup_button_proc,     245,  170,  60,  20,  0,    0,    27,  D_EXIT,      0,    0,  "REJECT",         NULL,  NULL }, /* 22 */

    { d_yield_proc,           0,    0,    0,   0,   0,    0,    0,   0,           0,    0,  NULL,             NULL,  NULL },
    { NULL,                   0,    0,    0,   0,   0,    0,    0,   0,           0,    0,  NULL,             NULL,  NULL },
};


#define I_RESLIST	2
#define I_SCANLINES	4
#define I_FILTERED	5
#define I_NOMUSIC	8
#define I_PLAYMODULES	9
#define I_PLAYCD	10
#define I_RECORDREMOS	11
#define I_MOUSESPEED	13
#define I_ACCEPT	21
#define I_REJECT	22


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
    int old_desired_video_mode = desired_video_mode;
    int old_want_scanlines = want_scanlines;
    
    old_font = font;
    font = dat[MINI].dat;

    /* set up config_dlg */
    {
	config_dlg[I_RESLIST].d1 = desired_video_mode;
	set_D_SELECTED(config_dlg + I_SCANLINES, want_scanlines);
	set_D_SELECTED(config_dlg + I_FILTERED, filtered);

	{
	    config_dlg[I_NOMUSIC].flags &=~ D_SELECTED;
	    config_dlg[I_PLAYMODULES].flags &=~ D_SELECTED;
	    config_dlg[I_PLAYCD].flags &=~ D_SELECTED;
	    switch (music_get_format()) {
		case MUSIC_FMT_NONE: config_dlg[I_NOMUSIC].flags |= D_SELECTED; break;
		case MUSIC_FMT_MOD: config_dlg[I_PLAYMODULES].flags |= D_SELECTED; break;
		case MUSIC_FMT_CD: config_dlg[I_PLAYCD].flags |= D_SELECTED; break;
	    }
	}
	    
	set_D_SELECTED(config_dlg + I_RECORDREMOS, record_demos);

	config_dlg[I_MOUSESPEED].d2 = mouse_speed;
    }

    
    /* run the dialog */
    set_dialog_color(config_dlg, agup_fg_color, agup_bg_color);
    centre_dialog(config_dlg);
    accepted = (do_dialog(config_dlg, -1) == I_ACCEPT);

    
    /* update settings */
    if (accepted)
    {
	desired_video_mode = config_dlg[I_RESLIST].d1;
	want_scanlines = config_dlg[I_SCANLINES].flags & D_SELECTED;
	filtered = config_dlg[I_FILTERED].flags & D_SELECTED;

	{
	    if (config_dlg[I_NOMUSIC].flags & D_SELECTED)
		music_set_format(MUSIC_FMT_NONE);
	    else if (config_dlg[I_PLAYMODULES].flags & D_SELECTED)
		music_set_format(MUSIC_FMT_MOD);
	    else if (config_dlg[I_PLAYCD].flags & D_SELECTED)
		music_set_format(MUSIC_FMT_CD);
	}
	
	record_demos = config_dlg[I_RECORDREMOS].flags & D_SELECTED;

	mouse_speed = config_dlg[I_MOUSESPEED].d2;
    }

    
    if ((old_desired_video_mode != desired_video_mode) ||
	(old_want_scanlines != want_scanlines)) {
	rpagup_shutdown();
	if (set_desired_video_mode_or_fallback() < 0)
	    suicide("Error setting video mode");
	set_palette(dat[GAMEPAL].dat);
	set_stretched_mouse_sprite(dat[XHAIRLCD].dat, (SCREEN_W == 640) ? 2 : 1, 2, 2);
	rpagup_init();
    }

    set_mouse_speed(mouse_speed, mouse_speed);

    
    font = old_font;

    /* In modes where not the full screen is used, we don't want the
     * top and bottom parts to be gray. */
    clear_bitmap(screen);
}
