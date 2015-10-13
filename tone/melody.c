#include <unistd.h>
#include <stdio.h>
#include <wiringPi.h>
#include <softTone.h>

const short GPIO_TONE = 21;

#define  c     261
#define  d     294
#define  e     329
#define  f     349
#define  g     392
#define  a     440
#define  b     493
#define  C     523
// Define a special note, 'R', to represent a rest
#define  R     0


int main() {
    
    wiringPiSetupGpio();
    softToneCreate(GPIO_TONE);

    //int melody[] = {  C,  b,  g,  C,  b,   e,  R,  C,  c,  g, a, C };
    int melody[] = {  d,  };
    int beats[]  = { 16, 16, 16,  8,  8,  16, 32, 16, 16, 16, 8, 8 }; 
    int MAX_COUNT = sizeof(melody) / sizeof(melody[0]); // Melody length, for looping.

    for (int i=0; i<MAX_COUNT; i++) {
    printf(" %d",melody[i]);
        fflush(stdout);
        softToneWrite(GPIO_TONE, melody[i]);
        usleep(beats[i]*50000);
    }

    softToneWrite(GPIO_TONE, 0);

    return 0;
}
