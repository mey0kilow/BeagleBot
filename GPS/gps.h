#ifndef GPS_H
#define GPS_H

#include <pthread.h>
#include <unistd.h>
#include <termios.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "../utils/utils.h"

#ifdef NO_CFSETSPEED
	#define cfsetspeed(device, speed) cfsetospeed(device, speed); cfsetispeed(device, speed)
#endif

#ifndef NMEA_STRING_SIZE
#define NMEA_STRING_SIZE 52
#endif

#ifndef WINDOW
#define WINDOW 10
#endif

#define GPS_BUFF_SIZE NMEA_STRING_SIZE*WINDOW

double dms2dd(double val);
#define dms2dd(val) ((int)((val)/100) + ((val)-(int)((val)/100)*100)/60)

struct gpsPos_t {
	double lat, lon;
};

struct gps_t;

typedef struct gpsPos_t gpsPos;
typedef struct gps_t * gps;

gps gpsCreate(const char *device);
int gpsDestroy(gps g);
gpsPos gpsRead(gps g);

#endif
