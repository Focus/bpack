#ifndef _STORAGE_H
#define _STORAGE_H

#include <vector>

std::vector<package> getInstalledPackages(const char*);
std::vector<std::string> loadLocation(const std::string);
int separate(const std::string,std::vector<std::string> &,const int);
void printPackages(const std::vector<package>);

#endif


