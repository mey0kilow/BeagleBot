#include "HMC5883L.h"

struct hmc5883l_t {
	int file;
	struct axis_t max, min;
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

	mag->max.x = HMC5883L_DEFAULT_X_MAX;
	mag->max.y = HMC5883L_DEFAULT_Y_MAX;
	mag->max.z = HMC5883L_DEFAULT_Z_MAX;

	mag->min.x = HMC5883L_DEFAULT_X_MIN;
	mag->min.y = HMC5883L_DEFAULT_Y_MIN;
	mag->min.z = HMC5883L_DEFAULT_Z_MIN;

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

	axis->x = (buffer[0] << 8) + buffer[1];
	axis->z = (buffer[2] << 8) + buffer[3];
	axis->y = (buffer[4] << 8) + buffer[5];

#ifdef HMC5883L_BOTH_IRON

#define HMC5883L_HARD_IRON
#define HMC5883L_SOFT_IRON

#elif HMC5883L_NONE_IRON

#undef HMC5883L_HARD_IRON
#undef HMC5883L_SOFT_IRON

#endif

#ifdef HMC5883L_HARD_IRON
	axis->x -= (mag->max.x + mag->min.x)/2;
	axis->z -= (mag->max.z + mag->min.z)/2;
	axis->y -= (mag->max.y + mag->min.y)/2;
#endif

#ifdef HMC5883L_SOFT_IRON
	axis->x = (axis->x - mag->min.x) / (mag->max.x - mag->min.x) * 2 - 1;
	axis->z = (axis->z - mag->min.z) / (mag->max.z - mag->min.z) * 2 - 1;
	axis->y = (axis->y - mag->min.y) / (mag->max.y - mag->min.y) * 2 - 1;
#endif

	return 0;
}

int hmc5883lDestroy(hmc5883l mag) {
	close(mag->file);
	free(mag);
	return 0;
}
