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

//Separates the foo-0.2.2 to foo and 0.2.2
//TODO: Lame name
void depVersion(string &dep, version &ver){
        int pos,temp;
        pos=dep.find_last_of("-");
        stringstream *ss= new stringstream;
        (*ss) << dep.substr(pos+1,1);
        //If we have a - and the - isnt like hello-me
        if(pos>0 && !((*ss) >> temp).fail()){
                  ver=dep.substr(pos+1,dep.size());
                  dep=dep.substr(0,pos);
	}
        delete ss;
}

//Extracts locations from the copy log
vector<string> stripCp(){
	vector<string> locs;
	ifstream *textfile=new ifstream;
	textfile->open("/tmp/hijack_log.txt");
	if(!(*textfile))
		err("I can't find the install log!",2);
	string *x=new string;
	while(!textfile->eof())
	{
		*textfile >> *x;
        	if(!textfile->eof())
			locs.push_back(*x);
	}
	erase("/tmp/hijack_log.txt");
	delete x;
	delete textfile;
	return locs;
}

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
		installed->setLocations(stripCp());
	installed->setVersion(pack.getVersion());
	installed->setName(pack.getName());
	installed->write();
	delete installed;
	cout<<"Clearing up..."<<endl;
	erase(Config::getTarballDir()+pack.getName()+"-"+pack.getVersion()+"/");
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
		if(!dlPack(packname))
			err("Package "+packname+" not found.",2);	
	}	
	string *temp=new string;
	*temp=*location;
	depVersion(*temp,*ver);
	delete temp;
	*packageinst=getPackage(Config::getPackInstDir()+*location);
	packageinst->setName(packname);
	packageinst->setVersion(*ver);
	vector<package> *installed=new vector<package>;
     	*installed=getInstalledPackages(Config::getPacklistPath().c_str());
     	string y;
	bool gotit;
	version depver;
	delete ver;
	delete location;
	//Check if the package is installed
	for(int i=0;i<installed->size();i++){
		if( !strcmp( ((*installed)[i].getName()).c_str(), (packageinst->getName()).c_str()) && packageinst->getVersion()<=(*installed)[i].getVersion()){
			cout<<"\nThe package is installed with the current or more recent version"<<endl;
			exit(0);
		}
	}
			
	//check for dependencies
     	while(packageinst->getNextDep(y)){
        
		for(int i=0;i<installed->size();i++){
                	gotit=0;
                	depVersion(y,depver);
            		if(!strcmp(  ((*installed)[i].getName()).c_str(),y.c_str())){
             			if( (((*installed)[i].Version())>=depver)  || ( ((*installed)[i].Version())=="0.0.0") ){
                 			cout<<"\nPackage "<<y<<" is installed, removing from dependencies...";
							packageinst->removeDep(packageinst->getLoc());
                			gotit=1;
                			break;
               			}
                		else{
                     			cout<<"\nPackage "<<y<<" version "<<(*installed)[i].getVersion()<<" found but version "<<depver<<" required."<<endl;
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

