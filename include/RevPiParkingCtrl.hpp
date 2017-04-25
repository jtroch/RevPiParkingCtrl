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

//#define SERVER_ADDRESS "http://192.168.1.110"

#define SERVER_ADDRESS "http://api.rsbots.org"

#define BASE_API_PATH  "/api.imber.nu/client/"

//#define DYNAMIC_AUTHENTICATION

#define SETTINGS_UPDATE_INTERVAL 1000000

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

#endif