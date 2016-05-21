#ifndef ADXL345_H
#define ADXL345_H

#include "../I2C/i2c.h"

#define ADXL345_ADDR 0x1D // ou 0x53. Testar com i2cdetect.

/* Device Detection */
#define ADXL345_DEVID_REG 0x00
#define ADXL345_DEVID 0xE5

/* FIFO configurations*/
#define ADXL345_FIFO_CTRL 0x38

#define ADXL345_FIFO_MODE 0b01000000
#define ADXL345_BYPASS_MODE 0b00000000
#define ADXL345_STREAM_MODE 0b10000000
#define ADXL345_TRIGGER_MODE_INT1 0b11000000
#define ADXL345_TRIGGER_MODE_INT2 0b11100000

#define ADXL345_FIFO_MAX_SAMPLES 0b00011111 /*Should be &'ed  with a integer number of samples*/

#define ADXL345_FIFO_STAT 0x39

/* AXIS DATA*/
#define ADXL345_DATA_FORMAT 0x31
#define ADXL345_DATAX 0x32
#define ADXL345_DATAY 0x35
#define ADXL345_DATAZ 0x37

/* AXIS OFFSET */
#define ADXL345_OFS_LSB 0.0156
#define ADXL345_OFSX 0x1E
#define ADXL345_OFSY 0x1F
#define ADXL345_OFSZ 0x20

/* TAP detection */
#define ADXL345_TAP_DUR 0x21
#define ADXL345_TAP_DUR_LSB 0.0652

#define ADXL345_TAP_LAT 0x22
#define ADXL345_TAP_LAT_LSB 1.25

#define ADXL345_TAP_WIN 0x23
#define ADXL345_TAP_WIN_LSB 1.25

#define ADXL345_TAP_CTL 0x2A
#define ADXL345_TAP_STAT 0x2B

/* Activity detection */
#define ADXL345_ACTIV_TRSH 0x24
#define ADXL345_ACTIV_TRSH_LSB 0.0625

#define ADXL345_INACT_TRSH 0x25
#define ADXL345_INACT_TRSH_LSB 0.0625

#define ADXL345_INACT_TIME 0x26
#define ADXL345_INACT_TIME_LSB 1

#define ADXL345_ACTIV_CTLR 0x27
#define ADXL345_ACT_AC
#define ADXL345_ACT_DC

/* Free-fall detection */
#define ADXL345_FF_TRSH 0x28
#define ADXL345_FF_CTRL 0x29

/* Power control*/
#define ADXL345_RATE 0x2C
#define ADXL345_POWR 0x2D

/* Interrupt control */
#define ADXL345_INT_ENABLE 0x2E
#define ADXL345_INT_MAP 0x2F
#define ADXL345_INT_SRC 0x30

/* */

#endif
