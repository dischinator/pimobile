#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include "eye.hpp"

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#ifdef DEBUG
void pmv::Eye::perform(const char *message) {
    double elapsedTime, totalTime;
    clock_gettime(CLOCK_REALTIME, &endTime);
    elapsedTime = endTime.tv_sec - lastTime.tv_sec;         // sec to ms
    elapsedTime += (endTime.tv_nsec - lastTime.tv_nsec) * 0.000000001;    // ns to s
    totalTime = endTime.tv_sec - startTime.tv_sec;         // sec to ms
    totalTime += (endTime.tv_nsec - startTime.tv_nsec) * 0.000000001;    // ns to s
    std::cout << "time for " << message << " " << elapsedTime << " - total time " << totalTime << "\n";
    lastTime = endTime;
}
#endif

pmv::Eye::Eye() {
    // open the default camera
    cap = new cv::VideoCapture(0);
    // check if we succeeded
    if(!cap->isOpened())
    {
        std::cout << "Capture device could not be opened\n";
    }
    cap->set(CV_CAP_PROP_FRAME_WIDTH, 640);
    cap->set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    objectCount = 0;
    
    #ifdef DEBUG
    clock_gettime(CLOCK_REALTIME, &startTime);
    lastTime = startTime;
    #endif                
}

void pmv::Eye::addObject(pmv::Object *object) {
    objects.push_back(object);
    objectCount++;
}

