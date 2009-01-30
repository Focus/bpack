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

using namespace std;

bool installScript(packinst inst)
{
     	// get variables
  	string  tardir,tar, logroot,hijack;
     	tardir = Config::getTarballDir();
     	logroot = Config::getLogDir();
     	hijack=Config::getLib();
     	int result;
	

	//Check if the tarball is there, if not grab it!
	tar=search(tardir,inst.getName()+"-"+ inst.getVersion()+".tar.*");
	if(tar==""){
		cout<<"\nPackage not found locally, I will download it now"<<endl;

		if(inst.getWget()==""){
			cerr<<"\n\nNo URL specified to download!"<<endl;
			return 0;
		}

		if(system( ("cd "+tardir+" && wget "+inst.getWget()).c_str())!=0)
		{
			cerr<<"\n\nDownload failed!"<<endl;
			return 0;
		}

	
	}
	tar=search(tardir,inst.getName()+"-"+inst.getVersion()+".tar.*");
	//Make a clean directory by removing previous extracts
	
	erase(tardir+inst.getName()+"-"+inst.getVersion());
	//Unpack the tar
	
	if(system( ("cd "+tardir+" && tar xf "+tar).c_str())!=0){
		cerr<<"\n\nUnable to unpack the tarball!"<<endl;
		return 0;
	}

	//Run config, make and make install...
	if(system( ("cd "+tardir+inst.getName()+"-"+inst.getVersion()+" && ./configure "+inst.getConfig()).c_str())!=0){
		cerr<<"\n\nConfiguration failed"<<endl;
		return 0;
	}

	if(system( ("cd "+tardir+inst.getName()+"-"+inst.getVersion()+" && make "+inst.getMake()).c_str())!=0){
		cerr<<"\n\nMake failed"<<endl;
		return 0;
	}

	//Lets hijack this ride! (this will log activities in /tmp/hijack_log.txt)
	setenv("LD_PRELOAD",hijack.c_str(),1);
	//And install
	
	if(system( ("cd "+tardir+inst.getName()+"-"+inst.getVersion()+" && make install "+inst.getMakeInst()).c_str())!=0){
		cerr<<"\n\nMake install failed"<<endl;
		return 0;
	}

	return 1;

}
