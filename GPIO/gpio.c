#include "gpio.h"

/* Struct that discrible a GPIO pin. */
struct gpio_t {
	pinNumber number;
	int value, direction, edge; /* fd of value, direciton, etc. file in pin's path, not the actual value */
};

int gpioSet(const gpio p, const char *direction) {

	write(p->direction, (void*) direction, 4*sizeof(char));
	lseek(p->direction, 0, SEEK_SET);

	return 0;

}

int gpioWrite(const gpio p, bool value) {

	int val = (int)value + '0';

	write(p->value, (void*) &val, sizeof(char));
	lseek(p->value, 0, SEEK_SET);

	return 0;

}

int gpioRead(const gpio p) {

	char ret;
	read(p->value, (void*) &ret, sizeof(char));
	lseek(p->value, 0, SEEK_SET);
	return (bool)(ret - '0');

}

gpio gpioExport(int num) {

	gpio p;
	int export;
	char str[35];

	p = (gpio)malloc(sizeof(struct gpio_t));

	if(p == NULL)
		return NULL;

	p->number = num;

	sprintf(str, "%s/export", GPIOBASEDIR);
	export = open(str, O_WRONLY | O_SYNC);
	if(export < 0) {
		free(p);
		return NULL;
	}

	sprintf(str, "%d", p->number);
	write(export, (void*) str, strlen(str)*sizeof(char));

	close(export);

	sprintf(str, "%s/gpio%d/value", GPIOBASEDIR, p->number);
	p->value = open(str, O_WRONLY | O_SYNC);
	if(p->value < 0) {
		free(p);
		return NULL;
	}

	sprintf(str, "%s/gpio%d/direction", GPIOBASEDIR, p->number);
	p->direction = open(str, O_WRONLY | O_SYNC);
	if(p->direction < 0) {
		close(p->value);
		free(p);
		return NULL;
	}

	p->edge = -1;

	return p;

}

int gpioEdge(gpio p, const char *edge) {
	if(p->edge < 0) {

		char str[35];

		sprintf(str, "%s/gpio%d/edge", GPIOBASEDIR, p->number);
		p->edge = open(str, O_WRONLY | O_SYNC);
		if(p->edge < 0)
			return -1;

	}

	write(p->edge, (void*) edge, 8*sizeof(char));
	lseek(p->edge, 0, SEEK_SET);

	return 0;
}

int gpioGetfd(gpio p) {
	return p->file;
}

int gpioUnexport(gpio p) {

	int unexport;
	char str[35];

	sprintf(str, "%s/unexport", GPIOBASEDIR);
	unexport = open(str, O_WRONLY | O_SYNC);

	sprintf(str, "%d", p->number);
	write(unexport, (void*) str, strlen(str)*sizeof(char));

	close(unexport);
	close(p->value);
	close(p->direction);

	if(p->edge > 0)
		close(p->edge);

	free(p);

	return 0;

}
