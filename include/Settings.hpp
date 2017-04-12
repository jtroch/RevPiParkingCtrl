//#include "IOHandler.hpp"
#include <onposix/AbstractThread.hpp>
#include <onposix/PosixSharedQueue.hpp>

#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

using namespace onposix;

class Settings : public AbstractThread {
private:
    PosixSharedQueue<HttpMsgType> * HttpWorkQueue;

    static bool EntranceBarrierContinuouslyOpen;
    static bool ExitBarrierContinuouslyOpen;
    static bool PLCWorksAuto;
    static int  DetectionLoopTimeout;
    static int  HttpReplyTimeout;
    static int  BarrierPulseLength;
    static int  TestOutput;

    RestClient::Connection* HttpConnection;

public:
    Settings(PosixSharedQueue<HttpMsgType> * queue);
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
    static bool BarrierContinuouslyOpen(BarrierType type);
    static int  GetDetectionLoopTimeout();
    static int  GetHttpReplyTimeout();
    static int  GetBarrierPulseLength();
};