#pragma once

#include <iostream>
#include <cstdio>
#include <vector>
#include "rapidjson/document.h"

#define CONFIG_NAME ".server_config.json"
typedef struct {
    std::string name;
    std::string value;
} ConfEntry;

namespace tet {

class WebConf{
public:
    WebConf();
    virtual ~WebConf();
    bool serviceEnabled;
    std::vector<ConfEntry> configuration;
private:
    bool readConf();
    bool checkValidity(rapidjson::Document *doc);
    void setConf(rapidjson::Document *doc);
    void makeConf();
    const char* keys[7] = {
                "dbname",
                "port",
                "hostaddr",
                "hostname",
                "username",
                "password",
                "table"};
};
}
