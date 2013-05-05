/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  AGUP wrapper.
 */


#include "a4aux.h"
#include "agup.h"
#include "agtk.h"
#include "awin95.h"


static int agup_inited = 0;


void rpagup_init(void)
{
#ifdef TARGET_WINDOWS
    agup_init(awin95_theme);
#else
    agup_init(agtk_theme);
#endif
    gui_fg_color = agup_fg_color;
    gui_bg_color = agup_bg_color;
    gui_shadow_box_proc = d_agup_shadow_box_proc;
    gui_button_proc = d_agup_button_proc;
    gui_edit_proc = d_agup_edit_proc;
    gui_text_list_proc = d_agup_text_list_proc;
    agup_inited = 1;
}


void rpagup_shutdown(void)
{
    if (agup_inited) {
	/* should really restore gui_*, but doesn't matter */
	agup_shutdown();
	agup_inited = 0;
    }
}


