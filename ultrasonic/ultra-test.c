#include <stdio.h>
#include "ultra.h"

int main(int argc, char **argv)
{
    double elapsedTime, distance;

    wiringPiSetupGpio();
    initializeUltraSonicModule();
    getUltraSonicData(&elapsedTime, &distance);
    
    printf("elapsedTime=%0.1f, distance=%0.1f\n", elapsedTime, distance);
    return 0;
}

