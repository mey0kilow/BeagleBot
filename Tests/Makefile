tests: pwm.test gpio.test stepper.test carrinho.test

pwm.test: pwm.test.c
	$(cc) pwm.test.c ../PWM/pwm.o -o pwm.test $(OPTS)

gpio.test: gpio.test.c
	$(cc) gpio.test.c ../GPIO/gpio.o ../utils/utils.o -o gpio.test $(OPTS)

stepper.test: stepper.test.c
	$(cc) stepper.test.c ../Stepper/stepper.o ../GPIO/gpio.o ../utils/utils.o -o stepper.test $(OPTS)

carrinho.test: carrinho.test.c
	$(cc) carrinho.test.c ../GPIO/gpio.o ../PWM/pwm.o ../Stepper/stepper.o ../utils/utils.o -o carrinho.test $(OPTS)

clean:
	-rm gpio.test 2> /dev/null
	-rm pwm.test 2> /dev/null
	-rm stepper.test 2> /dev/null
	-rm carrinho.test 2> /dev/null

