#ifndef _PACKINST_H
#define _PACKINST_H


#include <vector>
#include <cstring>
#include <fstream>
class packinst{
      
      public:
             packinst(){loc=0;}
             ~packinst(){}
             std::string getName() const {return name;}
             std::string getVersion() const {return version;}

             // Wget use is deprecated although never implemented
             std::string getWget() const {return wget;}
             std::string getArchiveName() const {return archivename;}
             std::string getArchiveUrl() const {return archiveurl;}
        
             std::string getConfig() const {return config;}
             std::string getMake() const {return make;}
             std::string getMakeInst() const {return makeinst;}
             int getLoc() const {return loc;}
             bool getNextDep(std::string&);
             void removeDep(const int deppos){deps.erase(deps.begin()+deppos-1);loc--;}
             void setName(const std::string pname){name=pname;}
             void setVersion(const std::string pversion){version=pversion;}

             // Wget space is being used for archive url/name
             void setWget(const std::string pwget)
             {
                wget=pwget;int pos;
                if ((pos = wget.find('/')) != wget.npos){
                        archiveurl=wget;
                        archivename = wget.substr(pos);
                }else{
                        archiveurl ="";
                        archivename = wget;
                }
             }
             void setConfig(const std::string pconfig){config=pconfig;}
             void setMake(const std::string pmake){make=pmake;}
             void setMakeInst(const std::string pmakeinst){makeinst=pmakeinst;}
             void setDeps(const std::vector<std::string> pdeps){deps=pdeps;}
             void addDep(const std::string pdep){deps.push_back(pdep);}   
      
      private:
              std::string name,version,wget,config,make,makeinst, archiveurl, archivename;
              std::vector<std::string> deps;
              int loc;
      
      };

packinst getPackage(const char*);
//packinst getPackage(const std::string location){return getPackage(location.c_str());}


#endif

