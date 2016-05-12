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

GPIO/gpio.o: GPIO/gpio.c GPIO/gpio.h
	$(MAKE) $(MFLAGS) -C GPIO

PWM/pwm.o: PWM/pwm.c PWM/pwm.h
	$(MAKE) $(MFLAGS) -C PWM

Stepper/stepper.o: Stepper/stepper.c Stepper/stepper.h
	$(MAKE) $(MFLAGS) -C Stepper

utils/utils.o: utils/utils.c utils/utils.h
	$(MAKE) $(MFLAGS) -C utils

GPS/gps.o: GPS/gps.c GPS/gps.h
	$(MAKE) $(MFLAGS) -C GPS

I2C/i2c.o: I2C/i2c.c I2C/i2c.h
	$(MAKE) $(MFLAGS) -C I2C

Compass/HMC5883L.o: Compass/HMC5883L.c Compass/HMC5883L.h
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

