#ifndef PIMOBILE_H_INCLUDED
#define PIMOBILE_H_INCLUDED

#include <wiringPi.h>

#define GPIO_TONE 21

void pimobileInitializeGpio();

char pimobileGetFrontLightStatus();
void pimobileSetFrontLight(char status);

void pimobileSetLeftMotorSpeed(short speed);
void pimobileSetRightMotorSpeed(short speed);
void pimobileSetBothMotorSpeed(short speed);
void pimobileSetMotorsSpeedAndDirection(short speedLeft, short speedRight, short directionLeft, short directionRight);
void pimobileStop(void);
void pimobileTurn(short direction, short speed);

void pimobileSetLeftMotorDirection(short direction);
void pimobileSetRightMotorDirection(short direction);
float getAngleDistance(float startAngle, float targetAngle);
int pimobileTurnTo(float targetAngle);
void pimobileCalculatePosition(double *Sx, double *Sy, double x1, double y1, double x2, double y2, double oAngle1, double oAngle2, double angleXAxis);

#endif
