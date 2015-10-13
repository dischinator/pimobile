#include <unistd.h>
#include <stdio.h>
#include "pimobile.h"

int main(int argc, char *argv[])
{
    wiringPiSetupGpio();
    pimobileInitializeGpio();

    printf("Turning to 0° N\n");
    pimobileTurnTo(0);
    printf("Moving for 5 seconds\n");
    pimobileSetMotorsSpeedAndDirection(100,100,1,1);
    sleep(5);
    
    printf("Turning to 90°\n");
    pimobileTurnTo(90);
    printf("Moving for 5 seconds\n");
    pimobileSetMotorsSpeedAndDirection(100,100,1,1);
    sleep(5);

    printf("Turning to 180°S\n");
    pimobileTurnTo(180);
    printf("Moving for 5 seconds\n");
    pimobileSetMotorsSpeedAndDirection(100,100,1,1);
    sleep(5);

    printf("Turning to 270°W\n");
    pimobileTurnTo(270);
    printf("Moving for 5 seconds\n");
    pimobileSetMotorsSpeedAndDirection(100,100,1,1);
    sleep(5);

    printf("Turning to 0° N\n");
    pimobileTurnTo(0);
    printf("Moving for 5 seconds\n");
    pimobileSetMotorsSpeedAndDirection(100,100,1,1);
    sleep(5);

    pimobileStop();

    return 0;
}
