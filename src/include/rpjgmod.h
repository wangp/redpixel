#ifndef __included_rpjgmod_h
#define __included_rpjgmod_h


void rpjgmod_init(void);
void rpjgmod_shutdown(void);
void rpjgmod_set_volume(int);
void rpjgmod_play_random_track(void);
void rpjgmod_play_specific_track(char *);
void rpjgmod_poll(void);
void rpjgmod_stop(void);


#endif
