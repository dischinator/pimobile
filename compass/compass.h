#ifndef COMPASS_H_INCLUDED
#define COMPASS_H_INCLUDED

static const char COMPASS_DEFAULT_GAIN;
static const char COMPASS_MODE_CONTINUOUS;
static const char COMPASS_MODE_SINGLE;
static const char COMPASS_MODE_IDLE;

int initializeCompass();
int setCompassConfigurationRegisterA(int mode);
int setCompassConfigurationRegisterB(int mode);
int setCompassModeRegister(int mode);
int getCompassData( float *angle, float *x, float *y );

#endif
