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
#include "error.hpp"
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
		locs=com2vec(search(path,""));
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
			err("Erase could not delete "+path,0,1);
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

int removePack(string);

void removeHanging(){
	string name;
	version ver;
	int rem,rem2;
	vector<string> del;
	package pack;
	vector<string> *packs = new vector<string>;
	vector<string> *save = new vector<string>;
	*packs = com2vec(search(Config::getPacklistPath()));
	*save = read(Config::getInstallDir()+"list");
	for(int i=0;i<packs->size();i++){
		rem = 1;
		name = (*packs)[i];
		sepVer(name,ver);
		for(int j=0;j<save->size();j++){
			if(!strcmp(name.c_str(),(*save)[j].c_str())){
				rem = 0;
				break;
			}
		}
		if(rem){
			rem2 = 0;
			pack=getInstalledPackage(name);
			for(int k=0;k<pack.getDeps().size();k++){
				if(search(Config::getPacklistPath(),pack.getDeps()[i])==""){
					rem2=1;
					break;
				}
			}
			if(!rem2)
				del.push_back(name);
		}
	}
	if(del.size()<=0){
		cout<<"No hanging packages detected."<<endl;
		return;
	}
	cout<<"   Packages to be removed:\n\n";
	for(int i=0;i<del.size();i++)
		cout<<del[i]<<"  ";
	cout<<"\n";

	if(ask("Are you sure you wish to remove these?")){
		for(int i=0;i<del.size();i++)
			removePack(del[i]);
	}
}


//Removes the package
int removePack(string packname){
	if(!strcmp(packname.c_str(),"hanging")){
		removeHanging();
		return 1;
	}
	vector<string> *packages=new vector<string>;
	package *pack=new package;
	string name,name2;
	version ver,ver2;
	*packages=com2vec(search(Config::getPacklistPath().c_str()));
	int index=-1;
	for(int i=0; i< packages->size();i++){
		name=(*packages)[i];
		sepVer(name,ver);
		ver="0.0.0";
		if(!strcmp(name.c_str(),packname.c_str())){
			cout<<"\nPackage "<<packname<<" found! \nRemoving package..."<<endl;
			index=i;
			break;
		}
	}

	if(index<0){
		err("No package named "+packname+" was found.");
		return 0;
	}
	*pack=getInstalledPackage((*packages)[index]);
	if(pack->getScan())
		err("This package was scanned, are you really sure you want to remove this?",1);
	for(int i=0; i<pack->getDeps().size();i++){
		name=pack->getDeps()[i];
		ver="0.0.0";
		sepVer(name,ver);
		for(int j=0;j<packages->size();j++){
			name2=(*packages)[j];
			ver2="0.0.0";
			sepVer(name2,ver2);
			if(!strcmp(name.c_str(),name2.c_str()) && ver<=ver2){
				err("Package "+name+" depends on "+packname,0,0);
				delete packages;
				delete pack;
				return 0;
			}
		}
	}
	cout<<"\nErasing "<<packname<<"..."<<endl;
	if(erase(pack->getLocations())){
		pack->remove();
		removeLine(pack->getName(),Config::getInstallDir()+"list");
		delete packages;
		delete pack;
		return 1;
	}
	else
		err("Removing the files have failed. Package not erased!",0);
	delete packages;
	delete pack;
	return 0;
}
