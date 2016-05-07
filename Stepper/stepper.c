#include "stepper.h"

typedef void (*step_f)(stepper s);

struct stepper_t {
	gpio pin[4];
	step_f cc, cw;
	double delay;
	int pos;
};

static void fclockStep(stepper s);
static void hclockStep(stepper s);
static void wclockStep(stepper s);
static void fcounterStep(stepper s);
static void hcounterStep(stepper s);
static void wcounterStep(stepper s);

stepper stepperCreate(pinNumber in1, pinNumber in2, pinNumber in3, pinNumber in4, enum step_t type, double freq) {
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

	for(i = 0; i < PHASES; i++) {
		gpioSet(s->pin[i], OUT);
		gpioWrite(s->pin[i], LOW);
	}

	switch(type) {
		case FULL:
			s->cw = fclockStep;
			s->cc = fcounterStep;
			s->delay = 1000/freq/(2*PHASES);
			break;
		case HALF:
			s->cw = hclockStep;
			s->cc = hcounterStep;
			s->delay = 1000/freq/(PHASES);
			break;
		case WAVE:
			s->cw = wclockStep;
			s->cc = wcounterStep;
			s->delay = 1000/freq/(PHASES);
			break;
		default:
			gpioUnexport(s->pin[3]);
			gpioUnexport(s->pin[2]);
			gpioUnexport(s->pin[1]);
			gpioUnexport(s->pin[0]);
			free(s);
	}

	return s;
}

static void fclockStep(stepper s) {

	gpioWrite(s->pin[0], true);
	delay(s->delay);
	gpioWrite(s->pin[3], false);
	delay(s->delay);
	gpioWrite(s->pin[1], true);
	delay(s->delay);
	gpioWrite(s->pin[0], false);
	delay(s->delay);
	gpioWrite(s->pin[2], true);
	delay(s->delay);
	gpioWrite(s->pin[1], false);
	delay(s->delay);
	gpioWrite(s->pin[3], true);
	delay(s->delay);
	gpioWrite(s->pin[2], false);
	delay(s->delay);
	s->pos++;

}

static void fcounterStep(stepper s) {

	gpioWrite(s->pin[0], true);
	delay(s->delay);
	gpioWrite(s->pin[1], false);
	delay(s->delay);
	gpioWrite(s->pin[3], true);
	delay(s->delay);
	gpioWrite(s->pin[0], false);
	delay(s->delay);
	gpioWrite(s->pin[2], true);
	delay(s->delay);
	gpioWrite(s->pin[3], false);
	delay(s->delay);
	gpioWrite(s->pin[1], true);
	delay(s->delay);
	gpioWrite(s->pin[2], false);
	delay(s->delay);
	s->pos--;

}

static void hclockStep(stepper s) {

	gpioWrite(s->pin[3], false);
	gpioWrite(s->pin[0], true);
	delay(s->delay);
	gpioWrite(s->pin[0], false);
	gpioWrite(s->pin[1], true);
	delay(s->delay);
	gpioWrite(s->pin[1], false);
	gpioWrite(s->pin[2], true);
	delay(s->delay);
	gpioWrite(s->pin[2], false);
	gpioWrite(s->pin[3], true);
	delay(s->delay);
	s->pos++;

}

static void hcounterStep(stepper s) {

	gpioWrite(s->pin[1], false);
	gpioWrite(s->pin[0], true);
	delay(s->delay);
	gpioWrite(s->pin[0], false);
	gpioWrite(s->pin[3], true);
	delay(s->delay);
	gpioWrite(s->pin[3], false);
	gpioWrite(s->pin[2], true);
	delay(s->delay);
	gpioWrite(s->pin[2], false);
	gpioWrite(s->pin[1], true);
	delay(s->delay);
	s->pos--;

}

static void wclockStep(stepper s) {

	gpioWrite(s->pin[3], false);
	gpioWrite(s->pin[1], true);
	delay(s->delay);
	gpioWrite(s->pin[0], false);
	gpioWrite(s->pin[2], true);
	delay(s->delay);
	gpioWrite(s->pin[1], false);
	gpioWrite(s->pin[3], true);
	delay(s->delay);
	gpioWrite(s->pin[2], false);
	gpioWrite(s->pin[0], true);
	delay(s->delay);
	s->pos++;

}

static void wcounterStep(stepper s) {

	gpioWrite(s->pin[1], false);
	gpioWrite(s->pin[3], true);
	delay(s->delay);
	gpioWrite(s->pin[0], false);
	gpioWrite(s->pin[2], true);
	delay(s->delay);
	gpioWrite(s->pin[3], false);
	gpioWrite(s->pin[1], true);
	delay(s->delay);
	gpioWrite(s->pin[2], false);
	gpioWrite(s->pin[0], true);
	delay(s->delay);
	s->pos--;

}

void step(stepper s, int steps) {

	int i;

	/*The initial state for any step function is in1 on and others off.
	 *We do it to make sure the step functions always start from in1*/
	gpioWrite(s->pin[0], true);

	if(steps > 0)
		for(i = steps; i != 0; --i)
			s->cw(s);
	else
		for(i = steps; i != 0; ++i)
			s->cc(s);

	for(i = 0; i < PHASES; i++)
		gpioWrite(s->pin[i], LOW);

}

int stepperGetPos(stepper s) {
	return s->pos;
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
