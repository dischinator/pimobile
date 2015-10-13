#include <fcgi_stdio.h>
#include <wiringPi.h>
#include <stdio.h>

#include <math.h>
#include <unistd.h>
#include <fcntl.h>

#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "pimobile.h"
#include "pimobile-api-client.h"

int lastDirectionLeftMotor, lastDirectionRightMotor, lastSpeedLeftMotor, lastSpeedRightMotor;
volatile char turnInProgress = 0;
pthread_t turnThread;

void processSpeedUpdate() {
    // Extract path parameters
    char *uri = getenv ("DOCUMENT_URI");
    char *copyUri = strcpy( alloca(strlen(uri)+1), uri );
    char *speedVectorString, *directionVectorString;
    long int speedVector, directionVector;
    int speedLeftMotor, speedRightMotor, speed, directionFactor, directionLeftMotor, directionRightMotor, speedOppositeMotor;

    char *ptr = strtok(copyUri, "/");
    ptr = strtok(NULL, "/"); // discard api path ("speed")
    ptr = strtok(NULL, "/"); // speed vector
    if( ptr == NULL) {
        printf ("{\"error\":\"Speed vector must be given.\"}\n");
        return;
    }
    if( strlen(ptr)+1 > 10 ) {
        // bail out to avoid stack overflow on invalid input
        printf ("{\"error\":\"Speed parameter is invalid.\"}\n");
        return;
    }
    speedVectorString = strcpy( alloca(strlen(ptr)+1), ptr );
    ptr = strtok(NULL, "/"); // direction vector
    if( ptr == NULL) {
        printf ("{\"error\":\"Direction vector must be given.\"}\n");
        return;
    }
    if( strlen(ptr)+1 > 10 ) {
        printf ("{\"error\":\"Direction parameter is invalid.\"}\n");
        return;
    }
    directionVectorString = strcpy( alloca(strlen(ptr)+1), ptr );
    
    speedVector = strtol(speedVectorString, (char **)NULL, 10);
    directionVector = strtol(directionVectorString, (char **)NULL, 10);
    
    // now translate speed and direction vectors into motor commands
    speedLeftMotor = speedRightMotor = speed = abs( speedVector );
    
    directionFactor = abs( directionVector );
    directionLeftMotor = directionRightMotor = fminl( 1, fmaxl(0, speedVector + 1) );
    
    if( speed < 5 && directionFactor > 5 ) {
        // special case turn without moving
        
        // motors always move opposite directions to turn on spot
        if( directionVector > 0 ) {
            // turn right
            directionRightMotor = 0;
            directionLeftMotor = 1;
        } else {
            // turn left
            directionRightMotor = 1;
            directionLeftMotor = 0;
        }
        
        // speed of both motors always is equal and matches directionFactor
        speedLeftMotor = speedRightMotor = directionFactor;        
    } else if( directionFactor > 5 ) {
        // now calculate motor speeds for left and right motor based on speed and direction to enable turning
        // ignore values below 5
        if( directionFactor < 50 ) {
            // values below 50 gradually reduce speed of the opposite motor
            speedOppositeMotor = (int)floor( speed - directionFactor / 50.0 * speed );
        } else {
            // values above 50 invert motor direction on opposite motor with increasing speeds
            speedOppositeMotor = (int)floor( (directionFactor - 50) / 50.0 * speed );

            // now invert motor direction
            if( directionVector > 0 ) {
                // turn right
                directionRightMotor = directionRightMotor ^ 1;
            } else {
                directionLeftMotor = directionLeftMotor ^ 1;
            }
        }

        if( directionVector > 0 ) {
            // turn right
            speedRightMotor = speedOppositeMotor;
        } else {
            // turn left
            speedLeftMotor = speedOppositeMotor;
        }
    }
    
    // Set motor GPIO's, only update GPIO values if changed
    
    // change left motor direction
    if(lastDirectionLeftMotor != directionLeftMotor) {
        #ifdef DEBUG
        printf("changing left motor direction\n");
        #endif
        pimobileSetLeftMotorDirection(directionLeftMotor);
        lastDirectionLeftMotor = directionLeftMotor;
    }
    // change left motor speed
    if(lastSpeedLeftMotor != speedLeftMotor) {
        #ifdef DEBUG
        printf("changing left motor speed\n");
        #endif
        pimobileSetLeftMotorSpeed(speedLeftMotor);
        lastSpeedLeftMotor = speedLeftMotor;
    }

    // change right motor direction
    if(lastDirectionRightMotor != directionRightMotor) {
        #ifdef DEBUG
        printf("changing right motor direction\n");
        #endif
        pimobileSetRightMotorDirection(directionRightMotor);
        lastDirectionRightMotor = directionRightMotor;
    }
    // change right motor speed
    if(lastSpeedRightMotor != speedRightMotor) {
        #ifdef DEBUG
        printf("changing right motor speed\n");
        #endif
        pimobileSetRightMotorSpeed(speedRightMotor);
        lastSpeedRightMotor = speedRightMotor;
    }

    #ifdef DEBUG
    printf("Input: %s (%d) Direction: %s (%d)\n", speedVectorString, speedVector, directionVectorString, directionVector );
    printf("Speed: left=%d, right=%d\n", speedLeftMotor, speedRightMotor );
    printf("Direc: left=%d, right=%d\n", directionLeftMotor, directionRightMotor );
    #endif
}

