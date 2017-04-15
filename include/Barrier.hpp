#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

#include "HttpThread.hpp"

using namespace onposix;

class Barrier : public HttpThread { 
    private: 
        struct itimerval Timer;
        GateType barrier;
        bool OpenBarrier;
        void FireTimer();
        void SetBarrier(int value);

        int ParseResponse(RestClient::Response response);
        int HandleRequest();
        
    public:
        Barrier(Gatetype type);
        ~Barrier();
        void run();
};