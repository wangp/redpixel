/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Simple module to handle simple ``stats'' files
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "stats.h"



static STAT_VAR *stblock = NULL;



static STAT_VAR *search(char *id)
{
    STAT_VAR *sv = stblock;
    while (sv->id) {
	if (strcmp(id, sv->id) == 0)
	  return sv;
	sv++;
    }
    return NULL;
}



static void set_value(STAT_VAR *sv, char *value)
{
    switch (sv->type) {
	case ST_INT:
	*((int *)sv->p) = atoi(value);
	break;
	
	case ST_FLOAT:
	*((float *)sv->p) = atof(value);
	break;
    }
}



static void parse_line(char *line)
{
    static char *sep = "\t,;( )=";
    char *copy, *p;
    char *id, *val;
    STAT_VAR *sv;
    int first_go = 1;
        
    copy = strdup(line);
    p = copy;
    
    for (;;) {
	if (first_go) {
	    id = strtok(copy, sep);
	    first_go = 0;
	} 
	else {
	    id = strtok(NULL, sep);
	}
	val = strtok(NULL, sep);
	
	if (!id || !val)
	  break;
	
	sv = search(id);
	if (sv) 
	  set_value(sv, val);
    }
    
    free(copy);
}



static char *chop(char *s)
{
    char *p = s;
    while (*p) {
	if (*p == '#' || *p == '\r' || *p == '\n')
	  *p = 0;
	else
	  p++;
    }
    return s;
}



static void read_stats_fp(FILE *fp)
{
    char buf[1024];
    while (fgets(buf, 1024, fp)) {
	if (strlen(chop(buf)))
	  parse_line(buf);
    }
}



int read_stats(char *fn, STAT_VAR *block)
{
    FILE *fp;
    struct stat buf;
    if ((stat(fn, &buf) < 0) || !S_ISREG(buf.st_mode))
	return 0;
    fp = fopen(fn, "r");
    if (!fp) 
      return 0;
    stblock = block;
    read_stats_fp(fp);
    fclose(fp);
    return 1;
}



void for_every_stat(STAT_VAR *block, int (*proc)(STAT_VAR *sv))
{
    STAT_VAR *sv = block;

    while (sv->id) {
	if (proc(sv))
	    break;
	sv++;
    }
}
