#include <stdint.h>

#ifndef RevPiParkingCtrl_H
#define RevPiParkingCtrl_H

//#define LOG_LEVEL_CONSOLE  LOG_NOLOG
//#define LOG_LEVEL_CONSOLE  LOG_WARNINGS
//#define LOG_LEVEL_CONSOLE  LOG_ERRORS
//#define LOG_LEVEL_CONSOLE  LOG_ALL

//#define LOG_LEVEL_FILE  LOG_NOLOG
//#define LOG_LEVEL_FILE  LOG_WARNINGS
//#define LOG_LEVEL_FILE  LOG_ERRORS
//#define LOG_LEVEL_FILE  LOG_ALL

//#define SERVER_ADDRESS "http://api.imber.nu"

#define SERVER_ADDRESS "http://192.168.0.132"

#define BASE_API_PATH  "/v1/controller/2/parking"

//#define DYNAMIC_AUTHENTICATION

#define SETTINGS_UPDATE_INTERVAL 1000000

#define SITE_ZWIJNAARDSESTEENWEG

enum HttpMsgType {
    HTTP_MSG_GET_SETTINGS,
    HTTP_MSG_GET_IDKEY,
    HTTP_MSG_POST_ENTRANCE,
    HTTP_MSG_POST_EXIT
};

enum GateType {
    ENTRANCE,
    EXIT,
    ENTRANCE_EXIT
};

enum SiteType {
    ZWIJNAARDE,
    ST_DENIJS,
};

// list of IOs
/*   Name                   Type            IO          Definition

    EntranceLoopAct         Input           I_1         active when vehicle is on entrance loop
    ExitLoopAct             Input           I_2         active when vehicle is on exit loop
    EntranceCO              Input           I_3         active means entrance barrier is always open
    ExitCO                  Input           I_4         active means exit barrier is always open
    PLCAuto                 Input           I_5         active means PLC works atonomously without server
    TicketAct               Input           I_6         active (pulse) when ticket is inserted 
    MoneyAct                Input           I_7         active (pulse) when money is inserted
    SafetyLoopAct           Input           I_8         active when vehicle is on safety loop

    ContinuOn               Output          O_1         Output continuously on
    ContinuClosed           Output          O_2         To 'Continue closed' input of barrier in case of site ZWIJNAARDE where there is only one barrier
    OpenEntrance            Output          O_3         active (pulse) means open entrance barrier
    OpenExit                Output          O_4         active (pulse) means open exit barrier

*/


#endif