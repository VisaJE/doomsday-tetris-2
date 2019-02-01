#include <iostream>
#include "UniqueIdentifier.h"
#include <random>
#include <time.h>
#include <stdlib.h>

 Uid tet::UniqueIdentifier::getUid(std::string name)
{
    srand (time(NULL));
    int idNum = rand()*3*(time(NULL)%1000000);
    char buf[10];
    for (int i = 0; i < 10; i++)
    {
       buf[i] = (char)((name.at(i%name.size()) + (int)(idNum / pow(10, i))%10 + 1)%26 + 65);
    }
    //std::cout << "Created identifier " << buf << std::endl;
    return (std::string) buf;
}
 bool tet::UniqueIdentifier::compare(Uid f, Uid s)
{
    return !f.compare(s);
}
