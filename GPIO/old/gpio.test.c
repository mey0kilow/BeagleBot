#include "GPIO/gpio.h"
#include <unistd.h>

void *gpioInit(void *args) {

	gpioSet(P9_18, OUT);
	gpioSet(P9_22, OUT);
	gpioWrite(P9_18, HIGH);
	gpioWrite(P9_22, LOW);

}

void *gpioTest(void *args) {

	while(true) {

		gpioWrite(P9_18, !gpioRead(P9_18));
		gpioWrite(P9_22, !gpioRead(P9_22));
		sleep(1);

	}

}

int main(void) {

	gpioInit(NULL);
	gpioTest(NULL);
	return 0;

}
