#include <stdio.h>
#include <sys/sem.h>
#include <syslog.h>

#include "ThreadSynchronization.hpp"

using namespace onposix;

PosixMutex ThreadSynchronization::SerialMutex;
PosixMutex ThreadSynchronization::SettingsMutex;
PosixMutex ThreadSynchronization::IOMutex;
PosixMutex ThreadSynchronization::IdKeyMutex;
int        ThreadSynchronization::semid=-1;
       
void ThreadSynchronization::AcquireEntranceBarrierSemaphore() {
    struct sembuf sb;

    if (semIdEntranceBarrier < 0) {
        if ((semIdEntranceBarrier=semget(SEM_ID_ENTRANCE, 1, 0666 | IPC_CREAT)) < 0)  
            syslog(LOG_ERR, "SYNCHRO: entrance barrier semaphore creation error");

    // acquire
    sb.sem_num=0;
    sb.sem_op=-1;
    sb.sem_flg=0;
    if (semop(semIdEntranceBarrier, &sb, 1) >= 0) {
         syslog(LOG_DEBUG, "SYNCHRO: entrance barrier semaphore acquired");
    }
}

void ThreadSynchronization::ReleaseEntranceBarrierSemaphore() {
    struct sembuf sb;

    if (semIdEntranceBarrier < 0) {
        if ((semIdEntranceBarrier=semget(SEM_ID_ENTRANCE, 1, 0666 | IPC_CREAT)) < 0)  
            syslog(LOG_ERR, "SYNCHRO: entrance barrier semaphore creation error");

    // release
    sb.sem_num=0;
    sb.sem_op= 1;
    sb.sem_flg=0;
    if (semop(semIdEntranceBarrier, &sb, 1) >= 0) {
         syslog(LOG_DEBUG, "SYNCHRO: entrance barrier semaphore released");
    }
}

void ThreadSynchronization::AcquireExitBarrierSemaphore(GateType type) {
    struct sembuf sb;

    if (semIdExitBarrier < 0) {
        if ((semIdExitBarrier=semget(SEM_ID_EXIT, 1, 0666 | IPC_CREAT)) < 0)  
            syslog(LOG_ERR, "SYNCHRO: exit barrier semaphore creation error");

    // acquire
    sb.sem_num=0;
    sb.sem_op=-1;
    sb.sem_flg=0;
    if (semop(semIdExitBarrier, &sb, 1) >= 0) {
         syslog(LOG_DEBUG, "SYNCHRO: exit barrier semaphore acquired");
    }
}

void ThreadSynchronization::ReleaseExitBarrierSemaphore(GateType type) {
    struct sembuf sb;

    if (semIdExitBarrier < 0) {
        if ((semIdExitBarrier=semget(SEM_ID_EXIT, 1, 0666 | IPC_CREAT)) < 0)  
            syslog(LOG_ERR, "SYNCHRO: exit barrier semaphore creation error");

    // acquire
    sb.sem_num=0;
    sb.sem_op= 1;
    sb.sem_flg=0;
    if (semop(semIdExitBarrier, &sb, 1) >= 0) {
         syslog(LOG_DEBUG, "SYNCHRO: exit barrier semaphore released");
    }
}