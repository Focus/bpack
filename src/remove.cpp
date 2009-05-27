#include <cstdio>
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include "version.hpp"
#include "package.hpp"
#include "config.hpp"
#include "storage.hpp"
#include "search.hpp"
using namespace std;
int erase(vector<string>);

//Lets bury ourselves in recursion
int erase(string path){
	struct stat buf;
	int exists=stat(path.c_str(),& buf);
	if(exists) //If the file isn't there, our work is cut out
		return 1;
	if((buf.st_mode & S_IFMT)==S_IFDIR){  //If we have a directory to delete, we need to empty it out first
		vector<string> locs;
		locs=loadLocation(search(path,""));
		//Check if we have a / at the end ( /etc will become /etc/ )
		if( strcmp((path.substr(path.size()-1)).c_str(),"/"))
			path=path+"/";
		//Pile on whats inside the directory
		for(int i=0;i<locs.size();i++)
			erase(path+locs[i]);
		//Remove the directory
		remove(path.c_str());
		}
	else{
  	if(remove(path.c_str())){
			cerr<<"Erase could not delete "<<path<<"\nReason: "<<strerror(errno)<<endl;
    		return 0;
		}
	}
	return 1;
}

int erase(vector<string> paths){
  for(int i=0;i<paths.size();i++){
    if(erase(paths[i])==0)
      return 0;
  }
  return 1;
}

//Removes the package
int removePack(string pack){
  vector<package> *packages=new vector<package>;
  vector<string> locations;
  *packages=getInstalledPackages(Config::getPacklistPath().c_str());
  int index=-1;
  for(int i=0; i< packages->size();i++){
    if(!strcmp(  (((*packages)[i]).getName()).c_str(),pack.c_str())){
      cout<<"\nPackage "<<pack<<" found! \nRemoving package..."<<endl;
      index=i;
      break;
    }
  }

  if(index<0){
    cerr<<"\nNo package named "<<pack<<" was found.\nI am done here."<<endl;
    return 0;
  }

  locations=((*packages)[index]).getLocations();
  if( ((*packages)[index]).getScan()){
    string input;
    cout<<"\n\nThis package was scanned, are you really sure you want to remove this? [y]es/[n]o:";
    cin>>input;
    if( (!strcmp( (input.substr(0,1)).c_str(),"y")) || (!strcmp(  (input.substr(0,1)).c_str(),"Y")))
      cout<<"\nYou are the boss, I will carry on."<<endl;
    else{
      cerr<<"\nUser aborted the removal process"<<endl;
      return 0;
    }
  }
  
  cout<<"\nErasing "<<pack<<"..."<<endl;
  if(erase(locations)){
	(*packages)[index].remove();
	delete packages;
	return 1;
  }
  delete packages;
  return 0;
}
