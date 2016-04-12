#ifndef GPIO_C
#define GPIO_C

#include "gpio.h"

int gpioSet(pin p, const char *direction) {

	FILE *file;
	char path[35];

	sprintf(path, "%s/gpio%d/direction", GPIOBASEDIR, p);
	file = fopen(path, "w");
	if(file == NULL)
		return -1;

	fputs(direction, file);

	fclose(file);

	return 0;

}

int gpioWrite(pin p, bool value) {

	int val;
	FILE *file;
	char path[35];

	val = (int)value + '0';

	sprintf(path, "%s/gpio%d/value", GPIOBASEDIR, p);
	file = fopen(path, "w");
	if(file == NULL)
		return -1;

	fputc(val, file);

	fclose(file);

	return 0;

}

int gpioRead(pin p) {

	FILE *file;
	char path[35];

	sprintf(path, "%s/gpio%d/value", GPIOBASEDIR, p);
	file = fopen(path, "r");
	if(file == NULL)
		return -1;

	if(fread((void*)path, sizeof(char), 1, file) == 0)
		return -1;

	if(path[0] == '1')
		return true;
	else
		return false;

}

/*int gpioExport(pin p) {

	FILE *file;
	struct stat st;
	char path[35];

	sprintf(path, "%s/gpio%d", GPIOBASEDIR, p);
	stat(path, &st);
	if(S_ISDIR(st.st_mode))
		return 0;

	sprintf(path, "%s/export", GPIOBASEDIR);
	file = fopen(path, "w");
	if(file == NULL)
		return -1;

	sprintf(path, "%d", p);
	fputs(path, file);

	sprintf(path, "%s/gpio%d", GPIOBASEDIR, p);
	stat(path, &st);
	if(S_ISDIR(st.st_mode))
		return 0;
	return -2;

}

int gpioUnexport(pin p) {

	FILE *file;
	struct stat st;
	char path[35];

	sprintf(path, "%s/gpio%d", GPIOBASEDIR, p);
	stat(path, &st);
	if(!S_ISDIR(st.st_mode))
		return 0;

	sprintf(path, "%s/unexport", GPIOBASEDIR);
	file = fopen(path, "w");
	if(file == NULL)
		return -1;

	sprintf(path, "%d", p);
	fputs(path, file);

	sprintf(path, "%s/gpio%d", GPIOBASEDIR, p);
	stat(path, &st);
	if(!S_ISDIR(st.st_mode))
		return 0;
	return -2;

}*/

#endif
