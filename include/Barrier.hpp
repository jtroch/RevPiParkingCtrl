#include <onposix/AbstractThread.hpp>

#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

#include "RevPiParkingCtrl.hpp"

using namespace onposix;

class Barrier : public AbstractThread { 
    private: 
        GateType barrier;
        void setBarrier(bool on);
        
    public:
        Barrier(GateType type);
        ~Barrier() {};
        void run();
};