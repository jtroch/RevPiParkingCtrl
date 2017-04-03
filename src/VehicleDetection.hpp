#include "IOHandler.hpp"
#include <AbstractThread.hpp>
#include <PosixSharedQueue.hpp>

class VehicleDetection : public AbstractThread { 
    private: 
        IOHandler IOhandler;
        PosixSharedQueue<HttpMsgType>& HttpWorkQueue;
        struct itimerval EntranceTimer;
        struct itimerval ExitTimer;
        void FireEntranceTimer();
        void FireExitTimer();

    public:
        VehicleDetection(PosixSharedQueue<HttpMsgType>& queue);
        ~VehicleDetection();
        void run();
};