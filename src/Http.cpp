#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <syslog.h>

#include "Http.hpp"

void Http::HandleRequest(RestClient::Connection* conn, HttpMsgType type) {
    RestClient::Response response;
    std::string url;
   
    // set connection timeout to 5s
    conn->SetTimeout(5);

    // set headers
    RestClient::HeaderFields headers;
    headers["Content-Type"] = "text/json";
    conn->SetHeaders(headers);

    url = BASE_API_PATH;

    switch (type) {
        case HTTP_MSG_GET_SETTINGS:
            url += "settings";
            syslog(LOG_DEBUG, "HTTP: sending GET %s", url.c_str());
            response = conn->get(url);
            break;
        case HTTP_MSG_GET_IDKEY:
            url += "idkey";
            syslog(LOG_DEBUG, "HTTP: sending GET %s", url.c_str());
            response = conn->get(url);
            break;
        case HTTP_MSG_POST_ENTRANCE:
            url += "entrance";  
            syslog(LOG_DEBUG, "HTTP: sending POST %s", url.c_str());
            response = conn->post(url, "{\"foo\": \"bla\"}");
            break;
        case HTTP_MSG_POST_EXIT:
            url += "exit";
            syslog(LOG_DEBUG, "HTTP: sending POST %s", url.c_str());
            response = conn->post(url, "{\"foo\": \"bla\"}");
            break;
    }
    
    syslog(LOG_DEBUG, "HTTP: response = (%i) %s", response.code, response.body.c_str());
}