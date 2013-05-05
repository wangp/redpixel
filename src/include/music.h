#ifndef __included_music_h
#define __included_music_h


#define MUSIC_FMT_NONE	0
#define MUSIC_FMT_MOD	1


void music_init(void);
void music_shutdown(void);
int music_get_format(void);
void music_set_format(int);
void music_play_random_track(void);
void music_poll(void);
void music_stop(void);


#endif
