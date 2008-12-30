#ifndef _PACKINST_H
#define _PACKINST_H


#include <vector>
#include <string>
class packinst{
      
      public:
             packinst(){loc=0;}
             ~packinst(){}
             std::string getName() const {return name;}
             std::string getVersion() const {return version;}
             std::string getGit() const {return git;}
             std::string getConfig() const {return config;}
             std::string getMake() const {return make;}
             std::string getMakeInst() const {return makeinst;}
             int getLoc() const {return loc;}
             bool getNextDep(std::string&);
             void removeDep(const int deppos){deps.erase(deps.begin()+deppos);}
             void setName(const std::string pname){name=pname;}
             void setVersion(const std::string pversion){version=pversion;}
             void setGit(const std::string pgit){git=pgit;}
             void setConfig(const std::string pconfig){config=pconfig;}
             void setMake(const std::string pmake){make=pmake;}
             void setMakeInst(const std::string pmakeinst){makeinst=pmakeinst;}
             void setDeps(const std::vector<std::string> pdeps){deps=pdeps;}
             void addDep(const std::string pdep){deps.push_back(pdep);}   
      
      private:
              std::string name,version,git,config,make,makeinst;
              std::vector<std::string> deps;
              int loc;
      
      };



#endif

