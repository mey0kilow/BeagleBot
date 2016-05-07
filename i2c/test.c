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
#include "../utils/utils.h"
#include "../GPIO/gpio.h"

#define HMC5883L_CRA 0x00
#define HMC5883L_CRB 0x01
#define HMC5883L_R_MODE 0x02
#define HMC5883L_DATA_X 0x03
#define HMC5883L_DATA_Y 0x07
#define HMC5883L_DATA_Z 0x05
#define HMC5883L_STATUS 0x09
#define HMC5883L_R_ID_A 0x0A
#define HMC5883L_R_ID_B 0x0B
#define HMC5883L_R_ID_C 0x0C

#define HMC5883L_ID_A 'H'
#define HMC5883L_ID_B '4'
#define HMC5883L_ID_C '3'

#define HMC5883L_CRA_1_SAMPLE 0b00000000
#define HMC5883L_CRA_2_SAMPLE 0b00100000
#define HMC5883L_CRA_4_SAMPLE 0b01000000
#define HMC5883L_CRA_8_SAMPLE 0b01100000

#define HMC5883L_CRA_00_75HZ 0b00000000
#define HMC5883L_CRA_01_50HZ 0b00000100
#define HMC5883L_CRA_03_00HZ 0b00001000
#define HMC5883L_CRA_07_50HZ 0b00001100
#define HMC5883L_CRA_15_00HZ 0b00010000
#define HMC5883L_CRA_30_00HZ 0b00010100
#define HMC5883L_CRA_75_00HZ 0b00011000

#define HMC5883L_CRA_NORMAL_MEASURE 0b00000000
#define HMC5883L_CRA_POSITIVE_MEASURE 0b00000001
#define HMC5883L_CRA_NEGATIVE_MEASURE 0b00000010

#define HMC5883L_CRB_0_88_GAIN 0b00000000
#define HMC5883L_CRB_1_30_GAIN 0b00100000
#define HMC5883L_CRB_1_90_GAIN 0b01000000
#define HMC5883L_CRB_2_50_GAIN 0b01100000
#define HMC5883L_CRB_4_00_GAIN 0b10000000
#define HMC5883L_CRB_4_70_GAIN 0b10100000
#define HMC5883L_CRB_5_60_GAIN 0b11000000
#define HMC5883L_CRB_8_10_GAIN 0b11100000

#define HMC5883L_0_88_GAIN 1370
#define HMC5883L_1_30_GAIN 1090
#define HMC5883L_1_90_GAIN 820
#define HMC5883L_2_50_GAIN 660
#define HMC5883L_4_00_GAIN 440
#define HMC5883L_4_70_GAIN 390
#define HMC5883L_5_60_GAIN 330
#define HMC5883L_8_10_GAIN 230

#define HMC5883L_MODE_HIGH_SPEED_I2C 0b11111100
#define HMC5883L_MODE_CONTINUOUS 0b00000000
#define HMC5883L_MODE_SINGLE 0b00000001
#define HMC5883L_MODE_IDLE 0b00000010

#define HMC5883L_STATUS_LOCK 0b00000010
#define HMC5883L_STATUS_DRDY 0b00000001

#define PI 3.14159265358979323846264338327950288419716939937510

static int file;
gpio drdy;

struct Axis_t {
	unsigned x : 16;
	unsigned z : 16;
	unsigned y : 16;
}; 

int i2c_open(char *filename, int addr);
int i2c_write(int file, unsigned char *buffer, int buffersize);
int i2c_read(int file, unsigned char *buffer, int buffersize);
void signalEnd(int signo);

int main(void) {
	short int raw_x = 0, raw_y = 0, raw_z = 0;
	long double x = 0, y = 0, z = 0, direction = 0;
	struct Axis_t axis;

	signal(SIGINT, signalEnd);

	drdy = gpioExport(P9_17);
	if(drdy == NULL) {
		fprintf(stderr, "GPIO export failed\n");
		exit(-1);
	}

	gpioSet(drdy, "in");

	file = i2c_open("/dev/i2c-1", 0x1E);
	if(file == -1) {
		fprintf(stderr, "Open failed\n");
		exit(-1);
	} else {
		fprintf(stdout, "I2C open...\n");
	}

	if(hmc5883l_config(file, (HMC5883L_CRA_1_SAMPLE | HMC5883L_CRA_75_00HZ | HMC5883L_CRA_NORMAL_MEASURE), HMC5883L_CRB_0_88_GAIN, HMC5883L_MODE_SINGLE)) {
		fprintf(stderr, "Config failed\n");
		exit(-1);
	}

	while(1) {
		hmc5883l_config_mode(file, HMC5883L_MODE_SINGLE);

		while(!gpioRead(drdy));

		hmc5883l_read(file, &axis);

		raw_x = axis.x;
		raw_y = axis.y;
		raw_z = axis.z;

		if(raw_x > 2047 || raw_x < -2048) {
			printf("x\n");
			sleep(1);
			continue;
		}

		if(raw_y > 2047 || raw_y < -2048) {
			printf("y\n");
			sleep(1);
			continue;
		}

		if(raw_z > 2047 || raw_z < -2048) {
			printf("z\n");
			sleep(1);
			continue;
		}

		direction = 180 * atan2l(-raw_y, raw_x)/M_PI;
		if(direction < 0) direction += 360;

		printf("x: %hd, y: %hd, z: %hd, Direction: %lf\n", raw_x, raw_y, raw_z, direction);
		
		sleep(1);
	}

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

int hmc5883l_config(int file, int cra, int crb, int mode) {
	unsigned char buffer[] = {HMC5883L_CRA, cra, HMC5883L_CRB, crb, HMC5883L_R_MODE, mode};

	if(i2c_write(file, buffer, 2))
		return -1;

	if(i2c_write(file, buffer+2, 2))
		return -1;

	if(i2c_write(file, buffer+4, 2))
		return -1;

	return 0;
}

int hmc5883l_config_cra(int file, int cra) {
	unsigned char buffer[] = {HMC5883L_CRA, cra};

	if(i2c_write(file, buffer, 2))
		return -1;

	return 0;
}

int hmc5883l_config_crb(int file, int crb) {

	unsigned char buffer[] = {HMC5883L_CRB, crb};

	if(i2c_write(file, buffer, 2))
		return -1;

	return 0;
}

int hmc5883l_config_mode(int file, int mode) {
	unsigned char buffer[] = {HMC5883L_R_MODE, mode};

	if(i2c_write(file, buffer, 2))
		return -1;

	return 0;
}

int hmc5883l_read(int file, struct Axis_t *axis) {
	unsigned char reg = HMC5883L_DATA_X;
	if(i2c_write(file, &reg, 1))
		return -1;
	if(i2c_read(file, (unsigned char*) axis, 6))
		return -2;
	return 0;
}
