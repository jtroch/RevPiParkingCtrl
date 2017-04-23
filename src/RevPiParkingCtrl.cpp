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

    syslog(LOG_DEBUG, "--------------------------------------------------");
    syslog(LOG_DEBUG, "-----------------   Parking PLC ------------------");
    syslog(LOG_DEBUG, "--------------------------------------------------");
   

    std::cout << "Starting Athentication ..." << std::endl;
    syslog(LOG_DEBUG, "Starting Authentication ..");
    authentication.start();
    // wait for authentication to finish before going on
    authentication.waitForTermination();

    Settings settings;
    LoopDetection loopDetectionEntrance(ENTRANCE);
    LoopDetection loopDetectionExit(EXIT);
    Barrier barrier(ENTRANCE_EXIT);
    
    syslog(LOG_DEBUG, "Checking all IOs ..");
    syslog(LOG_DEBUG, "EntranceLoopAct: %i",  IOHandler::GetIO("EntranceLoopAct"));
    syslog(LOG_DEBUG, "ExitLoopAct    : %i",  IOHandler::GetIO("ExitLoopAct"));
    syslog(LOG_DEBUG, "EntranceCO     : %i",  IOHandler::GetIO("EntranceCO"));
    syslog(LOG_DEBUG, "ExitCO         : %i",  IOHandler::GetIO("ExitCO"));
    syslog(LOG_DEBUG, "PLCAuto        : %i",  IOHandler::GetIO("PLCAuto"));
    syslog(LOG_DEBUG, "OpenEntrance   :   "); IOHandler::SetIO("OpenEntrance", false);
    syslog(LOG_DEBUG, "OpenExit       :   "); IOHandler::SetIO("OpenExit", false);
    syslog(LOG_DEBUG, "TestOutput     :   "); IOHandler::SetIO("TestOutput", false);

    std::cout << "Finished, starting all other thtreads.." << std::endl;

    ledAnimation.start();
    settings.start();
    loopDetectionEntrance.start();
    loopDetectionExit.start();
    barrier.start();
    
    ledAnimation.waitForTermination();
    settings.waitForTermination();
    loopDetectionEntrance.waitForTermination();
    loopDetectionExit.waitForTermination();
    barrier.waitForTermination();
  
    return 0;
}