#ifndef __include_options_h
#define __include_options_h


extern int mouse_speed;
extern int record_demos;
extern int filtered;
extern int family;
extern int mute_sfx;


void options(void);


void load_settings();
void save_settings();


#endif
