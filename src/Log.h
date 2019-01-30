#pragma once

#include <stdio.h>
#ifdef DEBUG
    #define LOG(args...) printf (args)
#else
    #define LOG(args...) do{}while(0)
#endif
