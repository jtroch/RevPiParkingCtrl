#include "IOHandler.hpp"
#include <AbstractThread.hpp>

class LedAnimation: public AbstractThread { 
    private: 
        IOHandler IOhandler;
        pthread_t Thread;
        void * ThreadFunction(void* arg);

    public:
        LedAnimation();
        ~LedAnimation();
        void run();
};