#ifndef __included_message_h
#define __included_message_h


void add_msg(char *s, int to_player);
void add_msgf(int target, char *fmt, ...);

void update_msgs(void);
void draw_msgs(void);
void reset_msgs(void);


#endif
