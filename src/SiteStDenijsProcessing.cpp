#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <stdint.h>
#include <iostream>
#include <syslog.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>

#include "SiteStDenijsProcessing.hpp"

void SiteStDenijsProcessing::start() {
    entranceProcessing.start();
    exitProcessing.start();
    entranceBarrier->start();
    exitBarrier->start();
}

void SiteStDenijsProcessing::waitForTermination() {
    entranceProcessing.waitForTermination(); 
    exitProcessing.waitForTermination(); 
    entranceBarrier->waitForTermination();
    exitBarrier->waitForTermination();
}