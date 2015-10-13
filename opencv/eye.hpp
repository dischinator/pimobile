#ifndef EYE_HPP_INCLUDED
#define EYE_HPP_INCLUDED

#include "object.hpp"
#include "match.hpp"

namespace pmv {
    class Eye {
        private:        
            // scene
            cv::Mat sceneImageMat;
            cv::vector<cv::KeyPoint> sceneKeypoints;
            cv::Mat sceneDescriptors;
            
            // Flann Matcher
            cv::FlannBasedMatcher flannMatcher;
            cv::vector<cv::DMatch> matches;
            static const double minDistMultiplier = 3;
            cv::VideoCapture *cap;
            
            // objects
            std::vector<pmv::Object*> objects;
            unsigned int objectCount;
            
            #ifdef DEBUG
            struct timespec startTime, endTime, lastTime;
            void perform(const char *message);
            #endif
        
        public:        
            Eye();
            void addObject(pmv::Object *object);
            std::vector<pmv::Match*> look();
    };
}

#endif