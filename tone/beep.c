#include <unistd.h>
#include <wiringPi.h>
#include <softTone.h>

const short GPIO_TONE = 21;

int main() {
    wiringPiSetupGpio();
    softToneCreate(GPIO_TONE);

    softToneWrite(GPIO_TONE, 2000);
    usleep(20000);
    softToneWrite(GPIO_TONE, 0);
    
    return 0;
}