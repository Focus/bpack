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


//TODO: Soooooooo many headers...

#include <iostream>
#include <cstring>

#include "error.hpp"
#include "version.hpp"
#include "packinst.hpp"
#include "package.hpp"
#include "installscript.hpp"
#include "storage.hpp"
#include "config.hpp"
#include "remove.hpp"
#include "search.hpp"
#include "update.hpp"

using namespace std;

#define MIN(x,y) x<y? x:y


//Installs and cleans up packages
void clean(packinst pack){
	cout<<"\nAdding install paths..."<<endl;
	package *installed=new package;
	if(pack.getMeta()){
		packinst *temp=new packinst;
		*temp=getPackage(Config::getPackInstDir()+pack.getName()+"-"+pack.getVersion());
		installed->setLocations(temp->getDeps());
		delete temp;
	}
	else
		installed->setLocations(read("/tmp/hijack_log.txt"));
	erase("/tmp/hijack_log.txt");
	installed->setVersion(pack.getVersion());
	installed->setName(pack.getName());
	installed->setDeps(pack.getDeps());
	installed->save();
	delete installed;
	cout<<"Clearing up..."<<endl;
	erase(Config::getTarballDir()+pack.getName()+"-"+pack.getVersion()+"/");
	cout<<"Saving options..."<<endl;
	vector<string> plus,minus;
	if(!getPackageDeps(Config::getPackInstDir()+pack.getName()+"-"+pack.getVersion(),plus,minus,pack.getName()))
		err("\tCannot save options!",0);
	else{
		string file;
		if(plus.size()>0){
			file="+"+plus[0];
			for(int i=1;i<plus.size();i++)
				file=file+",+"+plus[i];
		}
		if(minus.size()>0){
			file=file+"-"+minus[0];
			for(int i=1;i<minus.size();i++)
				file=file+",-"+minus[i];
		}
		if(file.length()>0)
			write(file,Config::getOptionDir()+pack.getName());
	}
}


//Grabs the latest version
string greatestVer(vector<string> bushsucks, string realname){

	if(bushsucks.size()==0)
		return "";
	version tempver,highver;
	highver="0.0.0";
	string temp,ret;
	for(int i=0; i< bushsucks.size();i++){
		temp=bushsucks[i];
		depVersion(temp,tempver);
		if(tempver>highver && !strcmp(temp.c_str(),realname.c_str()))
			ret=bushsucks[i];
	}
	return ret;

}
//Recursively checks for dependencies and installs files (passes them to install script)
void install(string packname, int bail){

	packinst *packageinst=new packinst;
	version *ver=new version;
	string *location=new string;
	depVersion(packname,*ver);
	if(*ver=="0.0.0"){
		*location=greatestVer(loadLocation(search(Config::getPackInstDir(),packname+"*")),packname);
	}
	else
		*location=search(Config::getPackInstDir(),packname+"-"+ver->asString());


	if(*location==""){
//if(!dlPack(packname))
		err("Package "+packname+" not found.",2);	
	}	
	string *temp=new string;
	*temp=*location;
	depVersion(*temp,*ver);
	delete temp;
	*packageinst=getPackage(Config::getPackInstDir()+*location,packname);
	packageinst->setName(packname);
	packageinst->setVersion(*ver);
	vector<string> *installed=new vector<string>;
	*installed=loadLocation(search(Config::getPacklistPath()));
	string y;
	bool gotit;
	version depver;
	delete ver;
	delete location;
	//Check if the package is installed
	for(int i=0;i<installed->size();i++){
		y=(*installed)[i];
		depVersion(y,depver);
		if( !strcmp(y.c_str(), (packageinst->getName()).c_str()) && depver>=packageinst->getVersion()){
			cout<<"\nThe package is installed with the current or more recent version"<<endl;
			exit(0);
		}
	}

	//check for dependencies
	string inst;
	version instver;
	depver="0.0.0";
	while(packageinst->getNextDep(y)){
		depVersion(y,depver);
		for(int i=0;i<installed->size();i++){
			inst=(*installed)[i];
			instver="0.0.0";
			depVersion(inst,instver);
			gotit=0;
			if(!strcmp(inst.c_str(),y.c_str())){
				if( instver>=depver.asString()  || instver=="0.0.0" ){
					cout<<"\nPackage "<<y<<" is installed, removing from dependencies...";
					gotit=1;
					break;
				}
				else{
					cout<<"\nPackage "<<y<<" version "<<instver.asString()<<" found but version "<<depver.asString()<<" required."<<endl;
				}
			}

		}
		if (!gotit){
			cout<<"\nDependency "<<y<<" needs to be installed.";
			install(y,bail); 
		}
	}
	delete installed;
	cout<<"\nInstalling "<<packname<<"...\n";
	//Have a packinst!
	if(!packageinst->getMeta()){
		if(installScript(*packageinst,bail)){
			clean(*packageinst);
			cout<<"\n"<<packageinst->getName()<<"-"<<packageinst->getVersion()<<" is installed!"<<endl;
		}

		else
			err("Error package not installed correctly. Consult the logs and inform us.",2);
	}			
	else{
		configuration(packageinst->getConf(),packageinst->getConfFile());
		clean(*packageinst);
	}
	delete packageinst;
}

void pretend(string packname, int bail){

	packinst *packageinst=new packinst;
	version *ver=new version;
	string *location=new string;
	depVersion(packname,*ver);
	if(*ver=="0.0.0"){
		*location=greatestVer(loadLocation(search(Config::getPackInstDir(),packname+"*")),packname);
	}
	else
		*location=search(Config::getPackInstDir(),packname+"-"+ver->asString());


	if(*location==""){
//if(!dlPack(packname))
		err("Package "+packname+" not found.",2);	
	}	
	string *temp=new string;
	*temp=*location;
	depVersion(*temp,*ver);
	delete temp;
	*packageinst=getPackage(Config::getPackInstDir()+*location);
	packageinst->setName(packname);
	packageinst->setVersion(*ver);
	vector<string> *installed=new vector<string>;
	*installed=loadLocation(search(Config::getPacklistPath()));
	string y;
	bool gotit;
	version depver;
	delete ver;
	delete location;
	//Check if the package is installed
	for(int i=0;i<installed->size();i++){
		y=(*installed)[i];
		depVersion(y,depver);
		if( !strcmp(y.c_str(), (packageinst->getName()).c_str()) && depver>=packageinst->getVersion()){
			cout<<"\nThe package is installed with the current or more recent version"<<endl;
			exit(0);
		}
	}

	//check for dependencies
	string inst;
	version instver;
	depver="0.0.0";
	while(packageinst->getNextDep(y)){
		depVersion(y,depver);
		for(int i=0;i<installed->size();i++){
			inst=(*installed)[i];
			instver="0.0.0";
			depVersion(inst,instver);
			gotit=0;
			if(!strcmp(inst.c_str(),y.c_str())){
				if( instver>=depver.asString()  || instver=="0.0.0" ){
					gotit=1;
					break;
				}
			}

		}
		if (!gotit){
			pretend(y,bail); 
		}
	}
	cout<<packageinst->getName()<<"-"<<packageinst->getVersion()<<":     ";
	for(int i=0;i<packageinst->getDeps().size();i++)
		cout<<(packageinst->getDeps())[i]<<"  ";
	cout<<"\n";
	delete installed;
	delete packageinst;
}

