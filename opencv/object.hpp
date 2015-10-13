#ifndef OBJECT_HPP_INCLUDED
#define OBJECT_HPP_INCLUDED

#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>

extern cv::SurfFeatureDetector surfDetector;
extern cv::SurfDescriptorExtractor surfExtractor;

namespace pmv {
    class Object {
        private:
            cv::Mat imageMat;
            cv::vector<cv::KeyPoint> keypoints;
            cv::Mat descriptors;
            cv::Scalar color;
            std::string name;
            
        public:
            Object(const char *fileName);
            cv::Mat getImageMat();
            cv::vector<cv::KeyPoint> getKeypoints();
            cv::Mat getDescriptors();
            cv::Scalar getColor();
            std::string getName();
    };
}

#endif