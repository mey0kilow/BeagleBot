# Compile options
cc=arm-cortex_a8-linux-gnueabi-gcc
OPTS=-std=gnu99 -fgnu89-inline -ggdb -march=armv7-a -mtune=cortex-a8 -mfpu=crypto-neon-fp-armv8 -ftree-vectorize -ffast-math -mfloat-abi=hard -funsafe-math-optimizations

# Compile things
OBJECTS=GPIO/gpio.o PWM/pwm.o Stepper/stepper.o utils/utils.o GPS/gps.o I2C/i2c.o Compass/HMC5883L.o
LINKS=-lpthread -lm

# Remote options
REMOTE_PATH=~/
REMOTE=169.254.218.91
USER=bbb

export LINKS
export OPTS
export cc

.PHONY: clean deploy

all: $(OBJECTS)

GPIO/gpio.o: GPIO/Makefile
	$(MAKE) $(MFLAGS) -C GPIO

PWM/pwm.o: PWM/Makefile
	$(MAKE) $(MFLAGS) -C PWM

Stepper/stepper.o: Stepper/Makefile
	$(MAKE) $(MFLAGS) -C Stepper

utils/utils.o: utils/Makefile
	$(MAKE) $(MFLAGS) -C utils

GPS/gps.o: GPS/Makefile
	$(MAKE) $(MFLAGS) -C GPS

I2C/i2c.o: I2C/Makefile
	$(MAKE) $(MFLAGS) -C I2C

Compass/HMC5883L.o: Compass/Makefile
	$(MAKE) $(MFLAGS) -C Compass

tests: Tests
	$(MAKE) $(MFLAGS) -C Tests tests

i2c/test: i2c
	$(MAKE) $(MFLAGS) -C i2c

clean:
	$(MAKE) $(MFLAGS) -C GPS clean
	$(MAKE) $(MFLAGS) -C utils clean
	$(MAKE) $(MFLAGS) -C GPIO clean
	$(MAKE) $(MFLAGS) -C PWM clean
	$(MAKE) $(MFLAGS) -C Stepper clean
	$(MAKE) $(MFLAGS) -C I2C clean
	$(MAKE) $(MFLAGS) -C deploy clean
	$(MAKE) $(MFLAGS) -C Tests clean
	$(MAKE) $(MFLAGS) -C Compass clean

export REMOTE
export USER

deploy:
	$(MAKE) $(MFLAGS) -C deploy prepare
	$(MAKE) $(MFLAGS) -C deploy send

