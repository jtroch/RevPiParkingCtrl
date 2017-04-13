#include <pthread.h>
#include <onposix/PosixMutex.hpp>

using namespace onposix;

class ThreadSynchronization {
private:
    static pthread_mutex_t SynchroMutex;
    PosixMutex SerialMutex;
    PosixMutex SettingsMutex;
    PosixMutex IOMutex;
    PosixMutex IdKeyMutex;

    static ThreadSynchronization * instance;
    ThreadSynchronization() {};

public:
    static ThreadSynchronization * getInstance();

    void LockSettings();
    void UnlockSettings();
    void LockSerial();
    void UnlockSerial();
    void RxTxSemaphoreTake();
    void RxTxSemaphoreGive();
    void LockIO();
    void UnlockIO();
    void LockIdKey();
    void UnlockIdKey();
};