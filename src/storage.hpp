#ifndef _STORAGE_H
#define _STORAGE_H

#include <vector>


std::vector<std::string> loadLocation(const std::string);
int separate(const std::string,std::vector<std::string> &,const int);
bool write (const std::string,const std::string);

#endif


