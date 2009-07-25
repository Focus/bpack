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


#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <vector>
#include <sys/stat.h> 
#include <sstream>

#include "package.hpp"

#include "version.hpp"
#include "packinst.hpp"
#include "config.hpp"
#include "search.hpp"
#include "storage.hpp"
#include "remove.hpp"

#define MIN(x,y) x<y ? x:y
using namespace std;

package::package(const string pname)
{
	name=pname;
	scan=0;
}

package::package(const string pname,const vector<string> plocations)
{
	name=pname;
	locations=plocations;
	scan=0;                       
}

package::package(const packinst inst)
{
	name=inst.getName();
	ver=inst.getVersion();
	scan=0;                    

}


bool package::save(){
	return write(locations,Config::getPacklistPath()+name+"-"+ver.asString());
}
//TODO version checks!
bool package::remove(){
	return erase(Config::getPacklistPath()+name+"-"+ver.asString());
}

//Separates the foo-0.2.2 to foo and 0.2.2
//TODO: Lame name
void depVersion(string &dep, version &ver){
	int pos,pos2,test;
	pos=0;
	string temp;
	stringstream ss;
	temp=dep;
	do{
		ss.flush();
		pos2=temp.find("-");
		pos=pos+pos2;
		temp=temp.substr(MIN(pos2+1,temp.length()));
		test=strcspn(temp.substr(0,1).c_str(),"1234567890");
	}while(pos2!=string::npos &&  test!=0);

	//If there is no version set
	if(pos2==string::npos)
		return;

	test=temp.length();
	//Make a 2.3.5-7 into 2.3.5.7
	while(temp.find("-")!=string::npos)
		temp[temp.find("-")]='.';
	while(temp.find("rc")!=string::npos)
		temp=temp.substr(0,temp.find("rc"))+"."+temp.substr(temp.find("rc")+2);
	while(temp.find("beta")!=string::npos)
		temp=temp.substr(0,temp.find("beta"))+temp.substr(temp.find("beta")+4);

	ver=temp;
	dep=dep.substr(0,dep.length()-test-1);

}

string packSize(vector<string> locs){

	double num=0;
	struct stat st;
	for(int i=0;i<locs.size();i++){
		if(!stat(locs[i].c_str(),&st))
			num=num+st.st_size;
	}
	string ret="";
	std::ostringstream ss;
	//Try GB then MB then KB
	if(num/1000000000>=1){
		ss<<(num/1000000000);
		ret=ss.str()+" GB";
	}
	else if(num/1000000>=1){
		ss<<(num/1000000);
		ret=ss.str()+" MB";
	}
	else{
		ss<<(num/1000);
		ret=ss.str()+" KB";
	}
	return ret;

}

//Prints out a vector of packages   
void printPackages(){
	vector<string> *packs=new vector<string>;
	*packs=loadLocation(search(Config::getPacklistPath()));
	for(int i=0;i<packs->size();i++)
		cout<<(*packs)[i]<<endl;
	delete packs;
}


//Gets a list of installed stuff from the location given
vector<package> getInstalledPackages(const string location){
	vector<string> *files=new vector<string>;
	*files=loadLocation(search(location));
	string name;
	version ver;
	package current;
	vector<package> ret;
	for(int i=0;i<files->size();i++){
		name=(*files)[i];
		depVersion(name,ver);
		current.setName(name);
		current.setVersion(ver);
		current.setLocations(read(location+(*files)[i]));
		ret.push_back(current);
	}
	delete files;
	return ret;
}
vector<package> getInstalledPackages(){
	return getInstalledPackages(Config::getPacklistPath().c_str());
}
/************************************************************************
 * Gets a package with the given name
 * ************************************************************************/

package getInstalledPackage(const string in){
	string name=in;
	version ver;
	depVersion(name,ver);
	vector<string> *files=new vector<string>;
	package ret;
	*files=loadLocation(search(Config::getPacklistPath()));
	string pname;
	version pver;
	for(int i=0;i<files->size();i++){
		pname=(*files)[i];
		depVersion(pname,pver);
		if(!strcmp(pname.c_str(),name.c_str()) && (pver==ver || ver=="0.0.0")){
			ret.setName(pname);
			ret.setVersion(pver);
			ret.setLocations(read(Config::getPacklistPath()+(*files)[i]));
			break;
		}
	}
	delete files;
	return ret;

}

