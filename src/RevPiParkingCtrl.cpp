#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <syslog.h>
#include <unistd.h>

//#define NDEBUG

#include <json/json.h>
#include <json/json-forwards.h>

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
#include "SiteZwijnaardeProcessing.hpp"
#include "SiteStDenijsProcessing.hpp"
#include "IOHandler.hpp"

#include <piControlIf.hpp>
#include <piControl.h>

using namespace onposix;

int ReadConfigFile(){
   
    Json::Value root;   // will contains the root value after parsing.
    Json::Reader reader;
    std::ifstream configstream("config.json", std::ifstream::binary);
    bool parsingSuccessful = reader.parse(configstream, root, false );
    if ( !parsingSuccessful )
    {
        // report to the user the failure and their locations in the document.
        std::cout  << reader.getFormattedErrorMessages() << "\n";
    }

    std::string site = root.get("site", "UTF-8" ).asString();
    std::cout << "Config: " << site << "\n";

    if       (site.compare("zwijnaarde")==0) Settings::Site=ZWIJNAARDE;
    else if  (site.compare("st-denijs")==0)  Settings::Site=ST_DENIJS;
    else {
        std::cout << "Error: site " << site << " unknown => fallback to default (Zwijnaarde)\n";
        Settings::Site=ZWIJNAARDE;
    }
    
    return 1;
}

PosixMutex IOMutex;

int main()
{
    // initialize RestClient
    RestClient::init();

    PosixMutex IOMutex;
    
    LedAnimation ledAnimation;
    Authentication authentication;
    Settings settings;
    
    std::cout << "-------------------------------------------------------" << std::endl;
    std::cout << "                    Parking PLC                        " << std::endl;
    std::cout <<  "-------------------------------------------------------" << std::endl;

    std::cout << "Reading config file...\n";
    ReadConfigFile();

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
    // INPUTS
    syslog(LOG_INFO, "Checking all IOs ..");
    syslog(LOG_INFO, "EntranceLoopAct: %i",  IOHandler::GetIO("EntranceLoopAct"));
    syslog(LOG_INFO, "ExitLoopAct    : %i",  IOHandler::GetIO("ExitLoopAct"));
    syslog(LOG_INFO, "EntranceCO     : %i",  IOHandler::GetIO("EntranceCO"));
    syslog(LOG_INFO, "ExitCO         : %i",  IOHandler::GetIO("ExitCO"));
    syslog(LOG_INFO, "PLCAuto        : %i",  IOHandler::GetIO("PLCAuto"));
    syslog(LOG_INFO, "TicketAct      : %i",  IOHandler::GetIO("TicketAct"));
    syslog(LOG_INFO, "MoneyAct       : %i",  IOHandler::GetIO("MoneyAct"));
    // OUPUTS
    syslog(LOG_INFO, "ContinuOn     :   "); IOHandler::SetIO("ContinuOn", true);
    sleep(1); IOHandler::SetIO("ContinuOn", false);
    syslog(LOG_INFO, "ContinuClosed  :   "); IOHandler::SetIO("ContinuClosed", true);
    sleep(1); IOHandler::SetIO("ContinuClosed", false);
    syslog(LOG_INFO, "OpenEntrance   :   "); IOHandler::SetIO("OpenEntrance", true);
    sleep(1); IOHandler::SetIO("OpenEntrance", false);
    syslog(LOG_INFO, "OpenExit       :   "); IOHandler::SetIO("OpenExit", true);
    sleep(1); IOHandler::SetIO("OpenExit", false);

    std::cout << "Finished, starting all other thtreads.." << std::endl;

    //ledAnimation.start();
    //settings.start();

    if (Settings::Site==ZWIJNAARDE) {
        SiteZwijnaardeProcessing zwijnaardeProcessing;
        zwijnaardeProcessing.start();
        // .......
        // Wait for all tasks to finish but the application should not get here
        zwijnaardeProcessing.waitForTermination();
    } else {
        SiteStDenijsProcessing stDenijsProcessing;
        stDenijsProcessing.start();
        // .......
        // Wait for all tasks to finish but the application should not get here
        stDenijsProcessing.waitForTermination();
    }

    ledAnimation.waitForTermination();
    settings.waitForTermination();

    //Barrier entranceBarrier(ENTRANCE);
    //Barrier exitBarrier(EXIT);
    //enranceBarrier.start();
    //exitBarrier.start();


    return 0;
}