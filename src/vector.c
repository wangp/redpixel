/* 
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
 * 
 *  Vectors.
 */


#include <stdlib.h>
#include "vector.h"


int __vector__resize(void **p, int size, int elemsize, int *rsize)
{
    void *q;
    
    if (!size) {
	free(*p);
	return 1;
    }
    
    if (!*rsize) 
	q = malloc(size * elemsize);
    else
        q = realloc(*p, size * elemsize);
    
    if (q) {
	*p = q;
	*rsize = size;
    }

    return !!q;
}
