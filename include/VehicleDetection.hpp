#include <onposix/AbstractThread.hpp>

#include "RevPiParkingCtrl.hpp"
#include "IOHandler.hpp"
#include "Settings.hpp"

using namespace onposix;

class VehicleDetection : public AbstractThread { 
    private: 
        struct itimerval EntranceTimer;
        struct itimerval ExitTimer;
        void FireEntranceTimer();
        void FireExitTimer();
        void EntranceTimerCallback(int signum);
        void ExitTimerCallback(int signum);

    public:
        VehicleDetection();
        ~VehicleDetection() {};
        void run();
};