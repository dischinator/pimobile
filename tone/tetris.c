#include <unistd.h>
#include <stdio.h>
#include <wiringPi.h>
#include <softTone.h>

const short GPIO_TONE = 21;

const int C = 261;
const int Cis = 277;
const int D = 293;
const int Dis = 311;
const int E = 329;
const int F = 349;
const int Fis = 369;
const int G = 391;
const int Gis = 415;
const int A = 440;
const int Ais = 466;
const int H = 493;
const int Takt = 1700;

#define TAKT_FAKTOR 800

void Beep( int tone, int duration ) {
    printf(" %d",tone);
    fflush(stdout);
    softToneWrite(GPIO_TONE, tone);
    usleep(duration * TAKT_FAKTOR);
    softToneWrite(GPIO_TONE, 0);
    usleep(TAKT_FAKTOR);
}

void hymne() {
    Beep(704,750); 
    Beep(792,250); 
    Beep(880,500); 
    Beep(792,500); 
    Beep(940,500); 
    Beep(880,500); 
    Beep(792,250); 
    Beep(660,250); 
    Beep(704,500); 
    Beep(1188,500); 
    Beep(1056,500); 
    Beep(940,500); 
    Beep(880,500); 
    Beep(792,500); 
    Beep(880,250); 
    Beep(704,250); 
    Beep(1056,1000); 
    Beep(704,750); 
    Beep(792,250); 
    Beep(880,500); 
    Beep(792,500); 
    Beep(940,500); 
    Beep(880,500); 
    Beep(792,250); 
    Beep(660,250); 
    Beep(704,500); 
    Beep(1188,500); 
    Beep(1056,500); 
    Beep(940,500); 
    Beep(880,500); 
    Beep(792,500); 
    Beep(880,250); 
    Beep(704,250); 
    Beep(1056,1000); 
    Beep(792,500); 
    Beep(880,500); 
    Beep(792,250); 
    Beep(660,250); 
    Beep(528,500); 
    Beep(940,500); 
    Beep(880,500); 
    Beep(792,250); 
    Beep(660,250); 
    Beep(528,500); 
    Beep(1056,500); 
    Beep(940,500); 
    Beep(880,750); 
    Beep(880,250); 
    Beep(990,500); 
    Beep(940,250); 
    Beep(1056,250); 
    Beep(1056,1000); 
    Beep(1408,750); 
    Beep(1320,250); 
    Beep(1320,250); 
    Beep(1188,250); 
    Beep(1056,500); 
    Beep(1188,750); 
    Beep(1056,250); 
    Beep(1056,250); 
    Beep(940,250); 
    Beep(880,500); 
    Beep(792,750); 
    Beep(880,125); 
    Beep(940,125); 
    Beep(1056,250); 
    Beep(1188,250); 
    Beep(940,250); 
    Beep(792,250); 
    Beep(704,500); 
    Beep(880,250); 
    Beep(792,250); 
    Beep(704,1000); 
    Beep(1408,750); 
    Beep(1320,250); 
    Beep(1320,250); 
    Beep(1188,250); 
    Beep(1056,500); 
    Beep(1188,750); 
    Beep(1056,250); 
    Beep(1056,250); 
    Beep(940,250); 
    Beep(880,500); 
    Beep(792,750); 
    Beep(880,125); 
    Beep(940,125); 
    Beep(1056,250); 
    Beep(1188,250); 
    Beep(940,250); 
    Beep(792,250); 
    Beep(704,500); 
    Beep(880,250); 
    Beep(792,250); 
    Beep(704,1000);  
}

void nokia() {
    Beep(676,460);
    Beep(804,340);
    Beep(676,230);
    Beep(676,110);
    Beep(902,230);
    Beep(676,230);
    Beep(602,230);
    Beep(676,460);
    Beep(1012,340);
    Beep(676,230);
    Beep(676,110);
    Beep(1071,230);
    Beep(1012,230);
    Beep(804,230);
    Beep(676,230);
    Beep(1012,230);
    Beep(1351,230);
    Beep(676,110);
    Beep(602,230);
    Beep(602,110);
    Beep(506,230);
    Beep(758,230);
    Beep(676,460);
    Beep(676,460); 
}

void tetris() {
    Beep(E * 2, Takt / 4);
    Beep(H * 1, Takt / 8);
    Beep(C * 2, Takt / 8);
    Beep(D * 2, Takt / 4);
    Beep(C * 2, Takt / 8);
    Beep(H * 1, Takt / 8);
    Beep(A * 1, Takt / 4);
    Beep(A * 1, Takt / 8);
    Beep(C * 2, Takt / 8);
    Beep(E * 2, Takt / 8);
    Beep(E * 2, Takt / 8);
    Beep(D * 2, Takt / 8);
    Beep(C * 2, Takt / 8);
    Beep(H * 1, Takt / 2);
    Beep(C * 2, Takt / 8);
    Beep(D * 2, Takt / 4);
    Beep(E * 2, Takt / 4);
    Beep(C * 2, Takt / 4);
    Beep(A * 1, Takt / 4);
    Beep(A * 1, Takt / 4);
    usleep(Takt / (8 / 3) * TAKT_FAKTOR);
    Beep(D * 2, Takt / 3);
    Beep(F * 2, Takt / 8);
    Beep(A * 2, Takt / 8);
    Beep(A * 2, Takt / 8);
    Beep(G * 2, Takt / 8);
    Beep(F * 2, Takt / 8);
    Beep(E * 2, Takt / 3);
    Beep(C * 2, Takt / 8);
    Beep(E * 2, Takt / 8);
    Beep(E * 2, Takt / 8);
    Beep(D * 2, Takt / 8);
    Beep(C * 2, Takt / 8);
    Beep(H * 1, Takt / 4);
    Beep(H * 1, Takt / 8);
    Beep(C * 2, Takt / 8);
    Beep(D * 2, Takt / 4);
    Beep(E * 2, Takt / 4);
    Beep(C * 2, Takt / 4);
    Beep(A * 1, Takt / 4);
    Beep(A * 1, Takt / 4);
}

int main() {
    wiringPiSetupGpio();
    softToneCreate(GPIO_TONE);

    while (1) {
        tetris();
        usleep(Takt / 4 * TAKT_FAKTOR);
    }

    usleep(100000);
    softToneWrite(GPIO_TONE, 0);
    usleep(100000);
    softToneWrite(GPIO_TONE, 0);
    return 0;
}
