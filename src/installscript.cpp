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

#include <iostream>
#include <cstring>
#include <sys/stat.h>

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
#define MIN(x,y) x < y ? x : y
#define MAX(x,y) x > y ? x : y
using namespace std;

//Changes the --prefix option to use the fakeroot method
string fakePrefix(const string config){
	int pos;
	if( (pos=config.find("--prefix"))==string::npos)
		return config;
	string ret=config.substr(0,pos);
	ret=ret+"--prefix="+Config::getInstallDir()+"fakeroot"+config.substr(pos+9);
	return ret;
}

//Returns 1 on success
bool configuration(string config,string location){


	vector<string> *add=new vector<string>;
	vector<string> *rem=new vector<string>;
	string *temp=new string;
	int pos;
	*temp=config;
	//First get the things we need to add
	while((pos=temp->find('+'))!=string::npos && pos!=temp->length()-1){
		*temp=temp->substr(pos+1);
		if(temp->find("+")==string::npos && temp->find("-")==string::npos)//this must be the last item
			add->push_back(*temp);
		else if(temp->find("+")==string::npos)//Get the bit in between the + and the -
			add->push_back(temp->substr(0,temp->find("-")-1));
		else if(temp->find("-")==string::npos)//Get bit in between the + and +
			add->push_back(temp->substr(0,temp->find("+")-1));
		else 
			add->push_back(temp->substr(0,MIN(temp->find_first_of("+"),temp->find_first_of("-"))-1));


	}
	*temp=config;
	//Now the things we need to remove
	while((pos=temp->find("-"))!=string::npos && pos!=temp->length()-1){
		*temp=temp->substr(pos+1);

		if(temp->find("+")==string::npos && temp->find("-")==string::npos)//this must be the last item
			rem->push_back(*temp);
		else if(temp->find("+")==string::npos)//Get the bit in between the - and the -
			rem->push_back(temp->substr(0,temp->find("-")-1));
		else if(temp->find("-")==string::npos)//Get bit in between the - and +
			rem->push_back(temp->substr(0,temp->find("+")-1));
		else 
			rem->push_back(temp->substr(0,MIN(temp->find("+"),temp->find("-"))-1));
	}

	delete temp;

	vector<string> *current=new vector<string>;
	*current=read(location);
	if(current->size()<=0){
		delete current;
		string *newconfig=new string;
		*newconfig="";
		for(int i=0;i<add->size();i++)
			*newconfig=*newconfig+(*add)[i]+"\n";
		if(write(*newconfig,location)){
			delete newconfig;
			return 1;
		}
		delete newconfig;
		return 0;
	}

	for(int i=0;i<current->size();i++){
		for(int j=0;j<rem->size();j++){
			if(!strncmp((*current)[i].c_str(),(*rem)[i].c_str(),(*rem)[j].length())){
				current->erase(current->begin()+i);	
				i--;
				break;
			}
		}
	}
	delete rem;
	for(int i=0;i<add->size();i++)
		current->push_back((*add)[i]);

	delete add;
	string *newconf=new string;
	*newconf="";
	for(int i=0;i<current->size();i++)
		*newconf=*newconf+(*current)[i]+"\n";
	delete current;
	if(write(*newconf,location,1)){
		delete newconf;
		return 1;
	}
	delete newconf;
	return 0;
}


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
	*tars=com2vec(search(tardir,inst.getName()+"-"+ inst.getVersion()+".tar.*"));
	*tars2=com2vec(search(tardir,tarName(inst.getWget())));
	if(tars2->size()>0)
		tar=(*tars2)[0];
	else if(tars->size()<=0){
		cout<<"\nPackage not found locally, I will download it now"<<endl;

		if(inst.getWget()==""){
			err("No URL specified to download!",2);
		}


		if(wget(inst.getWget().c_str(),tardir.c_str(),0,LOGNONE,1))
			err("Download failed!",1);

		tar=tarName(inst.getWget());

	}
	else
		tar=(*tars)[0];
	delete tars;
	delete tars2;
	//Make a clean directory by removing previous extracts
	string tempdir="/tmp/bpack-build/"+inst.getName()+"-"+inst.getVersion();
	erase(tempdir);
	//Any error in the one below will be caught by the if statement below, no need to care about /dir existing errors
	mkdir("/tmp/bpack-build/",S_IWUSR);
	if(mkdir(tempdir.c_str(),S_IWUSR) != 0)
			err("Unable to create "+tempdir,2,1);
	//Unpack the tar
	if(system( ("tar xf "+tardir+tar+" -C "+tempdir).c_str())!=0){
		err("Unable to unpack the tarball!",2);
	}

	//Find the folder the damn thing extracted and cd to it
	vector<string> *tfiles = new vector<string>;
	*tfiles=com2vec(search(tempdir));
	if(tfiles->size() == 0)
			err("There doesn't seem to be a folder extracted!",2);
	if(chdir( (tempdir+"/"+(*tfiles)[0]).c_str())!=0 ){
			err("Cannot cd to "+tempdir+(*tfiles)[0],2,1);
	}
	delete tfiles;
	//Aplly the patches
	vector<string> patches;
	for(int i=0;i<inst.getPatches().size();i++){
		if(inst.getPatches()[i].find_last_of("/")==string::npos){
			err("Patch url "+inst.getPatches()[i]+" looks bad, skipping",0);
			continue;
		}
		if(wget(inst.getPatches()[i].c_str(),tardir.c_str(),inst.getPatches()[i].substr(inst.getPatches()[i].find_last_of("/")+1).c_str(),LOGNONE,1)){
			err("Patch url "+inst.getPatches()[i]+" could not be downloaded, skipping",0);
			continue;
		}
		patches.push_back(inst.getPatches()[i].substr(inst.getPatches()[i].find_last_of("/")+1));
	}
	for(int i=0;i<patches.size();i++){
		if( system( ("patch -Np1 -i ../"+patches[i]).c_str())!=0)
			err("Patch "+patches[i]+" could not be applied",0);
		else
			cout<<"Patch "+patches[i]+" successful"<<endl;
	}
	//Run preinstall commands
	if(inst.getPreInstall()!=""){
		if( system( inst.getPreInstall().c_str() )!=0)
			err("Pre-install command has failed!");
	}

	//Set compiler flags
	if(Config::getCflags().length()>0)
		setenv("CFLAGS",Config::getCflags().c_str(),1);
	if(Config::getCxxflags().length()>0)
		setenv("CXXFLAGS",Config::getCxxflags().c_str(),1);

	//Run config, make and make install...
	if(strcmp(inst.getConfig().c_str(),"no")){
		string *conf=new string;
		*conf=inst.getConfig();
		macro(*conf);
		if(!inst.getFakeroot()){
			if(system( ("./configure "+*conf).c_str())!=0)
				err("Configuration failed",2);
		}
		else{
			string *fakeroot=new string;
			*fakeroot=fakePrefix(*conf);
			if(system( ("./configure "+*fakeroot).c_str())!=0)
				err("Configuration failed",2);
			delete fakeroot;
		}

		delete conf;
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
	if(!inst.getFakeroot()){
		erase("/tmp/hijack_log.txt");
		setenv("LD_PRELOAD",hijack.c_str(),1);
	}
	else{
		erase(Config::getInstallDir()+"fakeroot/");
		mkdir( (Config::getInstallDir()+"fakeroot/").c_str(),766);
	}
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
		else if(!configuration(inst.getConf(),inst.getConfFile()))
			err("Cannot write the configuration!! Don't worry, your package has been installed. Configure manually!");
	}
	return 1;

}
