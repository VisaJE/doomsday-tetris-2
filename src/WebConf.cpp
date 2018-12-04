#include "WebConf.h"
#include <iostream>
#include <vector>
#include <cstdio>
#include <sstream>
#include <fstream>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"


using namespace rapidjson;
using namespace tet;

WebConf::WebConf()
{
    configuration = std::vector<ConfEntry>();
    if (!readConf()) makeConf();
}

WebConf::~WebConf()
{
}

bool WebConf::readConf()
{
    FILE* conf =fopen(CONFIG_NAME, "rb");
    std::cout << "Reading config" << std::endl;
    if (!conf) {
        return false;
    }
    Document doc;
    char readBuffer[10000];
    FileReadStream in(conf, readBuffer, sizeof(readBuffer));
    doc.ParseStream(in);
    fclose(conf);
    std::cout << "Gonna check validity" << std::endl;
    if (checkValidity(&doc)) setConf(&doc);
    else
    {
        makeConf();
        return false;
    }
    return true;
}

bool WebConf::checkValidity(Document *doc)
{
    bool valid = true;
    if (!doc->IsObject()) valid = false;
    Value::MemberIterator enabled = doc->FindMember("enabled");
    if (enabled == doc->MemberEnd()) valid = false;
    if (!enabled->value.IsBool()) valid = false;
    std::cout << "I wonder if were valid " << valid << std::endl;
    return valid;
}

void WebConf::setConf(Document *doc)
{
    std::cout << "Setting from the config file" << std::endl;
    serviceEnabled= doc->FindMember("enabled")->value.GetBool();
    for (const char* i : keys)
    {
        if (doc->HasMember(i))
        {
            auto thisValue = doc->FindMember(i);
            if (thisValue->value.IsString())
            {
               configuration.push_back({i, thisValue->value.GetString()});
            }
        }
    }
}

void WebConf::makeConf()
{
    std::cout << "MAking a new configuration file" << std::endl;
    serviceEnabled = false;
    std::stringstream defaultString;
    defaultString << "\{\n\"enabled\": false";
    for (auto i : keys)
    {
        defaultString << ",\n\"" << i << "\": \"\"";
    }
    defaultString << "}";
	remove (CONFIG_NAME);
	std::ofstream out(CONFIG_NAME);
	out << defaultString.str();
	out.close();
}
