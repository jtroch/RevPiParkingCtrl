#include <onposix/AbstractThread.hpp>

#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

#include "RevPiParkingCtrl.hpp"

using namespace onposix;

class Barrier : public AbstractThread { 
    private: 
        struct itimerval Timer;
        GateType barrier;
        bool OpenBarrier;
        void fireTimer();
        void setBarrier(bool on);
        void timerCallback(int signum);
        
    public:
        Barrier(GateType type);
        ~Barrier() {};
        void run();
};