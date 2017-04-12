#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <syslog.h>

#include "Http.hpp"

void Http::HandleRequest(RestClient::Connection* conn, HttpMsgType type) {
// set connection timeout to 5s
    conn->SetTimeout(5);

    // set headers
    RestClient::HeaderFields headers;
    headers["Content-Type"] = "text/json";
    conn->SetHeaders(headers);
    RestClient::Response r = conn->get("/api.domain.com/settings");
    syslog(LOG_DEBUG, "SETTINGS: response = (%i) %s", r.code, r.body.c_str());
}