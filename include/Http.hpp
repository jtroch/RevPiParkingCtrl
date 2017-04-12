#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>
#include "RevPiParkingCtrl.hpp"

class Http {
    private:
        Http() {};

    public:
        static void HandleRequest(RestClient::Connection* conn, HttpMsgType type);
};
    