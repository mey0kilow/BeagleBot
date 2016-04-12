# Compile options
cc=arm-cortex_a8-linux-gnueabi-gcc
OPTS=-std=gnu99 -fgnu89-inline -ggdb

# Compile things
OBJECTS=GPIO/gpio.o PWM/pwm.o Stepper/stepper.o utils/utils.o

# Remote options
REMOTE_PATH=~/
REMOTE=169.254.218.91
USER=bbb

export OPTS
export cc

.PHONY: clean deploy

all: $(OBJECTS)

GPIO/gpio.o: GPIO
	$(MAKE) $(MFLAGS) -C GPIO

PWM/pwm.o: PWM
	$(MAKE) $(MFLAGS) -C PWM

Stepper/stepper.o: Stepper
	$(MAKE) $(MFLAGS) -C Stepper

utils/utils.o: utils
	$(MAKE) $(MFLAGS) -C utils

tests: Tests
	$(MAKE) $(MFLAGS) -C Tests tests

clean:
	$(MAKE) $(MFLAGS) -C utils clean
	$(MAKE) $(MFLAGS) -C GPIO clean
	$(MAKE) $(MFLAGS) -C PWM clean
	$(MAKE) $(MFLAGS) -C Stepper clean
	$(MAKE) $(MFLAGS) -C deploy clean

export REMOTE
export USER

deploy:
	$(MAKE) $(MFLAGS) -C deploy prepare
	$(MAKE) $(MFLAGS) -C deploy send

