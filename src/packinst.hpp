/*
 * 		Copyright 2009 (c) Bpack- Bati Sengul <batieatworld@gmail.com>
 * 							      Mike Bush <mpbush@gmail.com>
 * 
 * 					http://www.bpack.co.uk
 * 
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */


#ifndef _PACKINST_H
#define _PACKINST_H


#include <vector>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include "config.hpp"
#include "version.hpp"
#include "storage.hpp"

class packinst{
      
      public:
             packinst(){loc=0;meta=0;}
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
	     std::vector<std::string> getDeps(){return deps;}
             int getLoc() const {return loc;}
             bool getNextDep(std::string&);
	     bool getMeta(){return meta;}
	
             void setPreInstall(const std::string pinst){preinstall=pinst;}
             void setConf(const std::string pname){conf=pname;}
             void setConfFile(const std::string pname){conffile=pname;}
             void setPostInstall(const std::string pname){postinstall=pname;}
             void removeDep(const int deppos){deps.erase(deps.begin()+deppos-1);loc--;}
             void setName(const std::string pname){name=pname;}
             void setVersion(const std::string pversion){ver=pversion;}
	     void setVersion(const version vers){ver=vers;}
	     void setMeta(std::string value){
		     if(!strcmp(value.c_str(),"1") || !strcmp(value.c_str(),"yes") || !strcmp(value.c_str(),"y"))
			     meta=1;
		     else
			     meta=0;
	     }
	     void setMeta(bool value){meta=value;}
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
	     void addDeps(const std::vector<std::string> pdeps){ for(int i=0;i<pdeps.size();i++) deps.push_back(pdeps[i]);}
      
      private:
              std::string name,wget,config,make,makeinst, archiveurl, archivename,conffile,conf,preinstall,postinstall;
	      version ver;
              std::vector<std::string> deps;
              int loc;
	      bool meta;
      
      };

packinst getPackage(std::string);

#endif

