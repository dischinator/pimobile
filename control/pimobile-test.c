#include <unistd.h>
#include <stdio.h>
#include "pimobile.h"

int main(int argc, char **argv)
{
    wiringPiSetupGpio();
    pimobileInitializeGpio();
    
    printf("Testing forward movement at full speed\n");
    pimobileSetMotorsSpeedAndDirection(100,100,1,1);
    sleep(1);

    printf("Testing forward movement at 40%% speed\n");
    pimobileSetMotorsSpeedAndDirection(40,40,1,1);
    sleep(1);

    printf("Testing backward movement at full speed\n");
    pimobileSetMotorsSpeedAndDirection(100,100,0,0);
    sleep(1);

    printf("Testing backward movement at 40%% speed\n");
    pimobileSetMotorsSpeedAndDirection(40,40,0,0);
    sleep(1);

    printf("Testing turning and compass. Turning to 0° North\n");
    pimobileTurnTo(0);
    sleep(1);

    printf("Testing turning and compass. Turning to 180° South\n");
    pimobileTurnTo(180);
    sleep(1);

    return 0;
}
