#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include "../utils/utils.h"
#include "../GPS/gps.h"

gps GPS;

void end(int signo) {
	gpsDestroy(GPS);
	exit(EXIT_SUCCESS);
}

int main(void) {

	gpsPos pos;

	signal(SIGINT, end);
	setbuf(stdout, NULL);

	GPS = gpsCreate("/dev/ttyO1");
	if(GPS == NULL) {
		perror("Can't open/configure device");
		exit(EXIT_FAILURE);
	}

	while(1) {
		delay(1000);
		pos = gpsRead(GPS);
		printf("%lf, %lf\n", pos.lat, pos.lon);
	}

	exit(EXIT_SUCCESS);

}
