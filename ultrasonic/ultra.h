#ifndef ULTRA_H_INCLUDED
#define ULTRA_H_INCLUDED

#include <wiringPi.h>

void initializeUltraSonicModule();
void getUltraSonicData(double *elapsedTime, double *distance);

#endif