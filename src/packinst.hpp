#ifndef _PACKINST_H
#define _PACKINST_H


#include <vector>
#include <cstring>
#include <fstream>
#include "version.hpp"
#include "storage.hpp"
class packinst{
      
      public:
             packinst(){loc=0;}
             ~packinst(){}
             std::string getName() const {return name;}
             std::string getVersion() const {return ver.asString();}

             // Wget use is deprecated although never implemented
		//We clearly disagree here, look at installscript.cpp for your TODO...I use getWet there
             std::string getWget() const {return wget;}
		//What are all these?
             std::string getArchiveName() const {return archivename;}
             std::string getArchiveUrl() const {return archiveurl;}

             std::string getPreInstall() const {return preinstall;}
             std::string getConf() const {return conf;}
             std::string getConfFile() const {return conffile;}
             std::string getPostInstall() const {return postinstall;}
             std::string getConfig() const {return config;}
             std::string getMake() const {return make;}
             std::string getMakeInst() const {return makeinst;}
             int getLoc() const {return loc;}
             bool getNextDep(std::string&);
	
             void setPreInstall(const std::string pinst){preinstall=pinst;}
             void setConf(const std::string pname){conf=pname;}
             void setConfFile(const std::string pname){conffile=pname;}
             void setPostInstall(const std::string pname){postinstall=pname;}
             void removeDep(const int deppos){deps.erase(deps.begin()+deppos-1);loc--;}
             void setName(const std::string pname){name=pname;}
             void setVersion(const std::string pversion){ver=pversion;}
	     void setVersion(const version vers){ver=vers;}
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
              std::string name,wget,config,make,makeinst, archiveurl, archivename,conffile,conf,preinstall,postinstall;
	      version ver;
              std::vector<std::string> deps;
              int loc;
      
      };

packinst getPackage(std::string);
//packinst getPackage(std::string location){return getPackage(location.c_str());}


#endif

