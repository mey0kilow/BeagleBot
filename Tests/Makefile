tests: pwm.test gpio.test stepper.test carrinho.test termios.test gps.test

pwm.test: pwm.test.c ../PWM/pwm.o
	$(cc) pwm.test.c ../PWM/pwm.o -o pwm.test $(OPTS) $(LINKS)

gpio.test: gpio.test.c ../GPIO/gpio.o ../utils/utils.o
	$(cc) gpio.test.c ../GPIO/gpio.o ../utils/utils.o -o gpio.test $(OPTS) $(LINKS)

stepper.test: stepper.test.c ../Stepper/stepper.o ../GPIO/gpio.o ../utils/utils.o
	$(cc) stepper.test.c ../Stepper/stepper.o ../GPIO/gpio.o ../utils/utils.o -o stepper.test $(OPTS) $(LINKS)

carrinho.test: carrinho.test.c ../PWM/pwm.o ../Stepper/stepper.o ../GPIO/gpio.o ../utils/utils.o
	$(cc) carrinho.test.c ../GPIO/gpio.o ../PWM/pwm.o ../Stepper/stepper.o ../utils/utils.o -o carrinho.test $(OPTS) $(LINKS)

termios.test: termios.test.c ../GPIO/gpio.o ../Stepper/stepper.o ../utils/utils.o
	$(cc) termios.test.c ../GPIO/gpio.o ../Stepper/stepper.o ../utils/utils.o -o termios.test $(OPTS) $(LINKS)

gps.test: gps.test.c ../GPS/gps.o ../utils/utils.o
	$(cc) gps.test.c ../GPS/gps.o ../utils/utils.o -o gps.test $(OPTS) $(LINKS)

clean:
	-rm gpio.test 2> /dev/null
	-rm pwm.test 2> /dev/null
	-rm stepper.test 2> /dev/null
	-rm carrinho.test 2> /dev/null
	-rm termios.test 2> /dev/null
	-rm gps.test 2> /dev/null

