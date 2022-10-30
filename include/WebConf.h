#pragma once

#include <iostream>
#include <cstdio>
#include <vector>
#include "rapidjson/document.h"

typedef struct {
    std::string name;
    std::string value;
} ConfEntry;

namespace tet {
//TODO: SHOULD SANITIZE THE CONFIG
class WebConf{
public:
    WebConf();
    virtual ~WebConf();
    void initiate();
    bool serviceEnabled;
    std::vector<ConfEntry> configuration;
private:
    std::string serverConfig;
    bool readConf();
    bool checkValidity(rapidjson::Document *doc);
    void setConf(rapidjson::Document *doc);
    void makeConf();
    const char* keys[8] = {
                "dbname",
                "port",
                "hostaddr",
                "host",
                "user",
                "password",
                "table",
                "connection_timeout"
                };
};
}
