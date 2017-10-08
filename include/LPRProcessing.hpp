#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

#include "HttpThread.hpp"

using namespace onposix;

class LPRProcessing : public HttpThread { 
    private: 
        bool onLoop();

        int HandleRequest();

        int HandleBarrierRequest();
        int ParseBarrierResponse(RestClient::Response response);
        std::string barrierUrl;
        
        int HandleLoopRequest(int loop);
        int ParseLoopResponse(RestClient::Response response);
        std::string loopUrl;

    public:
        LPRProcessing();
        ~LPRProcessing() {};
        void run();
};