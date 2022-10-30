cmake_minimum_required(VERSION 3.8.0 FATAL_ERROR)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()


project(DoomsdayTetris-2 VERSION 1.3 LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
if (WIN32)
    add_compile_definitions("WIN32")
    set(CMAKE_CXX_FLAGS_RELEASE "-Ox")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()


list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)

set(tet_ROOT ${CMAKE_CURRENT_LIST_DIR})

include_directories(
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

find_package(SDL2 REQUIRED)
find_package(SDL2_TTF REQUIRED)

set(tet_INCLUDES
    ${tet_ROOT}/include/Block.h
    ${tet_ROOT}/include/BoxTexture.h
    ${tet_ROOT}/include/Conf.h
    ${tet_ROOT}/include/Events.h
    ${tet_ROOT}/include/GlobalHighscore.h
    ${tet_ROOT}/include/Grid.h
    ${tet_ROOT}/include/Highscorer.h
    ${tet_ROOT}/include/Paths.h
    ${tet_ROOT}/include/Screen.h
    ${tet_ROOT}/include/SqlConn.h
    ${tet_ROOT}/include/StaticBlock.h
    ${tet_ROOT}/include/UberBlockifier.h
    ${tet_ROOT}/include/UniqueIdentifier.h
    ${tet_ROOT}/include/WebConf.h
    ${tet_ROOT}/include/Log.h
)
set(tet_SOURCES
    ${tet_ROOT}/src/Block.cpp
    ${tet_ROOT}/src/BoxTexture.cpp
    ${tet_ROOT}/src/Conf.cpp
    ${tet_ROOT}/src/Events.cpp
    ${tet_ROOT}/src/GlobalHighscore.cpp
    ${tet_ROOT}/src/Grid.cpp
    ${tet_ROOT}/src/Highscorer.cpp
    ${tet_ROOT}/src/main.cpp
    ${tet_ROOT}/src/Screen.cpp
    ${tet_ROOT}/src/SqlConn.cpp
    ${tet_ROOT}/src/StaticBlock.cpp
    ${tet_ROOT}/src/UberBlockifier.cpp
    ${tet_ROOT}/src/UniqueIdentifier.cpp
    ${tet_ROOT}/src/WebConf.cpp
)

add_executable(doomsday-tetris ${tet_SOURCES} ${tet_INCLUDES})

target_compile_features(doomsday-tetris PUBLIC cxx_std_17)

target_link_libraries(doomsday-tetris
    SDL2
    SDL2_ttf
    pq
    fontconfig
    )

install(TARGETS doomsday-tetris EXPORT doomsday-tetris-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
