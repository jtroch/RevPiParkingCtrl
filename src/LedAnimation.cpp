#include "LedAnimation.hpp"
#include <Logger.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

VehicleDetection::VehicleDetection() {
    IOhandler = new IOHandler.getInstance();
}

void VehicleDetection::run() {
    uint8_t ledvalue=0x00;
    uint8_t newledvalue=0x00;
    
    ledvalue=0x01;
    
    for (;;)  {    
        SetIO("Leds", ledvalue);
        newledvalue = (x << 1) | (x >> 7); // rotate left
        ledvalue = newledvalue;
        usleep(SETTINGS_UPDATE_INTERVAL); 
    }
}