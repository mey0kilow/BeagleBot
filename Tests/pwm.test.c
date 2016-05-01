#include <stdio.h>
#include <stdbool.h>
#include "../PWM/pwm.h"

int main(void) {

	pwm pwm1, pwm2;

	// Export
	pwm1 = pwmExport(P9_14);
	if(pwm1 == NULL)
		exit(EXIT_FAILURE);

	pwm2 = pwmExport(P9_16);
	if(pwm2 == NULL)
		exit(EXIT_FAILURE);

	// Set period
	pwmSetFreq(pwm1, 50);
	pwmSetFreq(pwm2, 50);

	// Set duty
	pwmSetDuty(pwm1, 0);
	pwmSetDuty(pwm2, 100);

	// run
	pwmEnable(pwm1, true);
	pwmEnable(pwm2, true);

	for(int k = 5; k != 0; k--){
		for(int i = 0; i <= 100; i++) {
			pwmSetDuty(pwm1, i);
			pwmSetDuty(pwm2, 100-i);
			for(int j = 0; j < 1000000; j++);
		}
		for(int i = 0; i <= 100; i++) {
			pwmSetDuty(pwm1, 100-i);
			pwmSetDuty(pwm2, i);
			for(int j = 0; j < 1000000; j++);
		}
	}

	// Unexport
	pwmUnexport(pwm1);
	pwmUnexport(pwm2);

	exit(EXIT_SUCCESS);

}
