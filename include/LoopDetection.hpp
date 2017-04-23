#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

#include "HttpThread.hpp"

using namespace onposix;

class LoopDetection : public HttpThread { 
    private: 
        GateType loop;
        //struct itimerval timer;
        //void fireTimer();
        //void timerCallback(int signum);
        bool onLoop();

        int ParseResponse(RestClient::Response response);
        int HandleRequest();
        std::string url;
        std::string body;
        char loopstring[12];

    public:
        LoopDetection(GateType type);
        ~LoopDetection() {};
        void run();
};