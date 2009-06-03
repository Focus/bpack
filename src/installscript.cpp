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


/*********************************************************************************

This function is God. Bow down to it!

*********************************************************************************/
#include <cstring>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "packinst.hpp"
#include "version.hpp"
#include "package.hpp"
#include "config.hpp"
#include "search.hpp"
#include "remove.hpp"
#include "storage.hpp"
#include "error.hpp"
extern "C"{
#include "qhttp.h"
}

using namespace std;


string tarName(string url){
	
	int pos=url.find_last_of("/");
	if( pos<=0 )
		return "";
	return url.substr(pos+1);
}

bool installScript(packinst inst, int bail=-1)
{
    // get variables
  	string  tardir,tar, logroot,hijack;
    tardir = Config::getTarballDir();
    logroot = Config::getLogDir();
    hijack=Config::getLib();
    int result;
	
	
	//Check if the tarball is there, if not grab it!
	vector<string> *tars=new vector<string>;
	vector<string> *tars2=new vector<string>;
	*tars=loadLocation(search(tardir,inst.getName()+"-"+ inst.getVersion()+".tar.*"));
	*tars2=loadLocation(search(tardir,tarName(inst.getWget())));
	if(tars2->size()>0)
		tar=(*tars2)[0];
	else if(tars->size()<=0){
		cout<<"\nPackage not found locally, I will download it now"<<endl;

		if(inst.getWget()==""){
			err("No URL specified to download!",2);
		}

		
		if(wget(inst.getWget().c_str(),tardir.c_str(),0,LOGNONE))
		{
			err("Download failed!",1);
		}
		
		tar=tarName(inst.getWget());
	
	}
	else
		tar=(*tars)[0];
	delete tars,tars2;
	//Make a clean directory by removing previous extracts
	erase(tardir+inst.getName()+"-"+inst.getVersion());
	//Unpack the tar
	if(chdir(tardir.c_str())!=0){
		err("Cannot cd to "+tardir,2,1);
	}
	if(system( ("tar xf "+tar).c_str())!=0){
		err("Unable to unpack the tarball!",2);
	}

	if(chdir( (tardir+inst.getName()+"-"+inst.getVersion()).c_str())!=0){
		err("Cannot cd to "+tardir,0,1);
		return 0;
	}
	//Run preinstall commands
	if(inst.getPreInstall()!=""){
		if( system( inst.getPreInstall().c_str() )!=0)
			err("Pre-install command has failed!");
	}

	//Run config, make and make install...
	if(strcmp(inst.getConfig().c_str(),"no")){
		if(system( ("./configure "+inst.getConfig()).c_str())!=0)
			err("Configuration failed",2);
	}
	if(strcmp(inst.getMake().c_str(),"no")){
		if(system( ("make "+inst.getMake()).c_str())!=0)
			err("Make failed",2);
	}
	//Do we need to remove the old package?
	if(bail==1){
		cout<<"Removing version "<<inst.getVersion()<<endl;
		if(!removePack(inst.getName()))
			err("Removing old package failed!",1);
		
	}
	
	//Lets hijack this ride! (this will log activities in /tmp/hijack_log.txt)
	erase("/tmp/hijack_log.txt");
	setenv("LD_PRELOAD",hijack.c_str(),1);
	//And install
	if(strcmp(inst.getMakeInst().c_str(),"no")){
		if(system( ("make install "+inst.getMakeInst()).c_str())!=0)
			err("Make install failed",1);
	}
	//Run postinstall command
	if(inst.getPostInstall()!=""){
		if( system(inst.getPostInstall().c_str()) !=0)
			err("Post-install command has failed!");
	}
	
	//Write the config file
	if(inst.getConf()!=""){
		if(inst.getConfFile()=="")
			err("I have a config but no file to write it to!");
		else if(!write(inst.getConf(),inst.getConfFile()))
			err("Cannot write the configuration!! Don't worry, your package has been installed. Configure manually!");
	}
	return 1;

}
