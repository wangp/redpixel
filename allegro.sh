/*
       -= SEER - C-Scripting engine v 0.4a =-
State      : under implementation
Author     : Przemyslaw Podsiadly
	     email: ppodsiad@elka.pw.edu.pl
	     WWW:   http://home.elka.pw.edu.pl/~ppodsiad
File:allegro.sh
Desc:SeeR interface to Allegro 3 by Shawn Hargreaves
	  http://www.talula.demon.co.uk/allegro/
	  (not complete)
*/

typedef void BITMAP;
typedef void MIDI;
typedef void SAMPLE;
typedef void FONT;
typedef void PACKFILE;

import int allegro_init();
import void allegro_exit();
import int install_mouse();
import int install_timer();
import int install_keyboard();

#define GFX_TEXT                 -1
#define GFX_AUTODETECT           0
#define GFX_VGA                  1
#define GFX_MODEX                2

import BITMAP *screen;
import void set_color_depth(int depth);
import void set_color_conversion(int mode);
import int set_gfx_mode(int card, int w, int h, int v_w, int v_h);
import BITMAP *create_bitmap(int width, int height);
import BITMAP *create_bitmap_ex(int color_depth, int width, int height);
import BITMAP *create_sub_bitmap(BITMAP *parent, int x, int y, int width, int height);
import BITMAP *create_video_bitmap(int width, int height);
import void destroy_bitmap(BITMAP *bitmap);

#define PAL_SIZE     256

typedef int RGB;
typedef RGB PALLETE[PAL_SIZE];

import void set_color(int index, RGB *p);
import void set_pallete(PALLETE p);
import void set_pallete_range(PALLETE p, int from, int to, int vsync);
import void get_color(int index, RGB *p);
import void get_pallete(PALLETE p);
import void get_pallete_range(PALLETE p, int from, int to);
import void generate_332_palette(PALLETE pal);
import int generate_optimized_palette(BITMAP *image, PALLETE pal, char rsvdcols[256]);
import int bestfit_color(PALLETE pal, int r, int g, int b);
import int makecol(int r, int g, int b);
import int makecol8(int r, int g, int b);
import int makecol_depth(int color_depth, int r, int g, int b);
import int getr(int c);
import int getg(int c);
import int getb(int c);
import int getr_depth(int color_depth, int c);
import int getg_depth(int color_depth, int c);
import int getb_depth(int color_depth, int c);

#define PALETTE                        PALLETE
#define black_palette                  black_pallete
#define desktop_palette                desktop_pallete
#define set_palette                    set_pallete
#define get_palette                    get_pallete
#define set_palette_range              set_pallete_range
#define get_palette_range              get_pallete_range
#define fli_palette                    fli_pallete
#define palette_color                  pallete_color
#define DAT_PALETTE                    DAT_PALLETE
#define select_palette                 select_pallete
#define unselect_palette               unselect_pallete
#define generate_332_pallete           generate_332_palette
#define generate_optimised_pallete     generate_optimised_palette

