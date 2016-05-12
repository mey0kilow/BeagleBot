#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <poll.h>
#include <stdbool.h>
#include <math.h>
#include "../Compass/HMC5883L.h"
#include "../GPIO/gpio.h"
#include "../utils/utils.h"

bool cont = true;

void sigHand(int signo) {
	cont = false;
	return;
}

int main(void) {
	signal(SIGINT, sigHand);

	hmc5883l mag = hmc5883lCreate("/dev/i2c-1",
			HMC5883L_CRA_1_SAMPLE | HMC5883L_CRA_75_00HZ | HMC5883L_CRA_NORMAL_MEASURE,
			HMC5883L_CRB_0_88_GAIN, HMC5883L_MODE_SINGLE);

	if(mag == NULL) {
		fprintf(stderr, "Can't create mag\n");
		exit(EXIT_FAILURE);
	}

	gpio drdy = gpioExport(P9_23);

	if(drdy == NULL) {
		fprintf(stderr, "Can't open gpio pin\n");
		exit(EXIT_FAILURE);
	}

	gpioSet(drdy, "in");

	struct axis_t axis, min, max;
	double direction;

	min.x = min.y = min.z = 2048;
	max.x = max.y = max.z = -2048;

	printf("#x\ty\tz\tDirection\n");

	while(cont) {
		if(gpioEdge(drdy, RISE) < 0) {
			perror("#Poll failed");
			continue;
		}

		hmc5883lRead(mag, &axis);

		if(axis.x > 2047 || axis.x < -2048) {
			printf("x\n");
			continue;
		}

		if(axis.x < min.x)
			min.x = axis.x;

		if(axis.x > max.x)
			max.x = axis.x;

		if(axis.y > 2047 || axis.y < -2048) {
			printf("y\n");
			continue;
		}

		if(axis.y < min.y)
			min.y = axis.y;

		if(axis.y > max.y)
			max.y = axis.y;

		if(axis.z > 2047 || axis.z < -2048) {
			printf("z\n");
			continue;
		}

		if(axis.z < min.z)
			min.z = axis.z;

		if(axis.z > max.z)
			max.z = axis.z;


		direction = 180 * atan2((double)-axis.y,(double)axis.x) / M_PI;
		if(direction < 0) direction += 360;

		printf("%hd\t%hd\t %hd\t\t%f\n", axis.x, axis.y, axis.z, direction);

		hmc5883lConfigMode(mag, HMC5883L_MODE_SINGLE);

	}

	gpioUnexport(drdy);
	hmc5883lDestroy(mag);

	printf("#Max:\n#x: %hd, y: %hd, z: %hd\n#Min:\n#x: %hd, y: %hd, z: %hd\n", max.x, max.y, max.z, min.x, min.y, min.z);

	exit(EXIT_SUCCESS);

}
