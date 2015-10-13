#include <wiringPi.h>
#include <time.h>

static const int ULTRA_SONIC_TIMEOUT_1 = 1000;    // timeout 1
static const long int ULTRA_SONIC_TIMEOUT_2 = 300000;  // timeout 2

static const char ULTRA_SONIC_GPIO_TRIGGER = 26;    // GPIO connected to trigger pin of HC-SR04
static const char ULTRA_SONIC_GPIO_ECHO = 4;       // GPIO connected to echo pin of HC-SR04

void initializeUltraSonicModule() {
    pinMode(ULTRA_SONIC_GPIO_TRIGGER, OUTPUT);
    pinMode(ULTRA_SONIC_GPIO_ECHO, INPUT);
}

void getUltraSonicData(double *elapsedTime, double *distance) {
    struct timespec startTime, endTime;
    long int timeout1, timeout2;
    
    digitalWrite(ULTRA_SONIC_GPIO_TRIGGER, HIGH);
    delay( 20 );
    // use timeout to avoid infinite loops (for example if no HC-SR04 is connected)
    timeout1 = timeout2 = 0;
    digitalWrite(ULTRA_SONIC_GPIO_TRIGGER, LOW);
    
    do {
        clock_gettime(CLOCK_REALTIME, &startTime);
        timeout1++;
    } while(digitalRead(ULTRA_SONIC_GPIO_ECHO) == 0 && timeout1 < ULTRA_SONIC_TIMEOUT_1);

    while(digitalRead(ULTRA_SONIC_GPIO_ECHO) == 1 && timeout2 < ULTRA_SONIC_TIMEOUT_2) {
        timeout2++;
    }
    clock_gettime(CLOCK_REALTIME, &endTime);
    
    *elapsedTime = (endTime.tv_sec - startTime.tv_sec) * 1000.0;         // sec to ms
    *elapsedTime += (endTime.tv_nsec - startTime.tv_nsec) * 0.000001;    // ns to ms
    
    // multiply with the speed of sound (34.300 cm/ms) and divide by two because the sound travels back and forth
    *distance = (*elapsedTime * 34.300) / 2;
}