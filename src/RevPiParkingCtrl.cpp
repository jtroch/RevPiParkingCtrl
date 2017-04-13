#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <syslog.h>

//#define NDEBUG

#include <onposix/AbstractThread.hpp>
#include <onposix/Logger.hpp>
#include <onposix/PosixMutex.hpp>
#include <onposix/PosixSharedQueue.hpp>

#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

#include "RevPiParkingCtrl.hpp"
#include "LedAnimation.hpp"
#include "Settings.hpp"
#include "Authentication.hpp"

/*
#include "VehicleDetection.hpp"
#include "Barrier.hpp"
#include "Authentication.hpp"
#include "NetworkHandler.hpp"
*/

int main()
{
    //PosixSharedQueue<HttpMsgType> * httpWorkQueue = new PosixSharedQueue<HttpMsgType>();
    
    // initialize RestClient
    RestClient::init();

    /*
    Authentication authentication = new Authentication(httpWorkQueue);
    VehicleDetection vehicleDetection = new VehicleDetection(httpWorkQueue);
    NetworkHandler networkHandler = new NetworkHandler(httpWorkQueue);
    Barrier entranceBarrier = new Barrier(ENTRANCE_BARRIER);
    Barrier exitBarrier = new Barrier(EXIT_BARRIER);
    */
    
    LedAnimation ledAnimation;
    Settings settings();
    Authentication authentication;
    
    std::cout << "-------------------------------------------------------" << std::endl;
    std::cout << "                    Parking PLC                        " << std::endl;
    std::cout <<  "-------------------------------------------------------" << std::endl;

    std::cout << "Starting Tasks...\n";

    syslog(LOG_DEBUG, "--------------------------------------------------");
    syslog(LOG_DEBUG, "-----------------   Parking PLC ------------------");
    syslog(LOG_DEBUG, "--------------------------------------------------");

    syslog(LOG_DEBUG, "Starting Authentication ..");

    //syslog(LOG_WARNING, "Dit is een warning");
    //syslog(LOG_ERR, "Dit is een error");

    authentication.start();
    //authentication.waitForTermination();

    ledAnimation.start();
    settings.start();
    

    ledAnimation.waitForTermination();
    settings.waitForTermination();

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