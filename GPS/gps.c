#include "gps.h"

struct gps_t {
	int fd, measureNum;
	double acquisitionTime;
	struct gpsPos_t position, measure[WINDOW];
	char *readerPos, *recognizerPos, rotatebuff[GPS_BUFF_SIZE];
	pthread_t readerThread, recognizerThread, extractorThread, updaterThread;
	pthread_mutex_t fixbuffMutex, measureMutex, positionMutex;
	char fixbuff[NMEA_STRING_SIZE];
	bool recognizerIsRuning;
};

void *gpsReader(void *arg);
void *gpsRecognizer(void *arg);
void *gpsExtractor(void *arg);
void *gpsUpdater(void *arg);

void *gpsReader(void *arg) {
	struct gps_t *g = (struct gps_t *) arg;
	int r, size = sizeof(char)*NMEA_STRING_SIZE;

	while(1) {
		/*If the next read shall overpass the buffer*/
		if(g->readerPos + size > g->rotatebuff + GPS_BUFF_SIZE) {
			/*Make size to fit on the buffer*/
			size = (g->rotatebuff + GPS_BUFF_SIZE) - g->readerPos;
			r = read(g->fd, g->readerPos, size);
			/*Back to the begin of the rotate buffer*/
			g->readerPos = g->rotatebuff;

			continue;
		}

		/*If we read a uncomplete string on the last read*/
		if(size < sizeof(char)*NMEA_STRING_SIZE) {
			/*Read the unreaded size of the string*/
			size = sizeof(char)*NMEA_STRING_SIZE - r;
			r = read(g->fd, g->readerPos, size);
			/*Advance reader pointer according de number of bytes actually read*/
			g->readerPos += r;
			/*Makes size the size of the nmea string again*/
			size = sizeof(char)*NMEA_STRING_SIZE;

			continue;
		}

		/*Try to read a complete nmea sentence*/
		r = read(g->fd, g->readerPos, size);
		/*Advance reader pointer according de number of bytes actually read*/
		g->readerPos += r;

		// give a minute to our gps...
		delay(100);

		/* Make sure that we wont overwrite recognizer's unprocessed data*/
		while(g->recognizerIsRuning && g->readerPos > g->recognizerPos-NMEA_STRING_SIZE
				&& g->readerPos < g->recognizerPos+NMEA_STRING_SIZE);		//isso ta bloqueando tudo

	}
}

void *gpsRecognizer(void *arg) {
	int i;
	struct gps_t *g = (struct gps_t *) arg;
	char *secondPos;

	delay(1000);
	g->recognizerIsRuning = true;

	while(1) {

		/*Find a '$'*/
		while(*g->recognizerPos != '$') {
			if(*g->recognizerPos == '-') {
				g->recognizerIsRuning = false;
				while(*g->recognizerPos == '-');
				g->recognizerIsRuning = true;
			}
			++g->recognizerPos;
			if(g->recognizerPos > g->rotatebuff + GPS_BUFF_SIZE)
				g->recognizerPos = g->rotatebuff;
		}

		/*Find a '*'*/
		if(++g->recognizerPos > g->rotatebuff + GPS_BUFF_SIZE)
			g->recognizerPos = g->rotatebuff;

		secondPos = g->recognizerPos;
		while(*secondPos != '*') {
			if(*secondPos == '-') {
				g->recognizerIsRuning = false;
				while(*secondPos == '-');
				g->recognizerIsRuning = true;
			}
			++secondPos;
			if(secondPos > g->rotatebuff + GPS_BUFF_SIZE)
				secondPos = g->rotatebuff;
		}

		i = 0;

		pthread_mutex_lock(&g->fixbuffMutex);

		/*Sentence broken in rotate*/
		if(secondPos < g->recognizerPos) {
			while(g->recognizerPos < g->rotatebuff + GPS_BUFF_SIZE) {
				g->fixbuff[i] = *g->recognizerPos;
				*g->recognizerPos = '-'; /*mark char as used*/
				++i;
				++g->recognizerPos;
			}
			g->recognizerPos = g->rotatebuff;
		}

		while(g->recognizerPos < secondPos) {
			g->fixbuff[i] = *g->recognizerPos;
			*g->recognizerPos = '-'; /*mark char as used*/
			++i;
			++g->recognizerPos;
		}

		/*Copy '*' */
		g->fixbuff[i++] = secondPos[0];
		/*Copy first checksum char*/
		g->fixbuff[i++] = secondPos[1];
		/*Copy second checksum char*/
		g->fixbuff[i] = secondPos[2];

		pthread_mutex_unlock(&g->fixbuffMutex);

		/*Call extractor*/
		pthread_create(&g->extractorThread, NULL, gpsExtractor, (void*) g);

	}
}

