#include "Barrier.hpp"
#include <Logger.hpp>

Barrier::Barrier(BarrierType type) {
    
    IOhandler = new IOHandler.getInstance();

    // Configure timers
    struct sigaction sa;
    
    /* Install timer handler for entrance barrier */
    memset (&sa, 0, sizeof (sa));
    sa.sa_handler = &TimerCallback;
    sigaction (SIGVTALRM, &sa, NULL);
}

void Barrier::SetBarrier(BarrierType type, int value)  {
    switch (type) {
        case ENTRANCE_BARRIER:
            IOHandler.SetIO("EntranceBarrier", value);
            break;
        case EXIT_BARRIER:
            IOHandler.SetIO("ExitBarrier", value);
            break;
    }
}

void Barrier::FireTimer() {

    /* Configure the timers to expire after .... msec. (and no repeat) */
    Timer.it_value.tv_sec = 0;
    Timer.it_value.tv_usec = Settings.GetBarrierPulseLength();
    Timer.it_interval.tv_sec = 0;
    Timer.it_interval.tv_usec = 0;
    
    setitimer(ITIMER_REAL, &Timer, NULL);
}

// Callback, called when timer ends
void Barrier::TimerCallback(int signum) {
   
    if (!Settings.BarrierContinuouslyOpen(barrier)) // do not close if set to be continuously open bij HW pin
    {
        // close barrier (= actualy don't drive open anymore')
        SetBarrier(barrier, 0);
    }
}

void Barriers::run() {
    
    
    while(1)  {
        
        // wait for trigger by Http thread
        if (...) {
            // open barrier 
            SetBarrier(barrier, 1);
            FireTimer();
        }

        if (BarrierContinuouslyOpen(barrier)) {
            SetBarrier(barrier, 1);
        }

        usleep(SETTINGS_UPDATE_INTERVAL); 
    }
    return NULL;
}