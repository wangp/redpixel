#ifndef __included_vector_h
#define __included_vector_h


int __vector__resize(void **p, int size, int elemsize, int *rsize);


#define VECTOR(name, type)		\
	type *name;			\
	int __vector_size__##name;

#define vector_resize(name, size)	\
	__vector__resize((void *)&name, size, sizeof *name, &__vector_size__##name)

#define vector_size(name) 	(__vector_size__##name)
	    

#endif
