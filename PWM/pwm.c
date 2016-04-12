#ifndef PWM_C
#define PWM_C

#include "pwm.h"

int pwmExport(pwm *p, int num) {

	int export;
	char str[35];

	p->number = num;

	sprintf(str, "%s/export", PWMBASEDIR);

	export = open(str, O_WRONLY | O_SYNC);
	if(export < 0) {
		p->enable = -1;
		p->duty = -1;
		p->period = -1;
		return -1;
	}

	sprintf(str, "%d", p->number);
	write(export, (void*) str, strlen(str)*sizeof(char));
	close(export);

	sprintf(str, "%s/pwm%d/period_ns", PWMBASEDIR, p->number);
	p->period = open(str, O_RDWR | O_SYNC);
	if(p->period < 0) {
		p->enable = -1;
		p->duty = -1;
		p->period = -1;
		return -1;
	}

	sprintf(str, "%s/pwm%d/duty_ns", PWMBASEDIR, p->number);
	p->duty = open(str, O_RDWR | O_SYNC);
	if(p->duty < 0) {
		close(p->period);
		p->enable = -1;
		p->duty = -1;
		p->period = -1;
		return -1;
	}

	sprintf(str, "%s/pwm%d/run", PWMBASEDIR, p->number);
	p->enable = open(str, O_RDWR | O_SYNC);
	if(p->enable < 0) {
		close(p->duty);
		close(p->period);
		p->enable = -1;
		p->duty = -1;
		p->period = -1;
		return -1;
	}

	return 0;

}

int pwmUnexport(pwm *p) {

	int unexport;
	char str[35];

	pwmEnable(*p, false);

	sprintf(str, "%s/unexport", PWMBASEDIR);

	unexport = open(str, O_WRONLY | O_SYNC);
	if(unexport < 0)
		return -1;

	sprintf(str, "%d", p->number);
	write(unexport, (void*) str, strlen(str)*sizeof(char));
	close(unexport);

	p->number = -1;

	close(p->period);
	p->period = -1;

	close(p->duty);
	p->duty = -1;

	close(p->enable);
	p->enable = -1;

	return 0;

}

int pwmSetFreq(const pwm p, double freq) {

	unsigned int period = 1e9/freq;

	return pwmSetPeriod(p, period);

}

int pwmSetPeriod(const pwm p, unsigned int period) {

	char str[30];

	sprintf(str, "%u", period);
	write(p.period, (void*) str, strlen(str)*sizeof(char));
	lseek(p.period, 0, SEEK_SET);

	return 0;

}

unsigned int pwmGetPeriod(const pwm p) {

	char str[30];

	read(p.period, (void*) str, 30*sizeof(char));
	lseek(p.period, 0, SEEK_SET);

	return atoi(str);

}

int pwmSetDuty(const pwm p, double percent) {

	unsigned int duty;
	unsigned int period;
	char str[30];

	period = pwmGetPeriod(p);
	if(period<0)
		return period;

	duty = period * percent/100;

	sprintf(str, "%u", duty);
	write(p.duty, (void*) str, strlen(str)*sizeof(char));
	lseek(p.duty, 0, SEEK_SET);

	return 0;

}

int pwmEnable(const pwm p, bool enable) {

	char value = (int)enable + '0';

	write(p.enable, (void*) &value, sizeof(char));
	lseek(p.enable, 0, SEEK_SET);

	return 0;

}

#endif
