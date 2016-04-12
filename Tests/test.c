#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "GPIO/gpio.h"

#undef P8_13

#include "PWM/pwm.h"
#include "utils/utils.h"
#include "Stepper/stepper.h"

gpio dir;
pwm pwm1;
stepper s;
gpio in1, in2, in3, in4;

void init(void);
void setVel(int percent);
void turn(bool dir, int ms);
void end(void);

int main(int argc, char *argv[]) {

	init();

	vel(30);

	delay(1000);

	turn(true, 500);

	delay(500);

	turn(false, 500);

	delay(500);

	setVel(0);

	end();

}

void init(void) {

	/*GPIO init*/
	gpioExport(&dir, P8_15);
	gpioSet(dir, OUT);

	/*PWM*/
	pwmExport(&pwm1, P8_13);
	pwmSetFreq(pwm1, 50);
	pwmSetDuty(pwm1, 0);

	/*Stepper*/
	gpioExport(&in1, P9_11);
	gpioExport(&in2, P9_13);
	gpioExport(&in3, P9_15);
	gpioExport(&in4, P9_17);
	stepperCreate(&s, in1, in2, in3, in4);

}

void setVel(int percent) {

	if(percent > 0)
		gpioWrite(dir, true);
	else
		gpioWrite(dir, false);
	percent = percent > 0? percent : -percent;
	pwmSetDuty(pwm1, percent);

}

void turn(bool dir, int ms) {
	int direction = 64 * dir?1:-1;
	step(&s, direction);
	delay(ms);
	step(&s, -direction);
}

void end(void) {
	/*GPIO*/
	gpioUnexport(&dir);

	/*PWM*/
	pwmUnexport(&pwm1);

	/*Stepper*/
	step(&s, -(s.pos));
	gpioUnexport(&in1);
	gpioUnexport(&in2);
	gpioUnexport(&in3);
	gpioUnexport(&in4);
}
