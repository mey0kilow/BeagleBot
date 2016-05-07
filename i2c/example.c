#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <unistd.h>
#include "i2c-dev.h"


int i2cset(int i2cbus, int address, int daddress, int value)
{
	char *end;
	int res, size, file;
	char filename[20];


	size = I2C_SMBUS_BYTE;

	sprintf(filename, "/dev/i2c-%d", i2cbus);
	file = open(filename, O_RDWR);
	if (file<0) {
		if (errno == ENOENT) {
			fprintf(stderr, "Error: Could not open file "
				"/dev/i2c-%d: %s\n", i2cbus, strerror(ENOENT));
		} else {
			fprintf(stderr, "Error: Could not open file "
				"`%s': %s\n", filename, strerror(errno));
			if (errno == EACCES)
				fprintf(stderr, "Run as root?\n");
		}
		exit(1);
	}

	if (ioctl(file, I2C_SLAVE, address) < 0) {
		fprintf(stderr,
			"Error: Could not set address to 0x%02x: %s\n",
			address, strerror(errno));
		return -errno;
	}

	res = i2c_smbus_write_byte_data(file, daddress, value);
	if (res < 0) {
		fprintf(stderr, "Warning - write failed, filename=%s, daddress=%d\n",
			filename, daddress);
	}
	
	return 0;
}


int i2cget(int i2cbus, int address, int daddress)
{
	char *end;
	int res, size, file;
	
	char filename[20];

	size = I2C_SMBUS_BYTE;


	sprintf(filename, "/dev/i2c-%d", i2cbus);
	file = open(filename, O_RDWR);
	if (file<0) {
		if (errno == ENOENT) {
			fprintf(stderr, "Error: Could not open file "
				"/dev/i2c-%d: %s\n", i2cbus, strerror(ENOENT));
		} else {
			fprintf(stderr, "Error: Could not open file "
				"`%s': %s\n", filename, strerror(errno));
			if (errno == EACCES)
				fprintf(stderr, "Run as root?\n");
		}
		exit(1);
	}

	if (ioctl(file, I2C_SLAVE, address) < 0) {
		fprintf(stderr,
			"Error: Could not set address to 0x%02x: %s\n",
			address, strerror(errno));
		return -errno;
	}

	res = i2c_smbus_write_byte(file, daddress);
	if (res < 0) {
		fprintf(stderr, "Warning - write failed, filename=%s, daddress=%d\n",
			filename, daddress);
	}
	
	res = i2c_smbus_read_byte_data(file, daddress);
	close(file);

	if (res < 0) {
		fprintf(stderr, "Error: Read failed, res=%d\n", res);
		exit(2);
	}

	return res;
}


int main(int argc, char *argv[])
{
	int i2cbus, address, daddress;
	short x_val, y_val, z_val;

	if(argc < 2) {
		fprintf(stderr, "Usage:  %s <i2c-bus> <i2c-address>\n", argv[0]);
		exit(1);
	}

	i2cbus   = atoi(argv[1]);
	address  = atoi(argv[2]);

	//continuously read and print the values for each axis
	while(1){
		//set to continuous measurement mode
		i2cset(i2cbus, address, 2, 0);

		x_val = ((i2cget(i2cbus, address, 3) << 8) + i2cget(i2cbus, address, 4));

		//small wait
		usleep(50000);			

		z_val = ((i2cget(i2cbus, address, 5) << 8) + i2cget(i2cbus, address, 6));

		//small wait
		usleep(50000);	

		y_val = ((i2cget(i2cbus, address, 7) << 8) + i2cget(i2cbus, address, 8));

		fprintf(stderr, "x value = %d, y value = %d, z value = %d\r", x_val, y_val, z_val);
		
		//small wait
		usleep(50000);		

	}	
	
	return 0;

}
