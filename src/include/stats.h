#ifndef __included_stats_h
#define __included_stats_h

enum STAT_TYPE { 
    ST_INT = 0, 
    ST_FLOAT
};

typedef struct {
    char *id;
    enum STAT_TYPE type;
    void *p;
} STAT_VAR;

int read_stats(char *fn, STAT_VAR *block);
void for_every_stat(STAT_VAR *block, int (*proc)(STAT_VAR *sv));


#endif 
