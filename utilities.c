#include <stdlib.h>
#include <string.h>

#include "utilities.h"

char* itoa(int i, char *a) {
	int d = 0;
	while (i) {
		a[d++] = (i%10)+'0';
		i /= 10;
	}

	int s = 0, e = d-1;
	while (s < e) {
		swap(a[s], a[e]);
		s++; e--;
	}

	return a;
}
