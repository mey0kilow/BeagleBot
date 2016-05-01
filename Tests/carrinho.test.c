#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../GPIO/gpio.h"

#undef P8_13
#undef P8_19

#include "../PWM/pwm.h"
#include "../Stepper/stepper.h"
#include "../utils/utils.h"

gpio dir;
pwm vel;
stepper s;

void turn(int steps, bool dir, int ms);
void setVel(int percent);
void init(void);
void end(void);

int main(int argc, char *argv[]) {

	int v, steps;

	sscanf(argv[1], "%d", &v);
	sscanf(argv[2], "%d", &steps);

	if(argc<2)
		exit(-1);

	init();

	setVel(v);

	delay(1000);

	turn(steps, true, 500);
	delay(500);
	turn(steps, false, 500);
	delay(500);

	setVel(0);

	end();

	exit(EXIT_SUCCESS);

}

void init(void) {
	// Tracao
	dir = gpioExport(P8_15);
	gpioSet(dir, OUT);
	gpioWrite(dir, LOW);

	vel = pwmExport(P8_13);
	pwmSetFreq(vel, 50);
	pwmSetDuty(vel, 0);
	pwmEnable(vel, true);

	// Direcao
	s = stepperCreate(P9_11, P9_13, P9_15, P9_17);

}

void end(void) {
	gpioUnexport(dir);
	pwmUnexport(vel);
	stepperDestroy(s);
}

void setVel(int percent) {

	gpioWrite(dir, percent > 0);
	percent = percent > 0 ? percent : -percent;
	pwmSetDuty(vel, percent);

}

void turn(int steps, bool dir, int ms) {

	step(s, steps*(dir?1:-1));
	delay(ms);
	step(s, steps*(dir?-1:1));

}
