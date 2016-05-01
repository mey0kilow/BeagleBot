#include "stepper.h"

struct stepper_t {
	gpio pin[4];
	int pos;
};

stepper stepperCreate(pinNumber in1, pinNumber in2, pinNumber in3, pinNumber in4) {
	int i;
	stepper s = (stepper)malloc(sizeof(struct stepper_t));

	if(s == NULL)
		return NULL;

	s->pin[0] = gpioExport(in1);
	if(s->pin[0] == NULL) {
		free(s);
		return NULL;
	}

	s->pin[1] = gpioExport(in2);
	if(s->pin[1] == NULL) {
		gpioUnexport(s->pin[0]);
		free(s);
		return NULL;
	}

	s->pin[2] = gpioExport(in3);
	if(s->pin[2] == NULL) {
		gpioUnexport(s->pin[1]);
		gpioUnexport(s->pin[0]);
		free(s);
		return NULL;
	}

	s->pin[3] = gpioExport(in4);
	if(s->pin[3] == NULL) {
		gpioUnexport(s->pin[2]);
		gpioUnexport(s->pin[1]);
		gpioUnexport(s->pin[0]);
		free(s);
		return NULL;
	}

	for(i = 0; i < 4; i++) {
		gpioSet(s->pin[i], OUT);
		gpioWrite(s->pin[i], LOW);
	}

	return s;
}

static void clockStep(stepper s) {

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

static void counterStep(stepper s) {

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

void step(stepper s, int steps) {

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

int stepperDestroy(stepper s) {

	// return to initial position
	step(s, -(s->pos));

	// Unexport gpios
	gpioUnexport(s->pin[0]);
	gpioUnexport(s->pin[1]);
	gpioUnexport(s->pin[2]);
	gpioUnexport(s->pin[3]);

	free(s);

	return 0;

}
