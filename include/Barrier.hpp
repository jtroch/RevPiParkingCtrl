#include "IOHandler.hpp"
#include "WorkQueue.h"
#include "onposix.hpp"

enum BarrierType: uint8_t {
    ENTRANCE_BARRIER,
    EXIT_BARRIER
};

class Barrier : public AbstractThread { 
    private: 
        IOHandler IOhandler;
        struct itimerval Timer;
        BarrierType barrier;
        bool OpenBarrier;
        void FireTimer();
        void SetBarrier(BarrierType barrier, int value);
        
    public:
        Barrier(Barriertype type);
        ~Barrier();
        void run();
};