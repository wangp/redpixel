/* examp.c
 * 
 * This file is part of the Allegro GUI Un-uglification Project.
 * It is a cheap demonstration of how pretty we are.
 * 
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "agup.h"
#include "agtk.h"
#include "awin95.h"
#include "aase.h"


/*----------------------------------------------------------------------*/
/* Main dialog stuff							*/
/*----------------------------------------------------------------------*/


extern MENU menu_theme[];


int select_theme(void)
{
    int c;
    agup_shutdown();
    agup_init(active_menu->dp);

    for (c=0; menu_theme[c].proc; c++)
	menu_theme[c].flags &=~ D_SELECTED;
    active_menu->flags |= D_SELECTED;

    gui_fg_color = agup_fg_color;
    gui_bg_color = agup_bg_color;

    return D_REDRAW;
}


int file_selector(void)
{
    char path[1024] = "";
    file_select_ex("Testing...", path, NULL, sizeof path, 0, 0);
    return D_REDRAW;
}


int demo_windows(void)
{
    DIALOG dlg[] =
    {
	{ d_agup_window_proc, 0,   0,   320, 200, 0,             0,             0, 0, 0, 0, "A window",   NULL, NULL },
	{ d_ctext_proc,       160, 100, 0,   0,   agup_fg_color, agup_bg_color, 0, 0, 0, 0, "Press ESC.", NULL, NULL },
	{ NULL,               0,   0,   0,   0,   0,             0,             0, 0, 0, 0, NULL,         NULL, NULL }
    };
    
    centre_dialog(dlg);
    do_dialog(dlg, -1);

    return D_REDRAW;
}


int surprise()
{
    alert("You pushed my button!", "Isn't this a useful feature?", 0,
	  "&Yes", "&Definitely", 'y', 'd');

    return D_O_K;
}


int quit_proc(int msg, DIALOG *d, int c)
{
    return (((msg == MSG_KEY) || (msg == MSG_XCHAR && c == KEY_F10 << 8))
	    && alert(0, "Quit?", 0, "Ok", "Cancel", 0, 0) == 1 ? D_CLOSE : D_O_K);
}


AL_CONST char *lister(int index, int *list_size)
{
    if (index >= 0) {
	return "hi";
    }
    else {
	index = 10;
	*list_size = index;
    }

    return NULL;
}


char sel[10];
char sel2[10];
char buf[101] = "101";
char buf2[101] = "dalmations";
char buf3[1024] = ("textbox buffer, thaawfeebdaed kalsjd askjd xuuq"
		   "dassl!lhskld essdklsegassem lanimilbus zhjkasdh"
		   "kjash!! asdasd zxx,asd. ! XYZ");


MENU menu12[] =
{
    /* text		proc		child	flags		dp */
    { "Submenu",	0,		0,	0,		0 },
    { "",		0,		0,	0,		0 },
    { "Checked",	0,		0,	D_SELECTED,	0 },
    { "Disabled",	0,		0,	D_DISABLED,	0 },
    { NULL,		0,		0,	0,		0 },
};


MENU menu1[] =
{
    /* text		proc		child	flags	dp */
    { "&File selector",	file_selector,	0,	0,	0 },
    { "&Window frames", demo_windows,	0,	0,	0 },
    { "Test &1",	0,		0,	0,	0 },
    { "Test &2",	0,		menu12,	0,	0 },
    { "Test &3\thi",	0,		0,	0,	0 },
    { NULL,		0,		0,	0,	0 },
};


MENU menu_theme[] =
{
    /* text		proc		child	flags		dp */
    { "&GTK",		select_theme,	0,	D_SELECTED,	0 },
    { "&Win95",		select_theme,	0,	0,		0 },
    { "&ASE",		select_theme,	0,	0,		0 },
    { NULL,		0,		0,	0,		0 },
};


MENU menubar[] =
{
    /* text		proc	child		flags	dp */
    { "&Menu 1",	0,	menu1,		0,	0 },
    { "&Theme",		0,	menu_theme,	0,	0 },
    { NULL,		0,	0,		0,	0 },
};


#define DIS D_DISABLED
#define SEL D_SELECTED


