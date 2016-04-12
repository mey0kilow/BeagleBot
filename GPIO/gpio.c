#include "gpio.h"

int gpioSet(const gpio p, const char *direction) {

	write(p.direction, (void*) direction, 4*sizeof(char));
	lseek(p.value, 0, SEEK_SET);

	return 0;

}

int gpioWrite(const gpio p, bool value) {

	int val = (int)value + '0';

	write(p.value, (void*) &val, sizeof(char));
	lseek(p.value, 0, SEEK_SET);

	return 0;

}

int gpioRead(const gpio p) {

	char ret;
	read(p.value, (void*) &ret, sizeof(char));
	lseek(p.value, 0, SEEK_SET);
	return (bool)(ret - '0');

}

int gpioExport(gpio *p, int num) {

	int export;
	char str[35];

	p->number = num;

	sprintf(str, "%s/export", GPIOBASEDIR);
	export = open(str, O_WRONLY | O_SYNC);
	if(export < 0) {
		p->value = -1;
		p->direction = -1;
		return -1;
	}

	sprintf(str, "%d", p->number);
	write(export, (void*) str, strlen(str)*sizeof(char));

	close(export);

	sprintf(str, "%s/gpio%d/value", GPIOBASEDIR, p->number);
	p->value = open(str, O_WRONLY | O_SYNC);
	if(p->value < 0) {
		p->value = -1;
		p->direction = -1;
		return -1;
	}

	sprintf(str, "%s/gpio%d/direction", GPIOBASEDIR, p->number);
	p->direction = open(str, O_WRONLY | O_SYNC);
	if(p->direction < 0) {
		close(p->value);
		p->value = -1;
		p->direction = -1;
		return -1;
	}

	return 0;

}

int gpioUnexport(gpio *p) {

	int unexport;
	char str[35];

	sprintf(str, "%s/unexport", GPIOBASEDIR);
	unexport = open(str, O_WRONLY | O_SYNC);

	sprintf(str, "%d", p->number);
	write(unexport, (void*) str, strlen(str)*sizeof(char));

	close(unexport);
	close(p->value);
	close(p->direction);

	p->value = -1;
	p->direction = -1;
	p->number = -1;

	return 0;

}
