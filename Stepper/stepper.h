#ifndef STEPPER_H
#define STEPPER_H

#include "../GPIO/gpio.h"
#include "../utils/utils.h"

#define PHASES 4

typedef struct stepper_t * stepper;

enum step_t { FULL, HALF, WAVE };

stepper stepperCreate(pinNumber in1, pinNumber in2, pinNumber in3, pinNumber in4, enum step_t type, double freq);
void step(stepper s, int steps);
int stepperGetPos(stepper s);
int stepperDestroy(stepper s);

#endif
