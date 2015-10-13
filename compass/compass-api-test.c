#include <stdio.h>
#include "../control/pimobile-api-client.h"

int main(int argc, char **argv)
{
    float angle;
    angle = getCompassAngleFromApi();
    
    printf("angle=%0.1f\n", angle);
    return 0;
}

