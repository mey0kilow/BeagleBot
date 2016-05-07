#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <math.h>
#include "utils.h"

#define PI 3.14159265358979323846264338327950288419716939937510

static int file;

int i2c_open(char *filename, int addr);
int i2c_write(int file, unsigned char *buffer, int buffersize);
int i2c_read(int file, unsigned char *buffer, int buffersize);
void signalEnd(int signo);

struct axis_t {
	unsigned x : 16;
	unsigned z : 16;
	unsigned y : 16;
};

int main(void) {
	short int raw_x = 0, raw_y = 0, raw_z = 0;
	long double x = 0, y = 0, z = 0, direction = 0;
	char *filename = "/dev/i2c-1";
	int addr = 0x1E;     // The I2C address of the device
//					1   , 2   , 3
//	unsigned char buffer[10] = {	0x3C, 0x00, 0x70,		// 15Hz
//					0x3C, 0x01, 0xA0,		// +/- 0.88 mG
//					0x3C, 0x02, 0x00};		// Continuos mode
//					1   , 3    2   ,
//	unsigned char buffer[10] = {	0x3C, 0x70, 0x00,		// 15Hz
//					0x3C, 0xA0, 0x01,		// +/- 0.88 mG
//					0x3C, 0x00, 0x02};		// Continuos mode
//					2,    1,    3
//	unsigned char buffer[10] = {	0x00, 0x3C, 0x70,		// 15Hz
//					0x01, 0x3C, 0xA0,		// +/- 0.88 mG
//					0x02, 0x3C, 0x00};		// Continuos mode
//					2   , 3   , 1   
//	unsigned char buffer[10] = {	0x00, 0x70, 0x3C,		// 15Hz
//					0x01, 0xA0, 0x3C,		// +/- 0.88 mG
//					0x02, 0x00, 0x3C};		// Continuos mode
//					3   , 1   , 2    
//	unsigned char buffer[10] = {	0x70, 0x3C, 0x00,		// 15Hz
//					0xA0, 0x3C, 0x01,		// +/- 0.88 mG
//					0x00, 0x3C, 0x02};		// Continuos mode
//					3   , 2   , 1   
//	unsigned char buffer[10] = {	0x70, 0x00, 0x3C,		// 15Hz
//					0xA0, 0x01, 0x3C,		// +/- 0.88 mG
//					0x00, 0x02, 0x3C};		// Continuos mode

	unsigned char buffer[10];

	struct axis_t axis;

	signal(SIGINT, signalEnd);

	file = i2c_open(filename, 0x1E);
	if(file == -1) {
		fprintf(stderr, "Open failed\n");
		exit(-1);
	} else {
		fprintf(stdout, "I2C open...\n");
	}

	buffer[0] = 0x3C;
	buffer[1] = 0x00;
	buffer[2] = 0x70;

	if(i2c_write(file, buffer, 3)) {
		fprintf(stderr, "Write failed\n");
		exit(-1);
	} else {
		fprintf(stdout, "Freq. set to 15Hz\n");
	}

	buffer[1] = 0x01;
	buffer[2] = 0xA0;

	if(i2c_write(file, buffer, 3)) {
		fprintf(stderr, "Write failed\n");
		exit(-1);
	} else {
		fprintf(stdout, "Gain set to 5\n");
	}

	buffer[1] = 0x02;
	buffer[2] = 0x00;

	if(i2c_write(file, buffer, 3)) {
		fprintf(stderr, "Write failed\n");
		exit(-1);
	} else {
		fprintf(stdout, "Continuos mode selected\n");
	}

	buffer[1] = 0x03;

	if(i2c_write(file, buffer, 2)) {
		fprintf(stderr, "Write failed\n");
		exit(-1);
	} else {
		fprintf(stdout, "Prepared to read...\n");
	}
	

	while(1){

		buffer[0] = 0x3D;
		buffer[1] = 0x06;
		if(i2c_write(file, buffer, 2)) {
			fprintf(stderr, "Write failed\n");
			exit(-1);
		}


		if(i2c_read(file, (unsigned char *) &axis, 6)) {
			fprintf(stderr, "Read failed\n");
			exit(-1);
		}

		raw_x = axis.x;
		raw_y = axis.y;
		raw_z = axis.z;
		
		direction = 180 * atan2l(-raw_y,raw_x)/M_PI;
		if(direction < 0) direction +=360;

		buffer[1] = 0x3C;
		buffer[0] = 0x03;

		if(i2c_write(file, buffer, 2)) {
			fprintf(stderr, "Write failed\n");
			exit(-1);
		}

		printf("x: %f mG, y: %f mG, z: %f mG\tDirection: %lf°\n", raw_x, raw_y, raw_z, direction);
		fflush(stdout);

		delay(65);

	}

	return 0;
	

}

void signalEnd(int signo) {
	close(file);
	exit(0);
}

int i2c_open(char *filename, int addr) {
	int file;

	// Open I2C file
	if((file = open(filename, O_RDWR)) < 0) {
		perror("Failed to open the i2c bus");
		return -1;
	}

	// Join I2C.
	if(ioctl(file, I2C_SLAVE, addr) < 0) {
		fprintf(stderr, "Failed to acquire bus access and/or talk to slave.\n");
		return -1;
	}

	return file;
}

int i2c_write(int file, unsigned char *buffer, int buffersize) {
	if(write(file, buffer, buffersize) != buffersize)
		return -1;
	return 0;

}

int i2c_read(int file, unsigned char *buffer, int buffersize) {
	if(read(file, buffer, buffersize) != buffersize)
		return -1;
	return 0;
}