import void putpixel(BITMAP *bmp, int x, int y, int color);
import void set_clip(BITMAP *bitmap, int x1, int y1, int x2, int y2);
import void drawing_mode(int mode, BITMAP *pattern, int x_anchor, int y_anchor);
import void xor_mode(int xor);
import void solid_mode();
import void do_line(BITMAP *bmp, int x1, int y1, int x2, int y2, int d, void (*proc)(BITMAP *, int, int, int));
import void triangle(BITMAP *bmp, int x1, int y1, int x2, int y2, int x3, int y3, int color);
import void polygon(BITMAP *bmp, int vertices, int *points, int color);
import void rect(BITMAP *bmp, int x1, int y1, int x2, int y2, int color);
import void do_circle(BITMAP *bmp, int x, int y, int radius, int d, void (*proc)(BITMAP *, int, int, int));
import void circle(BITMAP *bmp, int x, int y, int radius, int color);
import void circlefill(BITMAP *bmp, int x, int y, int radius, int color);
import void do_ellipse(BITMAP *bmp, int x, int y, int rx, int ry, int d, void (*proc)(BITMAP *, int, int, int));
import void ellipse(BITMAP *bmp, int x, int y, int rx, int ry, int color);
import void ellipsefill(BITMAP *bmp, int x, int y, int rx, int ry, int color);
import void calc_spline(int *points, int npts, int *x, int *y);
import void spline(BITMAP *bmp, int points[8], int color);
import void floodfill(BITMAP *bmp, int x, int y, int color);
import void blit(BITMAP *source, BITMAP *dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height);
import void masked_blit(BITMAP *source, BITMAP *dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height);
import void stretch_blit(BITMAP *s, BITMAP *d, int s_x, int s_y, int s_w, int s_h, int d_x, int d_y, int d_w, int d_h);
import void stretch_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, int w, int h);
import void rotate_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, fixed angle);
import void rotate_scaled_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, fixed angle, fixed scale);
import void draw_gouraud_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, int c1, int c2, int c3, int c4);
import void clear(BITMAP *bitmap);

import void text_mode(int mode);
import void textout(BITMAP *bmp, FONT *f,  char *str, int x, int y, int color);
import void textout_centre(BITMAP *bmp, FONT *f,  char *str, int x, int y, int color);
import void textout_justify(BITMAP *bmp, FONT *f,  char *str, int x1, int x2, int y, int diff, int color);
import void textprintf(BITMAP *bmp, FONT *f, int x, int y, int color, char *format, ...);
import void textprintf_centre(BITMAP *bmp, FONT *f, int x, int y, int color, char *format, ...);
import int text_length(FONT *f,  char *str);
import int text_height(FONT *f);
import void destroy_font(FONT *f);

import int play_fli(char *filename, BITMAP *bmp, int loop, int (*callback)());
import int play_memory_fli(void *fli_data, BITMAP *bmp, int loop, int (*callback)());
import int open_fli(char *filename);
import int open_memory_fli(void *fli_data);
import void close_fli();
import int next_fli_frame(int loop);
import void reset_fli_variables();
import BITMAP *fli_bitmap;             /* current frame of the FLI */
import PALLETE fli_pallete;            /* current FLI pallete */

import int fli_frame;                  /* current frame number */

#define DIGI_AUTODETECT       -1
#define DIGI_NONE             0

#define MIDI_AUTODETECT       -1
#define MIDI_NONE             0

import int install_sound(int digi_card, int midi_card, char *cfg_path);
import void set_volume(int digi_volume, int midi_volume);

import SAMPLE *load_sample(char *filename);
import SAMPLE *load_wav(char *filename);
import SAMPLE *load_voc(char *filename);
import SAMPLE *create_sample(int bits, int stereo, int freq, int len);
import void destroy_sample(SAMPLE *spl);

import int play_sample(SAMPLE *spl, int vol, int pan, int freq, int loop);
import void stop_sample(SAMPLE *spl);
import void adjust_sample(SAMPLE *spl, int vol, int pan, int freq, int loop);

import MIDI *load_midi(char *filename);
import void destroy_midi(MIDI *midi);
import int play_midi(MIDI *midi, int loop);
import int play_looped_midi(MIDI *midi, int loop_start, int loop_end);
import void stop_midi();
import void midi_pause();
import void midi_resume();
import int midi_seek(int target);
import void midi_out( char *data, int length);
import int load_midi_patches();
//FILES:
import void packfile_password(char *password);
import PACKFILE *pack_fopen(char *filename, char *mode);
import int pack_fclose(PACKFILE *f);
import int pack_fseek(PACKFILE *f, int offset);
import PACKFILE *pack_fopen_chunk(PACKFILE *f, int pack);
import PACKFILE *pack_fclose_chunk(PACKFILE *f);
import int pack_igetw(PACKFILE *f);
import long pack_igetl(PACKFILE *f);
import int pack_iputw(int w, PACKFILE *f);
import long pack_iputl(long l, PACKFILE *f);
import int pack_mgetw(PACKFILE *f);
import long pack_mgetl(PACKFILE *f);
import int pack_mputw(int w, PACKFILE *f);
import long pack_mputl(long l, PACKFILE *f);
import long pack_fread(void *p, long n, PACKFILE *f);
import long pack_fwrite(void *p, long n, PACKFILE *f);
import char *pack_fgets(char *p, int max, PACKFILE *f);
import int pack_fputs(char *p, PACKFILE *f);

