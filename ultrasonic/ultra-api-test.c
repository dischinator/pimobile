#include <stdio.h>
#include "../control/pimobile-api-client.h"

int main(int argc, char **argv)
{
    float distance;
    distance = getDistanceFromApi();
    
    printf("distance=%0.1f\n", distance);
    return 0;
}

