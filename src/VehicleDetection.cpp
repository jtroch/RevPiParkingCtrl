#include "VehicleDetection.hpp"
#include <Logger.hpp>

VehicleDetection::VehicleDetection(PosixSharedQueue<HttpMsgType>& queue) {
    // Work-queue
    this.HttpWorkQueue = queue;
    
    IOhandler = new IOHandler.getInstance();

    // Configure timers
    struct sigaction sa_entrance, sa_exit;
    
    /* Install timer handler for entrance loop detection */
    memset (&sa_entrance, 0, sizeof (sa_entrance));
    sa_entrance.sa_handler = &EntranceTimerCallback;
    sigaction (SIGVTALRM, &sa_entrance, NULL);

    /* Install timer handler for exit loop detection */
    memset (&sa_exit, 0, sizeof (sa_exit));
    sa_exit.sa_handler = &ExitTimerCallback;
    sigaction (SIGVTALRM, &sa_exit, NULL); 
}

void VehicleDetection::FireEntranceTimer() {

    /* Configure the timers to expire after .... msec. (and no repeat) */
    EntranceTimer.it_value.tv_sec = 0;
    EntranceTimer.it_value.tv_usec = Settings.GetDetectionLoopTimeout();
    EntranceTimer.it_interval.tv_sec = 0;
    EntranceTimer.it_interval.tv_usec = 0;

    setitimer(ITIMER_REAL, &EntranceTimer, NULL);
}

void VehicleDetection::FireExitTimer() {

    /* Configure the timers to expire after .... msec. (and no repeat) */
    ExitTimer.it_value.tv_sec = 0;
    ExitTimer.it_value.tv_usec = Settings.GetDetectionLoopTimeout();
    ExitTimer.it_interval.tv_sec = 0;
    ExitTimer.it_interval.tv_usec = 0;
    
    setitimer(ITIMER_REAL, &ExitTimer, NULL);
}

// Callback, called when timer of entry loop ends
// Time during which vehicle should be on entry loop
void VehicleDetection::EntranceTimerCallback(int signum) {
   uint8_t bOnLoop;

   bOnLoop = IOhandler.GetIO("EntranceLoopActive");
   if (bOnLoop) {
      if( !Settings.PLCWorksAutonomously()) {    
          this.HttpWorkQueue.push(HTTP_POST_ENTRANCE);
      } else {
          // trigger barrier directly
          
      }
   }
}

// Callback, called when timer of exit loop ends
// Time during which vehicle should be on entry loop
// Triggers the http send task ot send POST API/EXIT message
void VehicleDetection::ExitTimerCallback(int signum) {
   bool bOnLoop;

   bOnLoop = digitalRead(CONTROLLINO_IN1);

   if (bOnLoop)
      if( !isAutonomous()) {
          this.HttpWorkQueue.add(HTTP_POST_EXIT);
      } else {
          // send message to barrier thread
      }
   }
}

void VehicleDetection::run() {
    uint32_t bOnEntryLoop=0;
    uint32_t bWasOnEntryLoop=0;
    uint32_t bOnExitLoop=0;
    uint32_t bWasOnExitLoop=0;
    
    while(1)  {
        if (GotApiIdKey) {
            bOnEntryLoop = IOhandler.GetIO("EntranceLoopActive");
            bOnExitLoop = IOhandler.GetIO("ExitLoopActive");

            // entry loop
            if (bOnEntryLoop && !bWasOnEntryLoop) { // rising edge detection
                bWasOnEntryLoop = true;
                FireEntranceTimer();
            } 
            else if (!bOnEntryLoop) { // reset when vehicle leaves loop
                bWasOnEntryLoop = false;
            }
            // exit loop
            if (bOnExitLoop && !bWasOnExitLoop) { // reset when vehicle leaves loop
                bWasOnExitLoop = true;
                FireExitTimer();
            } 
            else if (!bOnExitLoop) { // reset when vehicle leaves loo
                bWasOnExitLoop = false;
            }
        }
        usleep(SETTINGS_UPDATE_INTERVAL20); 
    }
    return NULL;
}