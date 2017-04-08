#include <PosixMutex.h>

class ThreadSynchronization {
private:
    static PosixMutex SerialMutex;
    static PosixMutex SettingsMutex;
    static PosixMutex IOMutex;

    ThreadSynchronization() {} 

public:
    static void LockSettings();
    static void UnlockSettings();
    static void LockSerial();
    static void UnlockSerial();
    static void RxTxSemaphoreTake();
    static void RxTxSemaphoreGive();
    static void LockIO();
    static void UnlockIO();
};