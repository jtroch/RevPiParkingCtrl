//#include "IOHandler.hpp"
#include <onposix/AbstractThread.hpp>
#include <onposix/PosixSharedQueue.hpp>

#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

#include "RevPiParkingCtrl.hpp"
#include "HttpThread.hpp"

using namespace onposix;

class Settings : public HttpThread {
private:
    static bool EntranceCO;
    static bool ExitCO;
    static bool PLCAuto;
    static int  LoopTimeout;
    static int  HttpTimeout;
    static int  BarrierPulseLength;
    static bool TestOutput;
    

    int ParseResponse(RestClient::Response response);
    int HandleRequest();  
    std::string url;
    
public:
    Settings();
    void run();

    static SiteType Site;

    static void Update(
        bool entranceCO,
        bool exitCO,
        bool PLCAuto,
        int  loopTimeout,
        int  httpTimeout,
        int  barrierPulseLength,
        bool testOutput
    );
    static long long CurrentTimeInMilliseconds();

    static bool PLCWorksAutonomously();
    static bool BarrierContinuouslyOpen(GateType type);

    static int  GetLoopTimeout();
    static int  GetHttpTimeout();
    static int  GetBarrierPulseLength();
};