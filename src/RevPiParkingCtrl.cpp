#include "Ledanimation.hpp"
#include "VehicleDetection.hpp"
#include "Barrier.hpp"
#include "Authentication.hpp"
#include "NetworkHandler.hpp"
#include "Settings.hpp"

#include "RevPiParkingCtrl.h"
#include <Logger.hpp>
#include <PosixSharedQueue.h>

#include SETTINGS_UPDATE_INTERVAL 20000

void main()
{
    PosixSharedQueue<HttpMsgType> httpWorkQueue = new PosixSharedQueue<HttpMsgType>();
    Authentication authentication = new Authentication(httpWorkQueue);
    VehicleDetection vehicleDetection = new VehicleDetection(httpWorkQueue);
    Settings settings = new Settings(httpWorkQueue);
    LedAnimation ledAnimation = new LedAnimation(httpWorkQueue);
    Barrier entranceBarrier = new Barrier(ENTRANCE_BARRIER);
    Barrier exitBarrier = new Barrier(EXIT_BARRIER);
    
    LOG_FILE("./tmp/myproject");

    DEBUG("Parking " << "PLC");
    DEBUG("Parking " << "PLC");
    
    std::cout << "-------------------------------------------------------" << std::endl;
    std::cout << "                    Parking PLC"                         << std::endl;
    std::cout << "-------------------------------------------------------" << std::endl;

    std::cout << "Starting Tasks..." << std::endl;
    
    authentication.start();
    vehicleDetection.start();
    settings.start();
    LedAnimation.start();
    entranceBarrier.start();
    exitBarrier.start();


}