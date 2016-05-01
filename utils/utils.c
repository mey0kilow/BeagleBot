#ifndef UTILS_C
#define UTILS_C

#include "utils.h"

int delay(double ms) {

	if(ms < 0) return -1;

	double mili = (int)ms%1000;
	int sec = (int)(ms-mili)/1000;
	mili += ms - (int)ms;

	struct timespec t = { .tv_sec = sec, .tv_nsec = mili*1000000 };

	nanosleep(&t, NULL);

	return 0;

}

size_t strlen(const char *str) {

	register int i = 0;
	while(str[i])
		++i;
	return i;

}
size_t zeros(void *buff, size_t size) {

	char *ptr = (char *) buff;
	size_t pos = 0;

	while(size) {
		ptr[pos] = '\0';
		++pos;
		--size;
	}

	return pos;

}


#endif
