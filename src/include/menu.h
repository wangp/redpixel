#ifndef __included_menu_h
#define __included_menu_h


typedef struct BLUBBER {
    void (*proc)(struct BLUBBER *bp, int command, int ex);
    char *string;
    void *thing;
    int d1, d2;
} BLUBBER;


extern int menu_end;


void prev_menu(BLUBBER *bp, int command, int ex);
void enter_menu(BLUBBER *bp);
void join_menu(BLUBBER *bp, int command, int ex);
void menu_proc(BLUBBER *bp, int command, int ex);


void blubber(BLUBBER *start);


#endif
