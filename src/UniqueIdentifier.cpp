#include <iostream>
#include "UniqueIdentifier.h"
#include <random>
#include <time.h>
#include <stdlib.h>

static Uid getUid(std::string name)
{
    srand (time(NULL));
    int idNum = rand()*3*(time(NULL)%1000000);
    char buf[10];
    for (int i = 0; i < 10; i++)
    {
       buf[i] = (char)(name.at(i) + (int)(idNum / pow(10, i))%10 + 1);
    }
    return (std::string) buf;
}
static bool compare(Uid f, Uid s)
{
    return !f.compare(s);
}
