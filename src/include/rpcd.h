#ifndef __included_rpcd_h
#define __included_rpcd_h


void rpcd_init();
void rpcd_shutdown();
int rpcd_get_volume();
void rpcd_set_volume(int);
void rpcd_play_random_track();
void rpcd_poll();
void rpcd_stop();


#endif
