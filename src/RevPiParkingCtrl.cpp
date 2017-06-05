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
#include "LoopDetectionEntrance.hpp"
#include "LoopDetectionExit.hpp"
#include "Barrier.hpp"
#include "IOHandler.hpp"

#include <piControlIf.hpp>
#include <piControl.h>

using namespace onposix;

PosixMutex IOMutex;

int main()
{
    // initialize RestClient
    RestClient::init();

    PosixMutex IOMutex;
    
    LedAnimation ledAnimation;
    Authentication authentication;
    
    std::cout << "-------------------------------------------------------" << std::endl;
    std::cout << "                    Parking PLC                        " << std::endl;
    std::cout <<  "-------------------------------------------------------" << std::endl;

    std::cout << "Starting Tasks...\n";

    syslog(LOG_INFO, "--------------------------------------------------");
    syslog(LOG_INFO, "-----------------   Parking PLC ------------------");
    syslog(LOG_INFO, "--------------------------------------------------");
   
#if DYNAMIC_AUTHENTICATION
    std::cout << "Starting Athentication ..." << std::endl;
    syslog(LOG_INFO, "Starting Authentication ..");
    authentication.start();
    // wait for authentication to finish before going on
    authentication.waitForTermination();
#else
    std::cout << "Only static authentication." << std::endl;
    syslog(LOG_INFO, "Only static Authentication ..");
#endif

    syslog(LOG_INFO, "Checking all IOs ..");
    syslog(LOG_INFO, "EntranceLoopAct: %i",  IOHandler::GetIO("EntranceLoopAct"));
    syslog(LOG_INFO, "ExitLoopAct    : %i",  IOHandler::GetIO("ExitLoopAct"));
    syslog(LOG_INFO, "EntranceCO     : %i",  IOHandler::GetIO("EntranceCO"));
    syslog(LOG_INFO, "ExitCO         : %i",  IOHandler::GetIO("ExitCO"));
    syslog(LOG_INFO, "PLCAuto        : %i",  IOHandler::GetIO("PLCAuto"));
    syslog(LOG_INFO, "OpenEntrance   :   "); IOHandler::SetIO("OpenEntrance", false);
    syslog(LOG_INFO, "OpenExit       :   "); IOHandler::SetIO("OpenExit", false);
    syslog(LOG_INFO, "TestOutput     :   "); IOHandler::SetIO("TestOutput", false);

    std::cout << "Finished, starting all other thtreads.." << std::endl;

    Settings settings;
    ledAnimation.start();
    settings.start();

    Barrier entranceBarrier(ENTRANCE);
    Barrier exitBarrier(EXIT);
    LoopDetectionEntrance entranceLoopDetection;
    LoopDetectionExit exitLoopDetection;
    entranceBarrier.start();
    exitBarrier.start();
    entranceLoopDetection.start();
    exitLoopDetection.start();

// Wait for all tasks to finish but the application should not get here
    
    ledAnimation.waitForTermination();
    settings.waitForTermination();
    entranceLoopDetection.waitForTermination();
    entranceBarrier.waitForTermination();
    exitLoopDetection.waitForTermination();
    exitBarrier.waitForTermination();

    return 0;
}