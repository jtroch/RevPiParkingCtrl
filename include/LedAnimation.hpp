#include <onposix/AbstractThread.hpp>
//#include "IOHandler.hpp"

using namespace onposix;

class LedAnimation: public AbstractThread { 
    private: 
        //IOHandler IOhandler;

    public:
        LedAnimation();
        ~LedAnimation();
        void run();
};