#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

#include "HttpThread.hpp"

using namespace onposix;

class VehicleDetection : public HttpThread { 
    private: 
        struct itimerval EntranceTimer;
        struct itimerval ExitTimer;
        void FireEntranceTimer();
        void FireExitTimer();
        void EntranceTimerCallback(int signum);
        void ExitTimerCallback(int signum);

        int ParseResponse(RestClient::Response response);
        int HandleRequest();

    public:
        VehicleDetection();
        ~VehicleDetection() {};
        void run();
};