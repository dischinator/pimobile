#include <stdio.h>
#include <fcntl.h>

#include <unistd.h>
#include <sys/ioctl.h>

#include <linux/i2c-dev.h>
#include <math.h>
#include "compass.h"

static const char COMPASS_DEFAULT_GAIN = 0x20;

static const char COMPASS_MODE_CONTINUOUS = 0b00000000;
static const char COMPASS_MODE_SINGLE = 0b00000001;
static const char COMPASS_MODE_IDLE = 0b00000011;

static const char HMC5883L_I2C_ADDR = 0x1E;
static const float SCALE = 0.92;
static const int X_OFFSET = 11;
static const int Y_OFFSET = -107;

int i2cFileHandle;

void selectCompassDevice(int addr, char * name)
{
    if (ioctl(i2cFileHandle, I2C_SLAVE, addr) < 0)
    {
        fprintf(stderr, "%s not present\n", name);
    }
}

int writeToCompassDevice(int reg, int val)
{
    char buf[2];
    buf[0]=reg;
    buf[1]=val;

    if (write(i2cFileHandle, buf, 2) != 2)
    {
        fprintf(stderr, "Can't write to ADXL345\n");
        return 1;
    }
    return 0;
}

int initializeCompass()
{
    if ((i2cFileHandle = open("/dev/i2c-1", O_RDWR)) < 0)
    {
        // Open port for reading and writing
        fprintf(stderr, "Failed to open i2c bus\n");
        return 1;
    }

    /* initialise ADXL345 */

    selectCompassDevice(HMC5883L_I2C_ADDR, "HMC5883L");
    
    if( setCompassConfigurationRegisterB(COMPASS_DEFAULT_GAIN) == 1 ) {
        return 1;
    }
    
    return 0;
}

int setCompassConfigurationRegisterA(int mode)
{
    return writeToCompassDevice(0x00, mode);
}

int setCompassConfigurationRegisterB(int mode)
{
    return writeToCompassDevice(0x01, mode);
}

int setCompassModeRegister(int mode)
{
    return writeToCompassDevice(0x02, mode);
}

int getCompassData( float *angle, float *x, float *y ) {
    unsigned char buf[16];
    buf[0] = 0x03;
    if ((write(i2cFileHandle, buf, 1)) != 1)
    {
        // Send the register to read from
        printf("{\"error\":\"Error writing to i2c slave\"}\n");
        return 1;
    }

    // wait until ready
    usleep( 5000 );

    if (read(i2cFileHandle, buf, 6) != 6) {
        printf("{\"error\":\"Unable to read from HMC5883L\"}\n");
        return 1;
    } else {
        *x = ((short)((buf[0] << 8) | buf[1]) - X_OFFSET) * SCALE;
        *y = ((short)((buf[4] << 8) | buf[5]) - Y_OFFSET) * SCALE;

        *angle = atan2(*y, *x);

        if (*angle < 0) {
            *angle += 2 * M_PI;
        }

        // convert to degree
        *angle *= 180 / M_PI;
    }
    return 0;
}
