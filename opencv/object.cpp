#include "object.hpp"

pmv::Object::Object(const char *fileName) {
    name.assign(fileName);
    
    // load object image file
    imageMat = cv::imread(fileName, 0);
    
    // execute surf
    surfDetector.detect(imageMat, keypoints);
    surfExtractor.compute(imageMat, keypoints, descriptors);
    
    // define random color
    color = cv::Scalar(floor(std::rand() * 255.0 / RAND_MAX),floor(std::rand() * 255.0 / RAND_MAX),floor(std::rand() * 255.0 / RAND_MAX));
}

cv::Mat pmv::Object::getImageMat() {
    return imageMat;
}

cv::vector<cv::KeyPoint> pmv::Object::getKeypoints() {
    return keypoints;
}

cv::Mat pmv::Object::getDescriptors() {
    return descriptors;
}

cv::Scalar pmv::Object::getColor() {
    return color;
}

std::string pmv::Object::getName() {
    return name;
}
