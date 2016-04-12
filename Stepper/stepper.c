#include "stepper.h"


int stepperCreate(stepper *s, pinNumber in1, pinNumber in2, pinNumber in3, pinNumber in4) {
	int i;

	gpioExport(&s->pin[0], in1);
	gpioExport(&s->pin[1], in2);
	gpioExport(&s->pin[2], in3);
	gpioExport(&s->pin[3], in4);

	for(i = 0; i < 4; i++) {
		gpioSet(s->pin[i], OUT);
		gpioWrite(s->pin[i], LOW);
	}
}

static void clockStep(stepper *s) {

	gpioWrite(s->pin[3], true);
	delay(1.25);
	gpioWrite(s->pin[0], false);
	delay(1.25);
	gpioWrite(s->pin[2], true);
	delay(1.25);
	gpioWrite(s->pin[3], false);
	delay(1.25);
	gpioWrite(s->pin[1], true);
	delay(1.25);
	gpioWrite(s->pin[2], false);
	delay(1.25);
	gpioWrite(s->pin[0], true);
	delay(1.25);
	gpioWrite(s->pin[1], false);
	delay(1.25);
	s->pos++;

}

static void counterStep(stepper *s) {

	gpioWrite(s->pin[1], false);
	delay(1.25);
	gpioWrite(s->pin[0], true);
	delay(1.25);
	gpioWrite(s->pin[2], false);
	delay(1.25);
	gpioWrite(s->pin[1], true);
	delay(1.25);
	gpioWrite(s->pin[3], false);
	delay(1.25);
	gpioWrite(s->pin[2], true);
	delay(1.25);
	gpioWrite(s->pin[0], false);
	delay(1.25);
	gpioWrite(s->pin[3], true);
	delay(1.25);
	s->pos--;

}

void step(stepper *s, int steps) {

	int i;

	if(steps > 0)
		for(i = steps; i != 0; --i)
			clockStep(s);
	else
		for(i = steps; i != 0; ++i)
			counterStep(s);

	for(i = 0; i < 4; i++)
		gpioWrite(s->pin[i], LOW);

}

int stepperDestroy(stepper *s) {

	// return to initial position
	step(s, -(s->pos));

	// Unexport gpios
	gpioUnexport(&s->pin[0]);
	gpioUnexport(&s->pin[1]);
	gpioUnexport(&s->pin[2]);
	gpioUnexport(&s->pin[3]);

	return 0;

}
