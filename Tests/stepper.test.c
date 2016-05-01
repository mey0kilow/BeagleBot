#include "../Stepper/stepper.h"
#include "../utils/utils.h"

int main(void) {

	stepper s;

	s = stepperCreate(P9_11, P9_13, P9_15, P9_17);

	step(s, 512);
	delay(500);
	step(s, -256);
	delay(500);

	stepperDestroy(s);

	return 0;

}
