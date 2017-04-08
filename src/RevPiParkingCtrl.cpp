#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <onposix/AbstractThread.hpp>
#include <onposix/Logger.hpp>
#include <onposix/PosixMutex.hpp>
#include <onposix/PosixSharedQueue.hpp>

#include "LedAnimation.hpp"
#include "RevPiParkingCtrl.hpp"

/*
#include "VehicleDetection.hpp"
#include "Barrier.hpp"
#include "Authentication.hpp"
#include "NetworkHandler.hpp"
#include "Settings.hpp"
*/

int main()
{
    /*
    PosixSharedQueue<HttpMsgType> httpWorkQueue = new PosixSharedQueue<HttpMsgType>();
    Authentication authentication = new Authentication(httpWorkQueue);
    VehicleDetection vehicleDetection = new VehicleDetection(httpWorkQueue);
    Settings settings = new Settings(httpWorkQueue);
    NetworkHandler networkHandler = new NetworkHandler(httpWorkQueue);
    Barrier entranceBarrier = new Barrier(ENTRANCE_BARRIER);
    Barrier exitBarrier = new Barrier(EXIT_BARRIER);
    */
    
    LedAnimation ledAnimation;

    LOG_FILE("./tmp/myproject");

    DEBUG("Parking " << "PLC");
    DEBUG("Parking " << "PLC");
    
    DEBUG("-------------------------------------------------------\n"); 
    DEBUG("                    Parking PLC                        \n");
    DEBUG("-------------------------------------------------------\n");

    std::cout << "Starting Tasks...\n";
    
    ledAnimation.start();

    
    

    // Start authentication and wait for completion before starting other tasks
    /*
    authentication.start();
    authentication.waitForTermination();

    settings.start();
    vehicleDetection.start();
    networkHandler.start();
    entranceBarrier.start();
    exitBarrier.start();
    */

    return 0;
}