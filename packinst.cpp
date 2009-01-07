#include <vector>
#include <string>
#include "packinst.hpp"

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

