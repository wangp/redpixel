#ifndef _included_statlist_h_
#define _included_statlist_h_

#define XXX(x) extern int st_##x;
#include "statlist.inc"
#undef XXX

extern STAT_VAR stat_block[];

#endif
