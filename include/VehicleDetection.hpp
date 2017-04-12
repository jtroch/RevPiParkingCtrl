#include <onposix/AbstractThread.hpp>
#include <onposix/PosixSharedQueue.hpp>

#include "RevPiParkingCtrl.hpp"
#include "IOHandler.hpp"
#include "Settings.hpp"

using namespace onposix;

class VehicleDetection : public AbstractThread { 
    private: 
        PosixSharedQueue<HttpMsgType> * HttpWorkQueue;
        struct itimerval EntranceTimer;
        struct itimerval ExitTimer;
        void FireEntranceTimer();
        void FireExitTimer();
        void EntranceTimerCallback(int signum);
        void ExitTimerCallback(int signum);

    public:
        VehicleDetection(PosixSharedQueue<HttpMsgType> * queue);
        ~VehicleDetection() {};
        void run();
};