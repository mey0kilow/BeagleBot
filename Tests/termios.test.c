#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include "../Stepper/stepper.h"
#include "../utils/utils.h"

int main(int argc, char *argv[]) {

	if(argc < 3) {
		fprintf(stderr, "Usage: %s -t [TYPE] [OPTIONS]\n-t\tType: Wave (w)\n\t\tFull (f)\n\t\tHalf(h)\n-n\tNumber of steps\n-f\tFreq.\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int i, n = 1, t = -1;
	double f = 120;

	for(i = 0; i < argc; i++) {
		if(argv[i][0] == '-') {
			switch(argv[i][1]) {
				case 't':
					switch(argv[i+1][0]) {
						case 'w':
							t = WAVE;
							break;
						case 'f':
							t = FULL;
							break;
						case 'h':
							t = HALF;
							break;
					}
					break;
				case 'n':
					n = atoi(argv[i+1]);
					break;
				case 'f':
					f = atoi(argv[i+1]);
					break;
			}
		}
	}

	printf("Selected: %d steps at %.2f Hz, %s step mode\n", n, f,
		t == WAVE ? "wave": t == FULL ? "full" : t == HALF ? "half" : "wrong");

	struct termios old, new;
	char c, d, e;

	stepper s = stepperCreate(P9_11, P9_13, P9_15, P9_17, t, f);

	tcgetattr(0, &old);
	new = old;
	new.c_lflag &= ~ECHO;
	new.c_lflag &= ~ICANON;
	tcsetattr(0, TCSANOW, &new);
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);

	while(read(0, &c, sizeof(c)) && read(0, &d, sizeof(d)) && read(0, &e, sizeof(e)) && c == 27) {
		switch(e) {
			case 'C':
				printf("Direita: ");
				if(stepperGetPos(s) < 50)
					step(s, n);
				printf("%d\n", stepperGetPos(s));
				break;
			case 'D':
				printf("Esquerda: ");
				if(stepperGetPos(s) > -50)
					step(s, -n);
				printf("%d\n", stepperGetPos(s));
				break;
			default:
				printf("%c\n", d);
		}
	}

	stepperDestroy(s);
	tcsetattr(0, TCSANOW, &old);

	exit(EXIT_SUCCESS);

}
