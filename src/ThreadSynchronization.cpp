#include "ThreadSynchronization.hpp"

using namespace onposix;

pthread_mutex_t ThreadSynchronization::SynchroMutex = PTHREAD_MUTEX_INITIALIZER;

ThreadSynchronization* ThreadSynchronization::instance = NULL;

void ThreadSynchronization::LockSettings() {
    SerialMutex.lock();
}

void ThreadSynchronization::UnlockSettings() {
    SerialMutex.unlock();
}

void ThreadSynchronization::LockSerial() {
    SerialMutex.lock();
}

void ThreadSynchronization::UnlockSerial() {
    SerialMutex.unlock();
}

void ThreadSynchronization::LockIO() {
    IOMutex.lock();
}

void ThreadSynchronization::UnlockIO() {
    IOMutex.unlock();
}

void ThreadSynchronization::LockIdKey() {
    IdKeyMutex.lock();
}

void ThreadSynchronization::UnlockIdKey() {
    IdKeyMutex.unlock();
}

ThreadSynchronization * ThreadSynchronization::getInstance() {
    pthread_mutex_lock(&SynchroMutex);
    if (instance == NULL) {
         instance = new ThreadSynchronization();
    }
    pthread_mutex_unlock(&SynchroMutex);
    return instance;
}