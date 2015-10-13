// PiMobile Engine Control Library

#include <wiringPi.h>
#include <math.h>
#include <unistd.h>
#include "pimobile-api-client.h"

#ifdef DEBUG
#include <stdio.h>
#endif

static const int PWM_MAX = 1024;

static const char GPIO_LEFT_PWM = 19;
static const char GPIO_LEFT_A = 6;
static const char GPIO_LEFT_B = 13;

static const char GPIO_RIGHT_PWM = 18;
static const char GPIO_RIGHT_A = 14;
static const char GPIO_RIGHT_B = 15;

static const char GPIO_FRONT_LIGHTS = 17;

char pimobileGetFrontLightStatus() {
    return digitalRead(GPIO_FRONT_LIGHTS);
}

void pimobileSetFrontLight( char status ) {
    digitalWrite(GPIO_FRONT_LIGHTS, status % 2 );
}

void setMotorSpeed( char gpio, short speed ) {
    #ifdef DEBUG
    printf("setMotorSpeed gpio=%d speed=%d pwm=%d\n", gpio, speed, (int)floor( speed / 100.0 * PWM_MAX ) );
    #endif
    pwmWrite(gpio, (int)floor( speed / 100.0 * PWM_MAX ) );
}

void pimobileSetLeftMotorSpeed( short speed ) {
    setMotorSpeed( GPIO_LEFT_PWM, speed );
}

void pimobileSetRightMotorSpeed( short speed ) {
    setMotorSpeed( GPIO_RIGHT_PWM, speed );
}

void pimobileSetBothMotorSpeed(short speed) {
    pimobileSetRightMotorSpeed(speed);
    pimobileSetLeftMotorSpeed(speed);
}

void pimobileStop(void) {
    pimobileSetBothMotorSpeed(0);
}

void setMotorDirection( char gpioA, short gpioB, short direction ) {
    digitalWrite(gpioA, direction);
    digitalWrite(gpioB, direction ^ 1);
}

void pimobileSetLeftMotorDirection( short direction ) {
    setMotorDirection( GPIO_LEFT_A, GPIO_LEFT_B, direction );
}

void pimobileSetRightMotorDirection( short direction ) {
    setMotorDirection( GPIO_RIGHT_A, GPIO_RIGHT_B, direction );
}

void pimobileSetMotorsSpeedAndDirection( short speedLeft, short speedRight, short directionLeft, short directionRight ) {
    pimobileSetLeftMotorSpeed(speedLeft);
    pimobileSetRightMotorSpeed(speedRight);
    pimobileSetLeftMotorDirection(directionLeft);
    pimobileSetRightMotorDirection(directionRight);
}

void pimobileTurn( short direction, short speed ) {
    // direction 0=right
    pimobileSetRightMotorDirection(direction);
    pimobileSetLeftMotorDirection(direction ^ 1);
    pimobileSetBothMotorSpeed(speed);
}

void pimobileInitializeGpio() {
  pinMode(GPIO_LEFT_PWM, PWM_OUTPUT);
  pinMode(GPIO_LEFT_A, OUTPUT);
  pinMode(GPIO_LEFT_B, OUTPUT);

  pinMode(GPIO_RIGHT_PWM, PWM_OUTPUT);
  pinMode(GPIO_RIGHT_A, OUTPUT);
  pinMode(GPIO_RIGHT_B, OUTPUT);
  
  pinMode(GPIO_FRONT_LIGHTS, OUTPUT);
}

// calculates distance from startAngle to targetAngle
float getAngleDistance( float startAngle, float targetAngle ) {
    float baseDistance = startAngle - targetAngle;
    float absoluteBaseDistance = fabsf(baseDistance);

    if( absoluteBaseDistance > 180 ) {
    // shorter to pass 0
        return fabsf(absoluteBaseDistance - 360) * baseDistance / absoluteBaseDistance * -1;
    } else {
        return baseDistance;
    }
}

int pimobileTurnTo( float targetAngle ) {
    float currentAngle, differenceAbs;
    int timeout = 0;

    #ifdef DEBUG
    printf("turning to angle %0.1f\n", targetAngle);
    #endif

    // get current angle
    currentAngle = getCompassAngleFromApi();
    if(currentAngle<0) {
        return 1;
    }

    // determine shortest turn direction to reach target angle
    float difference = getAngleDistance(currentAngle, targetAngle);
    short direction = difference < 0 ? 0 : 1;   // 0=right 1=left
    short differenceSign = difference < 0 ? -1 : 1; // needed to detect when target angle was passed
  
    #ifdef DEBUG
    printf("> startAngle=%0.1f targetAngle=%0.1f difference=%0.1f turnDirection=%s\n", currentAngle, targetAngle, difference, direction == 0? "right":"left" );
    #endif

    // start turning at full speed until target angle is reached or passed
    pimobileTurn(direction, 100);
    
    char slowedDown = 0;

    // todo: implement timeout to avoid infinite loop when vehicle is stuck
    do {
        if(timeout > 600) {
            // if target angle is not reached after 60 seconds assume vehicle is stuck and abort
            #ifdef DEBUG
            printf("\ntimeout while turning\n");
            #endif
            pimobileStop();
            return 1;
        }
        timeout++;
        usleep(60000);
        currentAngle = getCompassAngleFromApi();
        if(currentAngle<0) {
            #ifdef DEBUG
            printf("\nerror reading compass api\n");
            #endif
            pimobileStop();
            return 1;
        }
        difference = getAngleDistance(currentAngle, targetAngle);
        differenceAbs = fabsf( difference );
        if(slowedDown == 0 && differenceAbs <= 10 ) {
            // slow down when near target angle
            pimobileSetBothMotorSpeed(35);
            slowedDown = 1;
        }

        #ifdef DEBUG
        printf("\rcurrentAngle=%0.1f difference=%0.1f differenceAbs=%0.1f currentSign=%0.6f ", currentAngle, difference, differenceAbs, difference / differenceAbs);
        fflush(stdout);
        #endif
        // loop until targetAngle reached or passed
    } while ( ( currentAngle != targetAngle ) && ( difference / differenceAbs == differenceSign ) );

    #ifdef DEBUG
    printf("\ntarget angle reached\n");
    #endif

    pimobileStop();
    return 0;
}

void pimobileCalculatePosition(double *Sx, double *Sy, double x1, double y1, double x2, double y2, double oAngle1, double oAngle2, double angleXAxis) {
    double pi = 4. * atan(1.);

    double angle1 = getAngleDistance(angleXAxis, oAngle1);
    double angle2 = getAngleDistance(angleXAxis, oAngle2);

    double a1=tan(pi / 180 * angle1);
    double b1=tan(pi / 180 * angle2);

    // calculate a0 and b0
    double a0 = y1 - a1 * x1;
    double b0 = y2 - b1 * x2;
        
    // calculate intersection
    *Sx = (b0 - a0) / (a1 - b1);
    
    // avoid errors for angles close to 90°
    if(fabs(a1)>fabs(b1)) {
        *Sy = b1 * *Sx + b0;
    } else {
        *Sy = a1 * *Sx + a0;
    } 
        
    #ifdef DEBUG
    printf("angle=%0.1f f(x)=%0.1f * x + %0.1f\n", angle1, a1, a0 );
    printf("angle=%0.1f g(x)=%0.1f * x + %0.1f\n", angle2, b1, b0 );
    printf("intersection: Sx=%0.2f, Sy=%0.2f\n\n", *Sx, *Sy);
    #endif
}    

