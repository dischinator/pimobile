// Calculate pimobiles position
 
#include <stdio.h>
#include <math.h>
#include "pimobile.h"

int main() {
    double Sx, Sy;
    pimobileCalculatePosition(&Sx, &Sy, 200, 430, 50, 210, 15, 280, 90);
    printf("Schnittpunkt: Sx=%0.2f, Sy=%0.2f\n\n", Sx, Sy);
    return 0;
}