DIALOG main_dlg[] =
{
    /* proc                   x     y     w    h    fg    bg    key  flags    d1    d2  dp            dp2    dp3 */
    { d_clear_proc,           0,    0,    0,   0,   0,    0,    0,   0,       0,    0,  0,            0,     0 }, /* 0 */
    { d_button_proc,          1,    20,   98,  20,  0,    0,    0,   0,       0,    0,  "button",     0,     0 }, /* 1 */
    { d_agup_button_proc,     103,  20,   98,  20,  0,    0,    0,   0,       0,    0,  "button",     0,     0 }, /* 2 */
    { d_agup_push_proc,       206,  20,   98,  20,  0,    0,    0,   0,       0,    0,  "button",     0,     surprise }, /* 3 */
    { d_agup_button_proc,     309,  20,   98,  20,  0,    0,    0,   DIS,     0,    0,  "button",     0,     0 }, /* 4 */
    { d_agup_button_proc,     309,  45,   98,  20,  0,    0,    0,   DIS|SEL, 0,    0,  "button",     0,     0 }, /* 5 */
    { d_check_proc,           1,    45,   98,  20,  0,    0,    0,   0,       0,    0,  "check box",  0,     0 }, /* 6 */
    { d_agup_check_proc,      103,  45,   98,  20,  0,    0,    0,   0,       0,    0,  "check box",  0,     0 }, /* 7 */
    { d_radio_proc,           1,    69,   98,  20,  0,    0,    0,   0,       1,    1,  "radio 1",    0,     0 }, /* 8 */
    { d_agup_radio_proc,      103,  69,   98,  20,  0,    0,    0,   0,       1,    0,  "radio 2",    0,     0 }, /* 9 */
    { d_radio_proc,           1,    93,   98,  20,  0,    0,    0,   0,       1,    2,  "radio 3",    0,     0 }, /* 10 */
    { d_agup_radio_proc,      103,  93,   98,  20,  0,    0,    0,   0,       1,    0,  "radio 4",    0,     0 }, /* 11 */
    { d_shadow_box_proc,      1,    117,  98,  20,  0,    0,    0,   0,       0,    0,  0,            0,     0 }, /* 12 */
    { d_agup_shadow_box_proc, 103,  117,  98,  20,  0,    0,    0,   0,       0,    0,  0,            0,     0 }, /* 13 */
    { d_icon_proc,            206,  45,   48,  20,  0,    0,    0,   0,       2,    2,  0,            0,     0 }, /* 14 */
    { d_agup_icon_proc,       256,  45,   48,  20,  0,    0,    0,   0,       2,    2,  0,            0,     0 }, /* 15 */
    { d_edit_proc,            1,    156,  98,  14,  0,    0,    0,   0,       100,  0,  buf,          0,     0 },
    { d_agup_edit_proc,       103,  154,  98,  14,  0,    0,    0,   0,       100,  0,  buf2,         0,     0 },
    { d_text_list_proc,       1,    170,  98,  48,  0,    0,    0,   0,       2,    2,  lister,       sel,   0 },
    { d_agup_list_proc,       103,  170,  98,  48,  0,    0,    0,   0,       2,    2,  lister,       sel2,  0 },
    { d_agup_textbox_proc,    206,  70,   100, 100, 0,    0,    0,   0,       0,    0,  buf3,         0,     0 },
    { d_slider_proc,          1,    140,  98,  12,  0,    0,    0,   0,       100,  0,  0,            0,     0 },
    { d_agup_slider_proc,     103,  140,  98,  12,  0,    0,    0,   0,       100,  0,  0,            0,     0 },
    { d_agup_menu_proc,       1,    1,    0,   0,   0,    0,    0,   0,       0,    0,  menubar,      0,     0 },
    { quit_proc,              0,    0,    0,   0,   0,    0,    27,  0,       0,    0,  0,            0,     0 },
    { d_yield_proc,           0,    0,    0,   0,   0,    0,    0,   0,       0,    0,  0,            0,     0 },
    { NULL,                   0,    0,    0,   0,   0,    0,    0,   0,       0,    0,  0,            0,     0 }
};


void do_main_dialog()
{
    BITMAP *icon;

    agup_init(agtk_theme);

    gui_fg_color = agup_fg_color;
    gui_bg_color = agup_bg_color;
    gui_shadow_box_proc = d_agup_shadow_box_proc;
    gui_button_proc = d_agup_button_proc;
    gui_edit_proc = d_agup_edit_proc;
    gui_text_list_proc = d_agup_text_list_proc;

    menu_theme[0].dp = (void *)agtk_theme;
    menu_theme[1].dp = (void *)awin95_theme;
    menu_theme[2].dp = (void *)aase_theme;

    icon = load_bitmap("gem.pcx", 0);
    main_dlg[14].dp = icon;
    main_dlg[15].dp = icon;
    
    set_dialog_color(main_dlg, makecol(0, 0, 0), makecol(255, 255, 255));
    
    do_dialog(main_dlg, -1);

    destroy_bitmap(icon);
    agup_shutdown();
}


/*----------------------------------------------------------------------*/
/* Main  								*/
/*----------------------------------------------------------------------*/


int init()
{
    allegro_init();
    install_timer();
    install_keyboard();
    install_mouse();
    
    set_window_title("Widgets test");
    set_color_depth(16);
    if (set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0) < 0) {
	allegro_message("Could not set graphics mode.\n");
	return -1;
    }
    
    return 0;
}


int main(void)
{
    if (init() < 0)
	return -1;
    show_mouse(screen);
    do_main_dialog();
    return 0;
}

END_OF_MAIN();
