#ifndef __included_statlist_h
#define __included_statlist_h


#define XXX(x) extern int st_##x;
#include "statlist.inc"
#undef XXX

extern STAT_VAR stat_block[];


void push_stat_block();
void pop_stat_block();


#endif
