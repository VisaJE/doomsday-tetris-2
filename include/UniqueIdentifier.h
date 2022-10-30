#pragma once

#include <iostream>

typedef std::string Uid;
namespace tet
{
class UniqueIdentifier
{
public:
    static Uid getUid(std::string name);
    static bool compare(Uid f, Uid s);

};}

