#include "ThreadSynchronization.hpp"
#include <Logger.hpp>

static void ThreadSynchronization::LockSettings() {
    SerialMutex.lock();
}

static void ThreadSynchronization::UnlockSettings() {
    SerialMutex.unlock();
}

static void ThreadSynchronization::LockSerial() {
    SerialMutex.lock();
}

static void ThreadSynchronization::UnlockSerial() {
    SerialMutex.unlock();
}

static void ThreadSynchronization::LockIO() {
    IOMutex.lock();
}

static void ThreadSynchronization::UnlockIO() {
    IOMutex.unlock();
}