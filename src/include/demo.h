#ifndef __included_demo_h
#define __included_demo_h


#define DEMO_SET_RNG_SEED	's'
#define DEMO_MAP_CHANGE 	'm'
#define DEMO_FRAME_DATA		'f'
#define DEMO_END		'x'


extern char demo_description[];


int demo_read_open(char *filename);
void demo_read_header(void);
int32_t demo_read_seed(void);
int demo_read_next_packet_type(void);
void demo_read_packet(void *p, int len);
void demo_read_string(char *p, int len);
void demo_read_close(void);


int demo_write_open(char *filename, int players, char *names[]);
int demo_is_recording(void);
void demo_write_close(void);

void demo_write_set_rng_seed(int32_t seed);
void demo_write_change_map(char *filename);
void demo_write_frame_data(void *packet, int len);
void demo_write_player_inputs(void);


char *new_demo_filename(char *ppath);


#endif
