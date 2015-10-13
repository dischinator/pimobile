#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <time.h>
#include <stdio.h>
#include <unistd.h>

#include "eye.hpp"
#include "object.hpp"
#include "match.hpp"

#include "../control/pimobile.h"
#include "../control/pimobile-api-client.h"
#include <softTone.h>

//int minHessian = 400;
static const int minHessian = 700;

static const unsigned int imageColumns = 640;
static const float cameraAngle = 62.0;  // wide angle
//static const float cameraAngle = 28.0;  // zoom
static const float pimobileSpeed = 7.24;  // cm/s

cv::SurfFeatureDetector surfDetector(minHessian);
cv::SurfDescriptorExtractor surfExtractor;

int main( int argc, char** argv )
{
    wiringPiSetupGpio();
    pimobileInitializeGpio();
    
    std::srand(time(0));

    pmv::Eye eye;

    // load objects
    for(int i=1; i<argc; i++ ) {
        eye.addObject(new pmv::Object(argv[i]));
    }
    
    for(;;) {
        float angle = getCompassAngleFromApi();
        float turnValue = 0.0;
        if(angle<0) {
            std::cout << "compass error\n";
            return 1;
        }
        
        std::vector<pmv::Match*> matches = eye.look();
        std::cout << "matches.size()=" << matches.size() << "\n";
        
        if(matches.size()>0) {
            if(matches[0]->getCenterX() < 0 ) {
                // special cases object center is out of view (left side)
                std::cout << "object center is out of view (left side)\n";
                // calculate angle between left view limit and object center
                turnValue = -1.0 * (fabsf(matches[0]->getCenterX()) / (imageColumns - 1.0) * cameraAngle + cameraAngle / 2.0);
            } else if(matches[0]->getCenterX() >= imageColumns) {
                // special cases object center is out of view (right side)
                std::cout << "object center is out of view (right side)\n";
                // calculate angle between right view limit and object center
                turnValue = (matches[0]->getCenterX() - imageColumns) / (imageColumns - 1.0) * cameraAngle + cameraAngle / 2.0;
            } else {
                // object center is in view, calculate angle between object center and field of view center
                turnValue = (matches[0]->getCenterX() - imageColumns / 2.0 ) / (imageColumns - 1.0) * cameraAngle;
            }
            
            std::cout << "found something at " << angle << "° \n";
            std::cout << "now trying to center on it.\n";
            std::cout << "turning " << turnValue << "°\n";
        } else {
            // nothing found -> turn pimobile and search again            
            turnValue = cameraAngle * 0.8;
            std::cout << "object not found turning " << turnValue << "°\n";
        }

        // calculate next step and handle values greater 360
        angle = fmod(angle + turnValue, 360.0);
        pimobileTurnTo(angle);
        
        if(matches.size()>0) {
            float distance = getDistanceFromApi();
            std::cout << "object " << matches[0]->getObject()->getName() << " found at " << angle << "° distance is " << distance << " cm\n";

            pimobileSetMotorsSpeedAndDirection(100,100,1,1);
            sleep((int)floor(distance/(2*pimobileSpeed)));

            distance = getDistanceFromApi();
            if(distance<30.0) {
                pimobileStop();
                std::cout << "Target reached.\n";
                
                softToneCreate(GPIO_TONE);
                pimobileSetFrontLight(1);
                softToneWrite(GPIO_TONE, 1000);
                usleep(2000);
                softToneWrite(GPIO_TONE, 0);
                pimobileSetFrontLight(0);
                usleep(200000);
                pimobileSetFrontLight(1);
                softToneWrite(GPIO_TONE, 1000);
                usleep(2000);
                softToneWrite(GPIO_TONE, 0);
                pimobileSetFrontLight(0);
                usleep(200000);
                pimobileSetFrontLight(1);
                softToneWrite(GPIO_TONE, 1000);
                usleep(2000);
                softToneWrite(GPIO_TONE, 0);
                pimobileSetFrontLight(0);
                softToneWrite(GPIO_TONE, 1000);
                usleep(400000);
                softToneWrite(GPIO_TONE, 0);
                return 0;
             }
        }
    }

    return 0;
}
