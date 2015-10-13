#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <wiringPi.h>
#include <softTone.h>

static const short GPIO_TONE = 21;

void INThandler(int sig)
{
     signal(sig, SIG_IGN);
     usleep(50000);
     softToneWrite(GPIO_TONE, 0);
     usleep(50000);
     softToneWrite(GPIO_TONE, 0);
     exit(0);
}

int main() {
    wiringPiSetupGpio();
    softToneCreate(GPIO_TONE);
    signal(SIGINT, INThandler);

    while(1) {
        for(int tone=500; tone<=2000; tone+=10 ) {
            printf("You are listening to %dhz\n", tone);
            softToneWrite(GPIO_TONE, tone);
            usleep(5000);
        }
        for(int tone=2000; tone>500; tone-=10 ) {
            printf("You are listening to %dhz\n", tone);
            softToneWrite(GPIO_TONE, tone);
            usleep(5000);
        }
    }

    softToneWrite(GPIO_TONE, 0);
    return 0;
}
