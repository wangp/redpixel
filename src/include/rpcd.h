#ifndef __included_rpcd_h
#define __included_rpcd_h


void rpcd_init(void);
void rpcd_shutdown(void);
int rpcd_get_volume(void);
void rpcd_set_volume(int);
void rpcd_play_random_track(void);
void rpcd_poll(void);
void rpcd_stop(void);


#endif
