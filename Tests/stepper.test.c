#include "../Stepper/stepper.h"
#include "../utils/utils.h"

int main(void) {

	stepper s;

	stepperCreate(&s, P9_11, P9_13, P9_15, P9_17);

	step(&s, 64);
	delay(500);
	step(&s, -32);
	delay(500);

	stepperDestroy(&s);

	return 0;

}
