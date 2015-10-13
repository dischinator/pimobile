#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <time.h>
#include "eye.hpp"
#include "object.hpp"
#include "match.hpp"

//int minHessian = 400;
static const int minHessian = 700;

cv::SurfFeatureDetector surfDetector(minHessian);
cv::SurfDescriptorExtractor surfExtractor;

int main( int argc, char** argv )
{
    std::srand(time(0));

    pmv::Eye eye;

    // load objects
    for(int i=1; i<argc; i++ ) {
        eye.addObject(new pmv::Object(argv[i]));
    }
    
    for(;;) {
        std::vector<pmv::Match*> matches = eye.look();
        std::cout << "matches.size()=" << matches.size() << "\n";
        for(unsigned int i=0; i<matches.size(); i++) {
            std::cout << "match #" << i << ": name=" << matches[i]->getObject()->getName() << " x=" << matches[i]->getCenterX() << ", y=" << matches[i]->getCenterY() << "\n";
        }
    }

    return 0;
}