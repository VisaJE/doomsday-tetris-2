-- premake5.lua
workspace "DoomsdayTetris"
   configurations { "Debug", "Release" }

project "DoomsdayTetris-2"
   kind "WindowedApp"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"
   includedirs {"../rapidjson/include",
        "../../Downloads/SDL2-2.0.12/i686-w64-mingw32/include",
        "../../Downloads/SDL2-2.0.12/i686-w64-mingw32/include/SDL2",
        "../../Downloads/SDL2_ttf-2.0.15/x86_64-w64-mingw32/include"}
    links {"../../Downloads/SDL2-2.0.12/i686-w64-mingw32/lib/SDL2main",
            "../../Downloads/SDL2-2.0.12/i686-w64-mingw32/lib/SDL2",
            "../../Downloads/SDL2_ttf-2.0.15/i686-w64-mingw32/lib/SDL2_ttf"}

   files { "**.h", "**.cpp" }

   filter "configurations:Debug"
      defines { "DEBUG", "NPOSTGRES"}
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG", "NPOSTGRES"}
      optimize "On"
