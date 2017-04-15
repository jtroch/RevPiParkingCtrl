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
#include "LoopDetection.hpp"
#include "Barrier.hpp"

int main()
{
    //PosixSharedQueue<HttpMsgType> * httpWorkQueue = new PosixSharedQueue<HttpMsgType>();
    
    // initialize RestClient
    RestClient::init();
    
    LedAnimation ledAnimation;
    Settings settings;
    Authentication authentication;
    LoopDetection loopDetectionEntrance(ENTRANCE);
    LoopDetection loopDetectionExit(EXIT);
    Barrier entranceBarrier(ENTRANCE);
    Barrier exitBarrier(EXIT);

    
    std::cout << "-------------------------------------------------------" << std::endl;
    std::cout << "                    Parking PLC                        " << std::endl;
    std::cout <<  "-------------------------------------------------------" << std::endl;

    std::cout << "Starting Tasks...\n";

    syslog(LOG_DEBUG, "--------------------------------------------------");
    syslog(LOG_DEBUG, "-----------------   Parking PLC ------------------");
    syslog(LOG_DEBUG, "--------------------------------------------------");

    ledAnimation.start();

    std::cout << "Starting Athentication ..." << std::endl;
    syslog(LOG_DEBUG, "Starting Authentication ..");
    authentication.start();
    // wait for authentication to finish before going on
    authentication.waitForTermination();
    std::cout << "Finished, starting all other thtreads.." << std::endl;

    settings.start();
    loopDetectionEntrance.start();
    entranceBarrier.start();

    //loopDetectionExit.start();
    //exitBarrier.start();
    
    ledAnimation.waitForTermination();
    settings.waitForTermination();
    loopDetectionEntrance.waitForTermination();
    entranceBarrier.waitForTermination();

    return 0;
}