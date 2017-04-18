#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

#include "HttpThread.hpp"

using namespace onposix;

class LoopDetection : public HttpThread { 
    private: 
        GateType loop;
        struct itimerval timer;
        void fireTimer();
        void timerCallback(int signum);

        int ParseResponse(RestClient::Response response);
        int HandleRequest();
        std::string url;
        std::string body;

    public:
        LoopDetection(GateType type);
        ~LoopDetection() {};
        void run();
};