void *turnTo_function( void *targetAnglePtr ) {
    // avoid proceccing multipe turn requests parallel
    turnInProgress = 1;
    double *targetAngle = (double *) targetAnglePtr;
    pimobileTurnTo( *targetAngle );
    // re-initialize last values to force setting on next manual control request
    lastSpeedLeftMotor = lastSpeedRightMotor = 200;
    lastDirectionLeftMotor = lastDirectionRightMotor = 10; // invalid state (gpio currently unset)
    turnInProgress = 0;
    return NULL;
}

void turnTo() {
    if( turnInProgress == 1 ) {
        printf ("{\"error\":\"Turn in progress.\"}\n");
        return;
    }
    
    // Extract path parameters
    char *uri = getenv ("DOCUMENT_URI");
    char *copyUri = strcpy( alloca(strlen(uri)+1), uri );
    char *angleString;
    double angle;

    char *ptr = strtok(copyUri, "/");
    ptr = strtok(NULL, "/"); // discard api path ("turnTo")
    ptr = strtok(NULL, "/"); // angle
    if( ptr == NULL) {
        printf ("{\"error\":\"Angle must be given.\"}\n");
        return;
    }
    if( strlen(ptr)+1 > 10 ) {
        // bail out to avoid stack overflow on invalid input
        printf ("{\"error\":\"Angle parameter is invalid.\"}\n");
        return;
    }
    angleString = strcpy( alloca(strlen(ptr)+1), ptr );
    angle = strtod(angleString, (char **)NULL);

    // Relative or absolute?
    ptr = strtok(NULL, "/"); // relative / absolute
    if( ptr != NULL && strstr(ptr, "relative") != NULL) {
    // relative. get current angle and calculate relative target angle
        
        // get current angle
        float currentAngle = getCompassAngleFromApi();
        if(currentAngle<0) {
            return;
        }

        angle += currentAngle;
    }

    // handle values greater 360
    angle = fmod(angle, 360.0);

    #ifdef DEBUG
    printf("angle=%0.2f\n", angle);
    #endif

    //pthread_cancel(turnThread);
    pthread_create(&turnThread, NULL, turnTo_function, (void *)&angle);
}

void setFrontLightStatus() {
    // Extract path parameters
    char *uri = getenv ("DOCUMENT_URI");
    char *copyUri = strcpy( alloca(strlen(uri)+1), uri );
    char *statusString;
    char status;

    char *ptr = strtok(copyUri, "/");
    ptr = strtok(NULL, "/"); // discard api path
    ptr = strtok(NULL, "/"); // light status
    if( ptr == NULL) {
        // if status is not given, toggle current status
        status = pimobileGetFrontLightStatus() ^ 1;
    } else if( strlen(ptr)+1 > 2 ) {
        // bail out to avoid stack overflow on invalid input
        printf ("{\"error\":\"Status parameter is invalid.\"}\n");
        return;
    } else {
        statusString = strcpy( alloca(strlen(ptr)+1), ptr );
        status = strtol(statusString, (char **)NULL, 10) % 2;
    }

    #ifdef DEBUG
    printf("status=%d\n", status);
    #endif
    
    pimobileSetFrontLight(status);
}

int main (int argc, char **argv)
{
  char *uri;

  // initialize motor control gpio's
  wiringPiSetupGpio();
  pimobileInitializeGpio();
  
  // initialize last values
  lastSpeedLeftMotor = lastSpeedRightMotor = 200;
  lastDirectionLeftMotor = lastDirectionRightMotor = 10; // invalid state (gpio currently unset)

  while (FCGI_Accept () >= 0)
    {
      printf ("Content-Type: application/json\n\n");

      uri = getenv ("DOCUMENT_URI");
      // check api path
      if (strstr(uri, "/turnto") != NULL)
        {
            turnTo();
        }
      else if (strstr(uri, "/speed") != NULL)
        {
            processSpeedUpdate();
        }
      else if (strstr(uri, "/frontlight") != NULL)
        {
            setFrontLightStatus();
        }
      else
        {
            printf ("{\"error\":\"Unknown api path.\"}\n");
        }
    }
  return 0;
}
