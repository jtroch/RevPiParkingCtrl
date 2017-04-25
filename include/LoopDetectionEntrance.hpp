#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

#include "HttpThread.hpp"

using namespace onposix;

class LoopDetectionEntrance : public HttpThread { 
    private: 
        //struct itimerval timer;
        //void fireTimer();
        //void timerCallback(int signum);
        bool onLoop();

        int ParseResponse(RestClient::Response response);
        int HandleRequest();
        std::string url;
        std::string body;

    public:
        LoopDetectionEntrance();
        ~LoopDetectionEntrance() {};
        void run();
};