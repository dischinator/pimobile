// Turns pimobile to the given angle

#include <unistd.h>
#include <strings.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "pimobile.h"
#include "pimobile-api-client.h"

int main(int argc, char *argv[])
{
    if( argc < 2 ) {
        printf("Usage: %s targetAngle\n", argv[0]);
        return 1;
    }
   
    wiringPiSetupGpio();
    pimobileInitializeGpio();
    
    float targetAngle = strtof(argv[1], (char **)NULL);
    pimobileTurnTo(targetAngle);
    
    printf("target angle=%0.1f actual reached angle=%0.1f\n", targetAngle, getCompassAngleFromApi());
    
    return 0;
}
