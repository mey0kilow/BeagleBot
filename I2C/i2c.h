#ifndef I2C_h
#define I2C_h

#include <errno.h>
#include <stdio.h> //?
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int i2cOpen(const char *filename, int addr);
int i2cWrite(int file, unsigned char *buffer, int buffersize);
int i2cRead(int file, unsigned char *buffer, int buffersize);
int i2cClose(int file);

#endif
