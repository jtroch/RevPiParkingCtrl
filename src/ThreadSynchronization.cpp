#include <stdio.h>
#include <sys/sem.h>
#include <syslog.h>

#include "ThreadSynchronization.hpp"

using namespace onposix;

PosixMutex ThreadSynchronization::SerialMutex;
PosixMutex ThreadSynchronization::SettingsMutex;
PosixMutex ThreadSynchronization::IOMutex;
PosixMutex ThreadSynchronization::IdKeyMutex;
int        ThreadSynchronization::semIdEntranceBarrier=-1;
int        ThreadSynchronization::semIdExitBarrier=-1;
       
void ThreadSynchronization::AcquireEntranceBarrierSemaphore() {
    struct sembuf sb;

void ThreadSynchronization::CreateBarrierSemaphore() {
    // create new semaphore
    semid = semget(MY_SEM_ID, 1, 0666 | IPC_CREAT);

    if (semid >= 0) {
        syslog(LOG_DEBUG, "SYNCHRO: semaphore %i created", semid);
    } else {
        syslog(LOG_ERR, "SYNCHRO: semaphore creation error");
    }
}
       
void ThreadSynchronization::AcquireBarrierSemaphore() {
    struct sembuf sb;

    if (semid<0) CreateBarrierSemaphore();

    // acquire
    sb.sem_num=0;
    sb.sem_op=-1;
    sb.sem_flg=0;
    if (semop(semid, &sb, 1) >= 0) {
         syslog(LOG_DEBUG, "SYNCHRO: semaphore acquired");
    }
}

void ThreadSynchronization::ReleaseBarrierSemaphore() {
    struct sembuf sb;

    if (semid<0) CreateBarrierSemaphore();

    // acquire
    sb.sem_num=0;
    sb.sem_op= 1;
    sb.sem_flg=0;
    if (semop(semid, &sb, 1) >= 0) {
         syslog(LOG_DEBUG, "SYNCHRO: semaphore released");
    }

}