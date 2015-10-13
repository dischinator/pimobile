// Measure the time pimobile needs for a full 360°turn

#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include "pimobile.h"

struct timespec startTime, endTime;

double printDuration(char print) {
    double elapsedTime;
    clock_gettime(CLOCK_REALTIME, &endTime);
    elapsedTime = endTime.tv_sec - startTime.tv_sec;         // sec to ms
    elapsedTime += (endTime.tv_nsec - startTime.tv_nsec) * 0.000000001;    // ns to s
    if( print ) {
        printf("\nTotal time %0.1f\n", elapsedTime);
    }
    return elapsedTime;
}

void gotoAngle( float angle ) {
    printf("\nTurning to %0.1f degrees\n", angle);
    pimobileTurnTo(angle);
    printDuration(1);
}

int main(int argc, char *argv[])
{
    wiringPiSetupGpio();
    pimobileInitializeGpio();

    printf("Turning to 0° N\n");
    pimobileTurnTo(0);
    printf("\nStarting timer\n");
    clock_gettime(CLOCK_REALTIME, &startTime);

    gotoAngle(90);
    gotoAngle(180);
    gotoAngle(270);
    gotoAngle(359.99);
    
    printf("\nTurnspeed is %0.2f°/s\n", 360.0 / printDuration(0) );

    pimobileStop();

    return 0;
}
