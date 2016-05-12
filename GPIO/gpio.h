#ifndef GPIO_H
#define GPIO_H

#include <stdio.h>		// sprintf
#include <stdbool.h>	// true, false
#include <stdlib.h>		// free
#include <sys/types.h>	// open
#include <sys/stat.h>	// open
#include <fcntl.h>		// open
#include <unistd.h>		// read, write
#include "../utils/utils.h"	// strlen, zeros
#include <errno.h>		// perror
#include <poll.h>

#ifndef PIN_TYPE_DEFINED
#define PIN_TYPE_DEFINED
typedef int pinNumber;
#endif

#ifndef P9_11
#define P9_11 30
#endif
#ifndef P9_12
#define P9_12 60
#endif
#ifndef P9_13
#define P9_13 31
#endif
#ifndef P9_14
#define P9_14 50
#endif
#ifndef P9_15
#define P9_15 48
#endif
#ifndef P9_16
#define P9_16 51
#endif
#ifndef P9_17
#define P9_17 5
#endif
#ifndef P9_18
#define P9_18 4
#endif
#ifndef P9_21
#define P9_21 3
#endif
#ifndef P9_22
#define P9_22 2
#endif
#ifndef P9_23
#define P9_23 49
#endif
#ifndef P9_24
#define P9_24 15
#endif
#ifndef P9_25
#define P9_25 117
#endif
#ifndef P9_26
#define P9_26 14
#endif
#ifndef P9_27
#define P9_27 115
#endif
#ifndef P9_28
#define P9_28 113
#endif
#ifndef P9_29
#define P9_29 111
#endif
#ifndef P9_30
#define P9_30 112
#endif
#ifndef P9_31
#define P9_31 110
#endif
#ifndef P9_41
#define P9_41 20
#endif
#ifndef P9_42
#define P9_42 7
#endif

#ifndef P8_3
#define P8_3 38
#endif
#ifndef P8_03
#define P8_03 38
#endif
#ifndef P8_4
#define P8_4 39
#endif
#ifndef P8_04
#define P8_04 39
#endif
#ifndef P8_5
#define P8_5 34
#endif
#ifndef P8_05
#define P8_05 34
#endif
#ifndef P8_6
#define P8_6 35
#endif
#ifndef P8_06
#define P8_06 35
#endif
#ifndef P8_7
#define P8_7 66
#endif
#ifndef P8_07
#define P8_07 66
#endif
#ifndef P8_8
#define P8_8 67
#endif
#ifndef P8_08
#define P8_08 67
#endif
#ifndef P8_9
#define P8_9 69
#endif
#ifndef P8_09
#define P8_09 69
#endif
#ifndef P8_10
#define P8_10 68
#endif
#ifndef P8_11
#define P8_11 45
#endif
#ifndef P8_12
#define P8_12 44
#endif
#ifndef P8_13
#define P8_13 23
#endif
#ifndef P8_14
#define P8_14 26
#endif
#ifndef P8_15
#define P8_15 47
#endif
#ifndef P8_16
#define P8_16 46
#endif
#ifndef P8_17
#define P8_17 27
#endif
#ifndef P8_18
#define P8_18 65
#endif
#ifndef P8_19
#define P8_19 22
#endif
#ifndef P8_20
#define P8_20 63
#endif
#ifndef P8_21
#define P8_21 62
#endif
#ifndef P8_22
#define P8_22 37
#endif
#ifndef P8_23
#define P8_23 36
#endif
#ifndef P8_24
#define P8_24 33
#endif
#ifndef P8_25
#define P8_25 32
#endif
#ifndef P8_26
#define P8_26 61
#endif
#ifndef P8_27
#define P8_27 86
#endif
#ifndef P8_28
#define P8_28 88
#endif
#ifndef P8_29
#define P8_29 87
#endif
#ifndef P8_30
#define P8_30 89
#endif
#ifndef P8_31
#define P8_31 10
#endif
#ifndef P8_32
#define P8_32 11
#endif
#ifndef P8_33
#define P8_33 9
#endif
#ifndef P8_34
#define P8_34 81
#endif
#ifndef P8_35
#define P8_35 8
#endif
#ifndef P8_36
#define P8_36 80
#endif
#ifndef P8_37
#define P8_37 78
#endif
#ifndef P8_38
#define P8_38 79
#endif
#ifndef P8_39
#define P8_39 76
#endif
#ifndef P8_40
#define P8_40 77
#endif
#ifndef P8_41
#define P8_41 74
#endif
#ifndef P8_42
#define P8_42 75
#endif
#ifndef P8_43
#define P8_43 72
#endif
#ifndef P8_44
#define P8_44 73
#endif
#ifndef P8_45
#define P8_45 70
#endif
#ifndef P8_46
#define P8_46 71
#endif

#define HIGH true
#define LOW false

struct gpio_t;

typedef struct gpio_t * gpio;

static const char GPIOBASEDIR[] = "/sys/class/gpio";
static const char OUT[] = "out";
static const char IN[] = "in ";
static const char RISE[] = "rising";
static const char FALL[] = "falling";
static const char BOTH[] = "both";

int gpioSet(const gpio p, const char *direction);
int gpioWrite(const gpio p, bool value);
int gpioRead(const gpio p);
int gpioEdge(gpio p, const char *edge);
gpio gpioExport(int num);
int gpioUnexport(gpio p);

int gpioGetfd(gpio p);

#endif