std::vector<pmv::Match*> pmv::Eye::look() {
    // drop old frames
    for(int i=0; i<15; i++) {
        cap->grab();
    }
    
    #ifdef DEBUG
    clock_gettime(CLOCK_REALTIME, &startTime);
    lastTime = startTime;
    #endif
    
    std::vector<pmv::Match*> resultMatches;

    cap->read(sceneImageMat); // get a new frame from camera    
    #ifdef DEBUG
    perform("frame capture");
    #endif
    cv::imwrite("../frontend/camera/scene.jpg", sceneImageMat);

    surfDetector.detect(sceneImageMat, sceneKeypoints);
    surfExtractor.compute(sceneImageMat, sceneKeypoints, sceneDescriptors);
    std::cout << "scene analyzed. sceneKeypoints=" << sceneKeypoints.size() << "\n";
    
    if(sceneDescriptors.empty()) {
        std::cout << KRED "no descriptors found for scene\n" KNRM;
        return resultMatches;
    }        
    
    #ifdef DEBUG
    perform("surf scene");
    #endif

    // try to match each loaded object
    for(unsigned int objectId=0; objectId<objectCount; objectId++) {
        double max_dist = 0;
        double min_dist = 100;
        
        std::cout << KMAG "Processing image #" << objectId << " keypoints=" << objects[objectId]->getKeypoints().size() << "\n" KNRM;
        
        flannMatcher.match(objects[objectId]->getDescriptors(), sceneDescriptors, matches);
        #ifdef DEBUG
        perform("flannMatcher");            
        #endif

        for( int i = 0; i < objects[objectId]->getDescriptors().rows; i++ )
        {
            double dist = matches[i].distance;
            if( dist < min_dist ) min_dist = dist;
            if( dist > max_dist ) max_dist = dist;
        }

        cv::vector<cv::DMatch> goodMatches;
        for( int i = 0; i < objects[objectId]->getDescriptors().rows; i++ )
        {
            if( matches[i].distance < minDistMultiplier*min_dist )
            {
                goodMatches.push_back(matches[i]);
            }
        }
        #ifdef DEBUG
        perform("evaluate good matches");
        #endif

        std::cout << KYEL "Good matches found: " << goodMatches.size() << "\n" KNRM;

        cv::Mat imageMatches;
        cv::drawMatches(objects[objectId]->getImageMat(), objects[objectId]->getKeypoints(), sceneImageMat, sceneKeypoints, goodMatches, imageMatches, cv::Scalar::all(-1), cv::Scalar::all(-1), cv::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
        #ifdef DEBUG
        perform("draw good matches");
        #endif

        cv::vector<cv::Point2f> obj, scn;
        for( std::size_t i = 0; i < goodMatches.size(); i++ )
        {
            //-- Get the keypoints from the good matches
            obj.push_back( objects[objectId]->getKeypoints()[ goodMatches[i].queryIdx ].pt );
            scn.push_back( sceneKeypoints[ goodMatches[i].trainIdx ].pt );
        }
        
        if(goodMatches.size() < 4) {
            std::cout << KRED "Not enough good matches for object #" << objectId << "\n" KNRM;
            continue;
        }

        cv::vector<uchar> outputMask;
        cv::Mat homography = cv::findHomography(obj, scn, CV_RANSAC, 3, outputMask);
        #ifdef DEBUG
        perform("findHomography");
        #endif

        int inlierCounter = 0;
        for (std::size_t i = 0; i < outputMask.size(); i++) {
            if (outputMask[i] == 1) {
                inlierCounter++;
            }
        }

        int inlierPercentage = (int)(((float)inlierCounter / (float)outputMask.size()) * 100);
        std::cout << KYEL "Inliers percentage: " << inlierPercentage << " Inliers counter: " << inlierCounter << "\n" KNRM;
        
        if( inlierPercentage < 30 || inlierCounter < 6 ) {
            std::cout << KRED "Object #" << objectId << " not found\n" KNRM;
        } else {
            std::cout << KGRN "Object #" << objectId << " found\n" KNRM;
            cv::vector<cv::Point2f> objCorners(4);
            objCorners[0] = cv::Point(0,0);
            objCorners[1] = cv::Point( objects[objectId]->getImageMat().cols, 0 );
            objCorners[2] = cv::Point( objects[objectId]->getImageMat().cols, objects[objectId]->getImageMat().rows );
            objCorners[3] = cv::Point( 0, objects[objectId]->getImageMat().rows );

            cv::vector<cv::Point2f> scnCorners(4);

            cv::perspectiveTransform(objCorners, scnCorners, homography);
            
            // calculate center point
            double minimumX = fmin(fmin(scnCorners[0].x, scnCorners[1].x), fmin(scnCorners[2].x, scnCorners[3].x));
            double maximumX = fmax(fmax(scnCorners[0].x, scnCorners[1].x), fmax(scnCorners[2].x, scnCorners[3].x));
            double minimumY = fmin(fmin(scnCorners[0].y, scnCorners[1].y), fmin(scnCorners[2].y, scnCorners[3].y));
            double maximumY = fmax(fmax(scnCorners[0].y, scnCorners[1].y), fmax(scnCorners[2].y, scnCorners[3].y));
            double centerX = minimumX + (maximumX - minimumX) / 2;
            double centerY = minimumY + (maximumY - minimumY) / 2;

            std::cout << KYEL "Object #" << objectId << " center x=" << centerX << " y=" << centerY << "\n" KNRM;
            
            // draw cirle in object center
            cv::circle( imageMatches, cv::Point2f( centerX, centerY) + cv::Point2f( objects[objectId]->getImageMat().cols, 0), 10, cv::Scalar(0, 255, 0), 2, CV_AA);
            // draw object outline
            cv::line( imageMatches, scnCorners[0] + cv::Point2f( objects[objectId]->getImageMat().cols, 0), scnCorners[1] + cv::Point2f( objects[objectId]->getImageMat().cols, 0), cv::Scalar(0, 255, 0), 2, CV_AA);
            cv::line( imageMatches, scnCorners[1] + cv::Point2f( objects[objectId]->getImageMat().cols, 0), scnCorners[2] + cv::Point2f( objects[objectId]->getImageMat().cols, 0), cv::Scalar(0, 255, 0), 2, CV_AA);
            cv::line( imageMatches, scnCorners[2] + cv::Point2f( objects[objectId]->getImageMat().cols, 0), scnCorners[3] + cv::Point2f( objects[objectId]->getImageMat().cols, 0), cv::Scalar(0, 255, 0), 2, CV_AA);
            cv::line( imageMatches, scnCorners[3] + cv::Point2f( objects[objectId]->getImageMat().cols, 0), scnCorners[0] + cv::Point2f( objects[objectId]->getImageMat().cols, 0), cv::Scalar(0, 255, 0), 2, CV_AA);
            
            // also draw in scene
            // draw cirle in object center
            cv::circle( sceneImageMat, cv::Point2f( centerX, centerY), 6, objects[objectId]->getColor(), 2, CV_AA);
            // draw object outline
            cv::line( sceneImageMat, scnCorners[0], scnCorners[1], objects[objectId]->getColor(), 2, CV_AA);
            cv::line( sceneImageMat, scnCorners[1], scnCorners[2], objects[objectId]->getColor(), 2, CV_AA);
            cv::line( sceneImageMat, scnCorners[2], scnCorners[3], objects[objectId]->getColor(), 2, CV_AA);
            cv::line( sceneImageMat, scnCorners[3], scnCorners[0], objects[objectId]->getColor(), 2, CV_AA);
            
            // create match object
            resultMatches.push_back(new pmv::Match(objects[objectId], centerX, centerY));
        }

        char outputFilename[255];
        sprintf(outputFilename, "../frontend/camera/output_%d.jpg", objectId);
        cv::imwrite(outputFilename, imageMatches);
        
        #ifdef DEBUG
        perform("image written");
        #endif
    }
    
    // write image showing all found objects
    cv::imwrite("../frontend/camera/output.jpg", sceneImageMat);
    
    #ifdef DEBUG
    perform("frame finished");
    #endif
    std::cout << "\n";
    
    // TODO return matches (id and center of object relative to scene) (as object?)
    return resultMatches;
}
