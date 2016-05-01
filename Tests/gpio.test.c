#include <stdlib.h>
#include "../GPIO/gpio.h"
#include "../utils/utils.h"

int main(void) {

	gpio pin;
	pin = gpioExport(P9_12);

	if(!pin)
		exit(EXIT_FAILURE);

	gpioSet(pin, OUT);
	gpioWrite(pin, HIGH);
	delay(1000);
	gpioWrite(pin, LOW);

	gpioUnexport(pin);

	exit(EXIT_SUCCESS);

}