void *gpsExtractor(void *arg) {
	int i, sum = 0;
	double lat, lon, time;
	struct gps_t *g = (struct gps_t *) arg;
	char localbuffer[NMEA_STRING_SIZE];
	char latdir, londir, valid;

	pthread_mutex_lock(&g->fixbuffMutex);

	/*Check check sum*/
	for(i = 0; g->fixbuff[i] != '*'; i++)
		sum ^= g->fixbuff[i];

	g->fixbuff[i+1] -= '0';
	if(g->fixbuff[i+1] > 10)
		g->fixbuff[i+1] -= 7;

	g->fixbuff[i+2] -= '0';
	if(g->fixbuff[i+2] > 10)
		g->fixbuff[i+2] -= 7;

	/*sum should be equal to checksum, witch is a hexa value. Here we convert, em subtract
	 * if the result is zero, the checksum pass, else, it fail, and the thread ends, without
	 * use the data of this read, because its corrupted*/
	if(sum - (g->fixbuff[i+1]*16 + g->fixbuff[i+2])) {
		pthread_mutex_unlock(&g->fixbuffMutex);
		pthread_exit((void*)-1);
	}

	/*Ok, checksum pass, lets make a copy of this sentence and unlock the fixbuff for other threads*/
	for(i = 0; i < NMEA_STRING_SIZE; i++)
		localbuffer[i] = g->fixbuff[i];

//	pthread_mutex_unlock(&g->fixbuffMutex);

	/*Read the sentece. If we cant read this number of matchs, its probably
	 * a "can't fix" sentence, so we can discard*/
	if(sscanf(localbuffer, "GPGLL,%lf,%c,%lf,%c,%lf,%c,%c*%*s",
			&lat, &latdir, &lon, &londir, &time, &valid, &valid) < 7) {
		pthread_exit((void*)-1);
	}

	if(valid != 'A') {
		pthread_exit((void*)-1);
	}

	pthread_mutex_lock(&g->measureMutex);

	g->measure[g->measureNum].lat = dms2dd(lat);
	g->measure[g->measureNum].lon = dms2dd(lon);
	++g->measureNum;

	pthread_mutex_unlock(&g->measureMutex);

	/*If we are done to call the updater*/
	if(g->measureNum == WINDOW) {
		g->acquisitionTime = time;
		g->measureNum = 0;
		pthread_create(&g->updaterThread, NULL, gpsUpdater, (void*) g);
	}

	pthread_mutex_unlock(&g->fixbuffMutex);

	pthread_exit((void*)0);

}

void *gpsUpdater(void *arg) {
	int i;
	struct gps_t *g = (struct gps_t *) arg;
	struct gpsPos_t avg;

	avg.lat = 0;
	avg.lon = 0;

	pthread_mutex_lock(&g->measureMutex);

	for(i = 0; i < WINDOW; i++) {
		avg.lat += g->measure[i].lat;
		avg.lon += g->measure[i].lon;
	}

	pthread_mutex_unlock(&g->measureMutex);

	pthread_mutex_lock(&g->positionMutex);

	g->position.lat = avg.lat/WINDOW;
	g->position.lon = avg.lon/WINDOW;

	pthread_mutex_unlock(&g->positionMutex);

	pthread_exit((void*)0);
}

gpsPos gpsRead(gps g) {
	struct gpsPos_t position;

	pthread_mutex_lock(&g->positionMutex);

	position.lat = g->position.lat;
	position.lon = g->position.lon;

	pthread_mutex_unlock(&g->positionMutex);

	return position;
}

gps gpsCreate(const char *device) {
	struct termios tty;

	struct gps_t *g = (struct gps_t *)malloc(sizeof(struct gps_t));
	if(g == NULL)
		return NULL;

	/*Clean the struct*/
	g->acquisitionTime = 0;
	zeros(&g->position, sizeof(gpsPos));
	zeros(&g->measure, sizeof(gpsPos)*WINDOW);
	g->measureNum = 0;
	g->readerPos = g->recognizerPos = g->rotatebuff;
	zeros(&g->rotatebuff, sizeof(char)*GPS_BUFF_SIZE);
	zeros(&g->fixbuff, sizeof(char)*NMEA_STRING_SIZE);
	g->recognizerIsRuning = false;

	/*Open tty*/
	g->fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);
	if(g->fd < 0)
		return NULL;

	zeros(&tty, sizeof(struct termios));

	/*Get tty state*/
	if(tcgetattr(g->fd, &tty) < 0)
		return NULL;

	/*Change tty configs*/
	/*115200,cs8*/
	cfsetspeed(&tty, B115200);
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;

	/*no echo...*/
	tty.c_lflag = 0;

	/*no flow control*/
	tty.c_iflag &= ~(IXON | IXOFF | IXANY);

	/*no parity*/
	tty.c_cflag &= ~(PARENB | PARODD);

	/*Set new tty config*/
	if(tcsetattr(g->fd, TCSANOW, &tty) != 0)
		return NULL;

	if(pthread_mutex_init(&g->fixbuffMutex, NULL)) {
		return NULL;
	}

	if(pthread_mutex_init(&g->measureMutex, NULL)) {
		pthread_mutex_destroy(&g->fixbuffMutex);
		return NULL;
	}

	if(pthread_create(&g->readerThread, NULL, gpsReader, (void*) g)) {
		pthread_mutex_destroy(&g->fixbuffMutex);
		pthread_mutex_destroy(&g->measureMutex);
		return NULL;
	}

	if(pthread_create(&g->recognizerThread, NULL, gpsRecognizer, (void*) g)) {
		pthread_mutex_destroy(&g->fixbuffMutex);
		pthread_mutex_destroy(&g->measureMutex);
		pthread_cancel(g->readerThread);
		return NULL;
	}

	return g;

}

int gpsDestroy(gps g) {

	pthread_cancel(g->recognizerThread);
	pthread_cancel(g->readerThread);
	pthread_mutex_destroy(&g->fixbuffMutex);
	pthread_mutex_destroy(&g->measureMutex);
	close(g->fd);

	return 0;

}
