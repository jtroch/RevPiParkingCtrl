#include <sys/sem.h>

#include <onposix/PosixMutex.hpp>

#define MY_SEM_ID 1

using namespace onposix;

class ThreadSynchronization {
    public:
        static PosixMutex SerialMutex;
        static PosixMutex SettingsMutex;
        static PosixMutex IOMutex;
        static PosixMutex IdKeyMutex;

        static void CreateBarrierSemaphore();
        static void AcquireBarrierSemaphore();
        static void ReleaseBarrierSemaphore();
    private:
        static int semid;
};