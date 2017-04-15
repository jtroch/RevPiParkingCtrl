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
    static bool EntranceBarrierContinuouslyOpen;
    static bool ExitBarrierContinuouslyOpen;
    static bool PLCWorksAuto;
    static int  DetectionLoopTimeout;
    static int  HttpReplyTimeout;
    static int  BarrierPulseLength;
    static int  TestOutput;

    int ParseResponse(RestClient::Response response);
    int HandleRequest();  
public:
    Settings();
    void run();

    static void Update(
        bool EntranceBarrierContinuouslyOpen,
        bool ExitBarrierContinuouslyOpen,
        bool PLCWorksAuto,
        int DetectionLoopTimeout,
        int HttpReplyTimeout,
        int BarrierPulseLength,
        int TestOutput
    );
    static bool PLCWorksAutonomously();
    static bool BarrierContinuouslyOpen(GateType type);
    static int  GetDetectionLoopTimeout();
    static int  GetHttpReplyTimeout();
    static int  GetBarrierPulseLength();
};