#include <wiringPi.h>
#include <softTone.h>

#define GPIO_TONE 21

int main() {
    wiringPiSetupGpio();
    softToneCreate(GPIO_TONE);
    softToneWrite(GPIO_TONE, 0);
}
