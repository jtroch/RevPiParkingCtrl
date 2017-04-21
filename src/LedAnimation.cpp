#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <iostream>
#include <syslog.h>

#include <onposix/AbstractThread.hpp>
#include "ThreadSynchronization.hpp"
#include "RevPiParkingCtrl.hpp"
#include "LedAnimation.hpp"
#include "piControlIf.hpp"

LedAnimation::LedAnimation() {};
LedAnimation::~LedAnimation() {};

using namespace onposix;

void LedAnimation::run() {
    uint8_t ledvalue=0x01;
    uint8_t newledvalue=0x00;

    printf("Led animation started\n");
    piControl piCtrl;

    while (1)  {    
        ThreadSynchronization::getInstance()->LockIO();
        piCtrl.Write(72, 1, &ledvalue);
        ThreadSynchronization::getInstance()->UnlockIO();
        newledvalue = (ledvalue << 1) | (ledvalue >> 3); // rotate left
        ledvalue = newledvalue;

        syslog(LOG_DEBUG, "Led...");
        usleep(2000000); 
    }
}