#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

#include "HttpThread.hpp"

using namespace onposix;

class Barrier : public HttpThread { 
    private: 
        struct itimerval Timer;
        GateType barrier;
        bool OpenBarrier;
        void fireTimer();
        void setBarrier(int value);
        void timerCallback(int signum);

        int ParseResponse(RestClient::Response response);
        int HandleRequest();
        
    public:
        Barrier(GateType type);
        ~Barrier() {};
        void run();
};