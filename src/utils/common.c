#include <stdlib.h>
#include <time.h>

#include "common.h"


char* itoa(int i, char *a) {
	if (i == 0) {
		a[0] = '0';
		a[1] = '\0';
		return a;
	}
	int off = 0;
	if (i < 0) {
		off = 1;
		a[0] = '-';
	}
	int d = off;
	while (i) {
		a[d++] = (i%10)+'0';
		i /= 10;
	}
	a[d] = '\0';
	int s = off, e = d-1;
	while (s < e) {
		swap(a[s], a[e], char);
		s++; e--;
	}
	return a;
}


void shuffle (void *arr, size_t nmemb, size_t size) {
	time_t t;
	srand((unsigned int) time(&t));

	char tmp[size];
	for (size_t i = 0; i < nmemb; i++) {
		size_t j = (rand() * (nmemb - 1)) / RAND_MAX;
		memcpy(tmp, arr + j * size, size);
		memcpy(arr + j * size, arr + i * size, size);
		memcpy(arr + i * size, tmp, size);
	}
}
