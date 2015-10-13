#ifndef MATCH_HPP_INCLUDED
#define MATCH_HPP_INCLUDED

#include "object.hpp"

namespace pmv {
    class Match {
        private:
            pmv::Object* object;
            double centerX;
            double centerY;            
            
        public:        
            Match(pmv::Object* matchedObject, double x, double y);
            pmv::Object* getObject();
            double getCenterX();
            double getCenterY();
    };
}

#endif