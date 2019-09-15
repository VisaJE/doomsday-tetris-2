#pragma once

#include "Log.h"

#include <iostream>
#include <fontconfig/fontconfig.h>

#include <sys/stat.h>
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
            auto confDir = configDir();
            confDir.append("/.hs.json");
            return confDir;
        }

        static std::string findFont(const char* name)
        {
            std::string fontFile = "";
            FcConfig* config = FcInitLoadConfigAndFonts();

            // configure the search pattern,
            // assume "name" is a std::string with the desired font name in it
            FcPattern* pat = FcNameParse((const FcChar8*)(name));
            FcConfigSubstitute(config, pat, FcMatchPattern);
            FcDefaultSubstitute(pat);

            // find the font
            FcResult fcResult;
            FcPattern* font = FcFontMatch(config, pat, &fcResult);
            if (font)
            {
               FcChar8* file = NULL;
               if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch)
               {
                  // save the file to another std::string
                  fontFile = (char*)file;
               }
               FcPatternDestroy(font);
            }

            FcPatternDestroy(pat);
            return fontFile;
        }
    private:

        static bool pathExists(const std::string &s)
        {
          struct stat buffer;
          return (stat (s.c_str(), &buffer) == 0);
        }

        static void createDirs(std::string path)
        {
            if (!pathExists(path))
            {
                std::string cmd = "mkdir -p ";
                cmd.append(path);
                if ( system(cmd.c_str())!= 0)
                {
                    LOG("Creating config directory failed\n");
                }
                if (!pathExists(path.c_str()))
                {
                    LOG("%s still not found\n", path.c_str());
                }
            }
        }

        static std::string configDir()
        {
            const char* overrideDir = std::getenv("XDG_CONFIG_HOME");
            if (overrideDir) LOG("Found XDG_CONFIG_HOME %s\n", overrideDir);

            std::string home = std::getenv("HOME");
            std::string confDir = overrideDir ? overrideDir : home + "/.config/";

            if (confDir.at(confDir.length() - 1) != '/') confDir.append("/");
            confDir.append( "DoomsdayTetris");

            createDirs(confDir);
            return confDir;
        }

};
}
