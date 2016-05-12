#include "i2c.h"

int i2cOpen(const char *filename, int addr) {
	int file;

	// Open I2C file
	if((file = open(filename, O_RDWR)) < 0)
		return -1;

	// Join I2C.
	if(ioctl(file, I2C_SLAVE, addr) < 0) {
		close(file);
		return -1;
	}

	return file;
}

int i2cWrite(int file, unsigned char *buffer, int buffersize) {
	int aux = write(file, buffer, buffersize);
	if(aux != buffersize) {
		perror("Write failed");
		return -1;
	}
	return 0;

}

int i2cRead(int file, unsigned char *buffer, int buffersize) {
	if(read(file, buffer, buffersize) != buffersize)
		return -1;
	return 0;
}
