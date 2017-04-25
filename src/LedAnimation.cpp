#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <iostream>
#include <syslog.h>

#include <onposix/AbstractThread.hpp>
#include <onposix/PosixMutex.hpp>

#include "ThreadSynchronization.hpp"
#include "IOHandler.hpp"
#include "RevPiParkingCtrl.hpp"
#include "LedAnimation.hpp"
#include "piControlIf.hpp"

LedAnimation::LedAnimation() {};
LedAnimation::~LedAnimation() {};

using namespace onposix;

void LedAnimation::run() {
    uint8_t ledvalue=0x01;
    uint8_t newledvalue=0x00;

    syslog(LOG_INFO, "LEDANIMATION: thread started");
    piControl piCtrl;

    while (1)  {    
        if ((ledvalue & 0x01)==0x00) IOHandler::SetIO("LED1", false) ; else IOHandler::SetIO("LED1", true);
        if ((ledvalue & 0x02)==0x00) IOHandler::SetIO("LED2", false) ; else IOHandler::SetIO("LED2", true);
        if ((ledvalue & 0x04)==0x00) IOHandler::SetIO("LED3", false) ; else IOHandler::SetIO("LED3", true);
        if ((ledvalue & 0x08)==0x00) IOHandler::SetIO("LED4", false) ; else IOHandler::SetIO("LED4", true);

        newledvalue = (ledvalue << 1) | (ledvalue >> 3); // rotate left
        ledvalue = newledvalue;

        syslog(LOG_DEBUG, "Led...");
        usleep(1000000); 
    }
}