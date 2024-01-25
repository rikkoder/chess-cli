#ifndef COMMON_H
#define COMMON_H


#define swap(x, y, type)	{type t = x; x = y; y = t;}
#define min(x, y)			((x) < (y) ? (x) : (y))
#define max(x, y)			((x) > (y) ? (x) : (y))

char*		itoa		(int i, char *a);
void		shuffle		(void *arr, size_t nmemb, size_t size);

#endif
