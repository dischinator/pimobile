#include <stdio.h>
#include "compass.h"

int main(int argc, char **argv)
{
    float angle, x=0, y=0;

    initializeCompass();
    setCompassModeRegister(COMPASS_MODE_SINGLE);
    getCompassData(&angle, &x, &y);
    
    printf("angle=%0.1f, x=%0.1f, y=%0.1f\n", angle, x, y);
    return 0;
}

