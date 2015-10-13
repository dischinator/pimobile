#include <fcgi_stdio.h>
#include <stdio.h>
#include "compass.h"

int main(int argc, char **argv)
{
    float angle, x, y;

    initializeCompass();

    while( FCGI_Accept() >= 0 ) {
        printf( "Content-Type: application/json\n\n" );
        
        // switch to single mode
        setCompassModeRegister(COMPASS_MODE_SINGLE);
        
        if (getCompassData(&angle, &x, &y) == 1) {
            printf("{\"error\":\"Unable to access HMC5883L\"}\n");
        } else {
            printf("{\"angle\":%0.1f,\"x\":%0.1f,\"y\":%0.1f}\n", angle, x, y);
        }
    }

    return 0;
}
