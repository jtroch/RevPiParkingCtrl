#include "IOHandler.hpp"
#include <PosixSharedQueue.hpp>

class Settings : public AbstractThread {
private:
    PosixSharedQueue<HttpMsgTypeint>& HttpWorkQueue;

    static bool EntranceBarrierContinuouslyOpen;
    static bool ExitBarrierContinuouslyOpen;
    static bool PLCWorksAutonomous;
    static int DetectionLoopTimeout;
    static int HttpReplyTimeout;
    static int BarrierPulseLength;
    static int TestOutput;

public:
    Settings(PosixSharedQueue<HttpMsgType>& queue);
    void run();

    static Update(
        bool EntranceBarrierContinuouslyOpen,
        bool ExitBarrierContinuouslyOpen,
        bool PLCWorksAutonomous,
        int DetectionLoopTimeout,
        int HttpReplyTimeout,
        int BarrierPulseLength,
        int TestOutput
    );
    static bool PLCWorksAutonomously();
    static bool BarrierContinuouslyOpen(BarrierType type();
    static int  GetDetectionLoopTimeout();
    static int  GetHttpReplyTimeout();
    static int  GetBarrierPulseLength();
};