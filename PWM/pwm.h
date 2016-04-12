#ifndef PWM_H
#define PWM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "../utils/utils.h"

#ifndef PIN_TYPE_DEFINED
#define PIN_TYPE_DEFINED
typedef int pinNumber;
#endif

#ifndef P8_13
#define P8_13 6
#endif
#ifndef P8_19
#define P8_19 5
#endif
#ifndef P9_14
#define P9_14 3
#endif
#ifndef P9_16
#define P9_16 4
#endif
#ifndef P9_21
#define P9_21 1
#endif
#ifndef P9_22
#define P9_22 0
#endif
#ifndef P9_28
#define P9_28 7
#endif
#ifndef P9_42
#define P9_42 2
#endif

typedef struct pwm_t {
	pinNumber number;
	int period;
	int duty;
	int enable;
} pwm;

static const char PWMBASEDIR[] = "/sys/class/pwm";
int pwmSetFreq(const pwm p, double freq);
int pwmSetPeriod(const pwm p, unsigned int period);
unsigned int pwmGetPeriod(const pwm p);
int pwmSetDuty(const pwm p, double percent);
double pwmGetDuty(const pwm p);
int pwmEnable(const pwm p, bool enable);
int pwmExport(pwm *p, int num);
int pwmUnexport(pwm *p);

#endif
