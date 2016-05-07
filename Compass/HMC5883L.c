#include "HMC5883L.h"

struct hmc5883l_t {
	int file;
	short int x, y, z;
	/*2 bytes Padding?*/
};

hmc5883l hmc5883lCreate(const char *file, int cra, int crb, int mode) {

	hmc5883l mag = (hmc5883l)malloc(sizeof(struct hmc5883l_t));

	if(mag == NULL)
		return NULL;

	mag->file = i2cOpen(file, HMC5883L_ADDR);

	if(mag->file < 0) {
		free(mag);
		return NULL;
	}

	if(hmc5883lConfigCRA(mag, cra)) {

		close(mag->file);
		free(mag);

		return NULL;

	}

	if(hmc5883lConfigCRB(mag, crb)) {

		close(mag->file);
		free(mag);

		return NULL;

	}

	if(hmc5883lConfigMode(mag, mode)) {

		close(mag->file);
		free(mag);

		return NULL;

	}

	return mag;
}

int hmc5883lConfigCRA(hmc5883l mag, int cra) {
	unsigned char buffer[] = {HMC5883L_CRA, cra};

	if(i2cWrite(mag->file, buffer, 2))
		return -1;

	return 0;
}

int hmc5883lConfigCRB(hmc5883l mag, int crb) {

	unsigned char buffer[] = {HMC5883L_CRB, crb};

	if(i2cWrite(mag->file, buffer, 2))
		return -1;

	return 0;
}

int hmc5883lConfigMode(hmc5883l mag, int mode) {
	unsigned char buffer[] = {HMC5883L_R_MODE, mode};

	if(i2cWrite(mag->file, buffer, 2))
		return -1;

	return 0;
}

int hmc5883lRead(hmc5883l mag, struct axis_t *axis) {
	unsigned char reg = HMC5883L_DATA_X;
	unsigned char buffer[6];

	if(i2cWrite(mag->file, &reg, 1))
		return -1;
	if(i2cRead(mag->file, buffer, 6))
		return -2;

	axis->x = mag->x = (buffer[0] << 8) + buffer[1];
	axis->z = mag->z = (buffer[2] << 8) + buffer[3];
	axis->y = mag->y = (buffer[4] << 8) + buffer[5];

	return 0;
}

int hmc5883lDestroy(hmc5883l mag) {
	close(mag->file);
	free(mag);
	return 0;
}
