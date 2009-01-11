#include <vector>
#include <cstring>
#include "packinst.hpp"

//Get the next dependency
bool packinst::getNextDep(std::string &dep)
{
     if(loc>=deps.size())
     return 0;
     else
     {
     dep=deps[loc];
     loc++;
     return 1;
     }
}

