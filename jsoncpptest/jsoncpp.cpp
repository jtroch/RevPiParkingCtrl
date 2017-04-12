#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <syslog.h>

#include <json/json.h>
#include <json/json-forwards.h>

void output(const Json::Value & value)
{
    // querying the json object is very simple
    std::cout << value["hello"];
    std::cout << value["number"];
    std::cout << value["array"][0] << value["array"][1];
    std::cout << value["object"]["hello"];
}

main()
{
    Json::Value fromScratch;
    Json::Value array;
    array.append("hello");
    array.append("world");
    fromScratch["hello"] = "world";
    fromScratch["number"] = 2;
    fromScratch["array"] = array;
    fromScratch["object"]["hello"] = "world";

    output(fromScratch);

    // write in a nice readible way
    Json::StyledWriter styledWriter;
    std::cout << styledWriter.write(fromScratch);

    // ---- parse from string ----

    // write in a compact way
    Json::FastWriter fastWriter;
    std::string jsonMessage = fastWriter.write(fromScratch);
    std::cout << jsonMessage << std::endl;

    Json::Value parsedFromString;
    Json::Reader reader;
    bool parsingSuccessful = reader.parse(jsonMessage, parsedFromString);
    if (parsingSuccessful)
    {
        std::cout << styledWriter.write(parsedFromString) << std::endl;
    }
}

    