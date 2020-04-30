#pragma once

#include "Log.h"

#include <iostream>

#include <direct.h>
#include <cstdlib>

using namespace std;
namespace tet
{
class Paths
{
    public:
        static std::string configPath()
        {
            auto confDir = configDir();
            confDir.append( "/config.json");
            LOG("Config at %s\n", confDir.c_str() );
            return confDir;
        }

        static std::string serverConfigPath()
        {
            auto confDir = configDir();
            confDir.append("/server_config.json");
            LOG("Server config at %s\n", confDir.c_str() );
            return confDir;
        }

        static std::string highscorePath()
        {
            auto dir = dataDir();
            dir.append("/.hs.json");
            return dir;
        }

        static std::string findFont(const char* name)
        {
            std::string fontFile = getenv("WINDIR");
            fontFile.append("/Fonts/");
            fontFile.append(name);
            fontFile.append(".ttf");
            return fontFile;
        }
    private:

        static bool pathExists(const std::string &s)
        {
          return false;
        }

        static void createDirs(std::string path)
        {
            if (!pathExists(path))
            {
                if (_mkdir(path.c_str()) != 0)
                {
                    LOG("Didnt create config directory \n");
                }
            }
        }

        static std::string configDir()
        {
            std::string confDir = getenv("APPDATA");
            confDir.append("/DoomsdayTetris");

            createDirs(confDir);
            return confDir;
        }

        static std::string dataDir()
        {
            return configDir();
        }
};
}