import int key[128];

import void vsync();

#define KEY_ESC               1     /* keyboard scan codes  */
#define KEY_1                 2 
#define KEY_2                 3 
#define KEY_3                 4
#define KEY_4                 5
#define KEY_5                 6
#define KEY_6                 7
#define KEY_7                 8
#define KEY_8                 9
#define KEY_9                 10
#define KEY_0                 11
#define KEY_MINUS             12
#define KEY_EQUALS            13
#define KEY_BACKSPACE         14
#define KEY_TAB               15 
#define KEY_Q                 16
#define KEY_W                 17
#define KEY_E                 18
#define KEY_R                 19
#define KEY_T                 20
#define KEY_Y                 21
#define KEY_U                 22
#define KEY_I                 23
#define KEY_O                 24
#define KEY_P                 25
#define KEY_OPENBRACE         26
#define KEY_CLOSEBRACE        27
#define KEY_ENTER             28
#define KEY_CONTROL           29
#define KEY_LCONTROL          29
#define KEY_A                 30
#define KEY_S                 31
#define KEY_D                 32
#define KEY_F                 33
#define KEY_G                 34
#define KEY_H                 35
#define KEY_J                 36
#define KEY_K                 37
#define KEY_L                 38
#define KEY_COLON             39
#define KEY_QUOTE             40
#define KEY_TILDE             41
#define KEY_LSHIFT            42
#define KEY_BACKSLASH         43
#define KEY_Z                 44
#define KEY_X                 45
#define KEY_C                 46
#define KEY_V                 47
#define KEY_B                 48
#define KEY_N                 49
#define KEY_M                 50
#define KEY_COMMA             51
#define KEY_STOP              52
#define KEY_SLASH             53
#define KEY_RSHIFT            54
#define KEY_ASTERISK          55
#define KEY_ALT               56
#define KEY_SPACE             57
#define KEY_CAPSLOCK          58
#define KEY_F1                59
#define KEY_F2                60
#define KEY_F3                61
#define KEY_F4                62
#define KEY_F5                63
#define KEY_F6                64
#define KEY_F7                65
#define KEY_F8                66
#define KEY_F9                67
#define KEY_F10               68
#define KEY_NUMLOCK           69
#define KEY_SCRLOCK           70
#define KEY_HOME              71
#define KEY_UP                72
#define KEY_PGUP              73
#define KEY_MINUS_PAD         74
#define KEY_LEFT              75
#define KEY_5_PAD             76
#define KEY_RIGHT             77
#define KEY_PLUS_PAD          78
#define KEY_END               79
#define KEY_DOWN              80
#define KEY_PGDN              81
#define KEY_INSERT            82
#define KEY_DEL               83
#define KEY_PRTSCR            84
#define KEY_F11               87
#define KEY_F12               88
#define KEY_LWIN              91
#define KEY_RWIN              92
#define KEY_MENU              93
#define KEY_PAD               100
#define KEY_RCONTROL          120
#define KEY_ALTGR             121
#define KEY_SLASH2            122
#define KEY_PAUSE             123

import void rest(long);

import void fade_out(int);
import void fade_in(PALETTE *, int);

import void hline(BITMAP *, int, int, int, int);
import void draw_sprite(BITMAP *, BITMAP *, int, int);

import void clear_keybuf();
import int keypressed();