#include <sys/sem.h>

#include <onposix/PosixMutex.hpp>

#define SEM_ID_ENTRANCE 1
#define SEM_ID_EXIT 2

using namespace onposix;

class ThreadSynchronization {
    public:
        static PosixMutex SerialMutex;
        static PosixMutex SettingsMutex;
        static PosixMutex IOMutex;
        static PosixMutex IdKeyMutex;

        static void AcquireEntranceBarrierSemaphore();
        static void ReleaseEntranceBarrierSemaphore();

        static void AcquireExitBarrierSemaphore();
        static void ReleaseExitBarrierSemaphore();
    private:
        static int semIdEntranceBarrier;
        static int semIdExitBarrier;
};