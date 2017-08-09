#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

#include "RevPiParkingCtrl.hpp"
#include "Barrier.hpp"
#include "EntranceProcessing.hpp"
#include "ExitProcessing.hpp"

class SiteStDenijsProcessing { 
    private: 
        EntranceProcessing entranceProcessing;
        ExitProcessing exitProcessing; 
        Barrier * entranceBarrier; 
        Barrier * exitBarrier;

    public:
        SiteStDenijsProcessing() {
            entranceBarrier = new Barrier(ENTRANCE);
            exitBarrier = new Barrier(EXIT);
        };
        ~SiteStDenijsProcessing() {
            delete entranceBarrier;
            delete exitBarrier;
        };
        void start();
        void waitForTermination();
};