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

#include "SiteZwijnaardeProcessing.hpp"

void SiteZwijnaardeProcessing::start() {
    //processing.start();
    lprProcessing.start();
    barrier->start();
}

void SiteZwijnaardeProcessing::waitForTermination() {
    //processing.waitForTermination(); 
    lprProcessing.waitForTermination();
    barrier->waitForTermination();
}