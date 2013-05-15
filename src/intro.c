/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Game introduction.
 *  Code converted from a SeeR script.
 *  Hacked to pieces to synchronise with music.
 */


#include <stdio.h>
#include "a4aux.h"
#include "blood.h"
#include "globals.h"
#include "resource.h"
#include "vidmode.h"



static double now(void)
{
    return al_get_time();
}

static double elapsed_time(double t0)
{
    return now() - t0;
}

static void wait_until(double t)
{
    al_rest(t - now());
}



static volatile int frames;

static void frame_ticker(void)
{
    frames++;
}



static int keypressed_allow_fullscreen(void)
{
    if (keypressed()) {
	if (readkey() >> 8 == KEY_F11)
	    toggle_fullscreen_window();
	else
	    return TRUE;
    }
    return FALSE;
}



static ALLEGRO_AUDIO_STREAM *start_music(const char *filename)
{
    ALLEGRO_MIXER *mixer;
    ALLEGRO_AUDIO_STREAM *stream;
    char buf[1024];

    mixer = al_get_default_mixer();
    if (!mixer)
	return NULL;

    snprintf(buf, sizeof buf, "%s/%s", get_resource(R_SHARE, "music"), filename);
    stream = al_load_audio_stream(buf, 2, 2048);
    if (stream) {
	al_attach_audio_stream_to_mixer(stream, mixer);
    }
    return stream;
}



static void stop_music(ALLEGRO_AUDIO_STREAM *stream)
{
    if (stream) {
	al_set_audio_stream_playing(stream, false);
	al_destroy_audio_stream(stream);
    }
}



/* duration to fit music: two seconds */
static int raster_words(char *s)
{
    double t0;
    BITMAP *txt1;
    BITMAP *txt2;
    int x, y;
    int w, h;
    int nopress = 0;

    t0 = now();
    LOCK_VARIABLE(frames);
    LOCK_FUNCTION(frame_ticker);
    install_int_ex(frame_ticker, BPS_TO_TIMER(60));

    /* create text images */
    w = text_length(dat[UNREAL].dat, s);
    h = text_height(dat[UNREAL].dat);
    if ((w % 2) == 0) w++;

    txt1 = create_bitmap(w, h);
    txt2 = create_bitmap(w, h);
    clear_bitmap(txt1);
    clear_bitmap(txt2);
    textout_ex(txt1, dat[UNREAL].dat, s, 0, 0, -1, -1);
    textout_ex(txt2, dat[UNREAL].dat, s, 0, 0, -1, -1);
    /* Have to use Allegro 5 to draw transparent lines. */
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
    al_set_target_bitmap(txt1->real);
    for (y = 0; y < h; y += 2) {
	al_draw_line(0, y + 0.5, w, y + 0.5, al_map_rgba(0, 0, 0, 0), 1.0);
    }
    al_set_target_bitmap(txt2->real);
    for (y = 0; y < h; y += 2) {
	al_draw_line(0, y + 1.5, w, y + 1.5, al_map_rgba(0, 0, 0, 0), 1.0);
    }
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

    y = 100 - h / 2;
    x = -w;

    /* text in */
    frames = 1;
    while (x < 160 - w/2) {
	if (frames > 0) {
	    x += 6;
	    if (--frames == 0) {
		clear_bitmap(dbuf);
		draw_sprite(dbuf, txt1, x, y);
		draw_sprite(dbuf, txt2, 319 - x - w, y);
		blit_to_screen(dbuf);
	    }
	}
	
	if (keypressed_allow_fullscreen())
	    goto quit;
    }

    /* pause a little */
    x = 160 - w/2 - 1;
    clear_bitmap(dbuf);
    draw_sprite(dbuf, txt1, x, y);
    draw_sprite(dbuf, txt2, 319 - x - w, y);
    blit_to_screen(dbuf);
    wait_until(t0 + 1.5);

    /* text out */
    frames = 1;
    while (x < 320) {
	if (frames > 0) {
	    x += 6;
	    if (--frames == 0) {
		clear_bitmap(dbuf);
		draw_sprite(dbuf, txt1, x, y);
		draw_sprite(dbuf, txt2, 319 - x - w, y);
		blit_to_screen(dbuf);
	    }
	}
	
	if (keypressed_allow_fullscreen())
	    goto quit;
    } 

    /* pause a little */
    clear_bitmap(screen);
    wait_until(t0 + 2.0);

    nopress = 1;

  quit:

    destroy_bitmap(txt1);
    destroy_bitmap(txt2);
    
    remove_int(frame_ticker);
    
    return nopress;
}


static int scan(int x, int y)
{
    BITMAP *bmp;
    int x2, i, j = 0;
    double t0;
    int keyed = 0;

    t0 = now();
    
    x2 = x + 60;

    bmp = create_bitmap(320, 200);
    clear_bitmap(bmp);

    do {
	j = (j) ? 0 : 1;

	for (i = j; i < 80; i += 2)
	    blit(dat[TITLE].dat, bmp, x, y + i, 120, 60 + i, 80, 1);

	blit_to_screen(bmp);
	x++;
	rest(60);
	if (keypressed_allow_fullscreen()) {
	    keyed = 1;
            break;
	}
    } while ((x < x2) && (elapsed_time(t0) < 3.6));

    destroy_bitmap(bmp);
    
    return !keyed;
}


void intro(void)
{
    ALLEGRO_AUDIO_STREAM *stream;
    int x, y, x2, y2;
    
    clear_keybuf();

    stream = start_music("specific/present.xm");

    if (raster_words("1998")
	&& scan(10, 100) 
	&& raster_words("PSYK SOFTWARE") 
	&& scan(160, 90)
	&& raster_words("PRESENTS")) {

	double t0 = now();
	double step = 0.0;

	x = 159; x2 = 160;
	y = 99;  y2 = 100;

	do {
	    clear_bitmap(dbuf);
	    blit(dat[TITLE].dat, dbuf, x, y, x, y, x2-x, y2-y);
	    blit_to_screen(dbuf);

	    do {
		step += 8.0/1000.0;
		x--; x2++;
		y--; y2++;
	    } while (step < elapsed_time(t0));
	    wait_until(t0 + step);
	} while (x >= 0);

	al_rest(1.6);
    }

    rp_fade_out(dbuf, 6);

    clear_keybuf();

    stop_music(stream);
}
