#include "ThreadSynchronization.hpp"
#include <piControlIf.hpp>
#include <piControl.h>
#include <stdio.h>
#include <Logger.hpp>

NetworkHandler::NetworkHandler(PosixSharedQueue<HttpMsgType>& queue) {
    // Work-queue
    this.HttpWorkQueue = queue;
}

void VehicleDetection::run() {
    
}
    