#ifndef _PACKAGE_H
#define _PACKAGE_H

#include <string>
#include "packinst.hpp"
#include <vector>

class package
{
      
      public:
             package(){scan=0;}
             package(const std::string);
             package(const std::string , const std::vector<std::string>);
             package(const packinst inst);
             ~package(){}
             std::string getName() const {return name;}
             std::string getVersion() const {return ver.asString();}
             version Version() const {return ver;}
             std::vector<std::string> getLocations() const {return locations;}
             bool getScan() const {return scan;}
             void setName(const std::string pname){name=pname;}
             void setVersion(const std::string pname){ver=pname;}
             void setLocations(const std::vector<std::string> plocations){locations=plocations;}
             void addLocation(const std::string plocation){locations.push_back(plocation);}
             void setScan(const bool pscan){scan=pscan;}
             bool write();
             
      private:
              std::string name;
              version ver;
              std::vector<std::string> locations;
              bool scan;
};

std::vector<package> getInstalledPackages(const char*);
void printPackages(const std::vector<package>);

#endif



