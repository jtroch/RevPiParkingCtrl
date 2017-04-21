#include <onposix/AbstractThread.hpp>
//#include "IOHandler.hpp"

using namespace onposix;

class LedAnimation: public AbstractThread { 
    private: 

    public:
        LedAnimation();
        ~LedAnimation();
        void run();
};