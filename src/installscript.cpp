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
			cerr<<"\n\nNo URL specified to download!"<<endl;
			return 0;
		}

		
		if(wget(inst.getWget().c_str(),tardir.c_str(),0,LOGMULTI))
		{
			cerr<<"\n\nDownload failed!"<<endl;
			return 0;
		}
		
		tar=tarName(inst.getWget());
	
	}
	else
		tar=(*tars)[0];
	delete tars,tars2;
	//Make a clean directory by removing previous extracts
	erase(tardir+inst.getName()+"-"+inst.getVersion());
	//Unpack the tar
	if(system( ("cd "+tardir+" && tar xf "+tar).c_str())!=0){
		cerr<<"\n\nUnable to unpack the tarball!"<<endl;
		return 0;
	}


	//Run preinstall commands
	if(inst.getPreInstall()!=""){
		if( system(  ("cd "+tardir+inst.getName()+"-"+inst.getVersion()+" && "+inst.getPreInstall() ).c_str() )!=0)
			cerr<<"\nPre-install command has failed!"<<endl;
	}

	//Run config, make and make install...
	if(strcmp(inst.getConfig().c_str(),"no")){
		if(system( ("cd "+tardir+inst.getName()+"-"+inst.getVersion()+" && ./configure "+inst.getConfig()).c_str())!=0){
			cerr<<"\n\nConfiguration failed"<<endl;
			return 0;
		}
	}
	if(strcmp(inst.getMake().c_str(),"no")){
		if(system( ("cd "+tardir+inst.getName()+"-"+inst.getVersion()+" && make "+inst.getMake()).c_str())!=0){
			cerr<<"\n\nMake failed"<<endl;
			return 0;
		}
	}
	//Do we need to remove the old package?
	if(bail==1){
		cout<<"Removing version "<<inst.getVersion()<<endl;
		if(!removePack(inst.getName())){
			cerr<<"Removing old package failed!\nI will carry on"<<endl;
		}
	}
	
	//Lets hijack this ride! (this will log activities in /tmp/hijack_log.txt)
	erase("/tmp/hijack_log.txt");
	setenv("LD_PRELOAD",hijack.c_str(),1);
	//And install
	if(strcmp(inst.getMakeInst().c_str(),"no")){
		if(system( ("cd "+tardir+inst.getName()+"-"+inst.getVersion()+" && make install "+inst.getMakeInst()).c_str())!=0){
			cerr<<"\n\nMake install failed"<<endl;
			return 0;
		}
	}
	//Run postinstall command
	if(inst.getPostInstall()!=""){
		if( system(  ("cd "+tardir+inst.getName()+"-"+inst.getVersion()+" && "+inst.getPostInstall() ).c_str() ) !=0)
			cerr<<"\nPost-install command has failed!"<<endl;
	}
	
	//Write the config file
	if(inst.getConf()!=""){
		if(inst.getConfFile()=="")
			cerr<<"\nI have a config but no file to write it to!"<<endl;
		else if(!write(inst.getConf(),inst.getConfFile()))
			cerr<<"\nCannot write the configuration!! Don't worry, your package has been installed. Configure manually!"<<endl;
	}
	return 1;

}
