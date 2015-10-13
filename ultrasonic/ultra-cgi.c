#include <fcgi_stdio.h>
#include <stdio.h>
#include "ultra.h"

const double MAX_DURATION = 30;  // Maximum runtime. Above it we assume an error happened.

int main(void)
{
    double elapsedTime, distance;
    double lastDistance = -1;

    wiringPiSetupGpio();
    initializeUltraSonicModule();

    while(FCGI_Accept() >= 0) {
        printf( "Content-Type: application/json\n\n" );
        getUltraSonicData(&elapsedTime, &distance);

        if(elapsedTime < MAX_DURATION) {
            // compensate measurement error (result 0)
            if (distance < 0.1 && lastDistance != -1) {
                distance = lastDistance;
            } else {
                lastDistance = distance;
            }

            printf("{\"time\":%0.3f,\"distance\":%0.1f}\n", elapsedTime, distance);
        } else {
            printf("{\"error\":true,\"time\":%0.3f}\n", elapsedTime);
        }
    }
    return 0;
}
