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

  The current config works like the example below...

  >cat packs/foobar-0.2

config:--prefix=/usr
make: DESTDIR=/dev/null
#This is a comment
conf: crash:allways
conffile:/etc/foobar.conf


 *********************************************************************************/


#include "packinst.hpp"
using namespace std;

//Get the next dependency
bool packinst::getNextDep(string &dep)
{
	if(loc>=deps.size())
		return 0;
	else
	{
		dep=deps[loc];
		loc++;
		return 1;
	}
}

//Sorts out optional dependencies
vector<string> optional(string packs){
	vector<string> *temp=new vector<string>;
	vector<string> *add=new vector<string>;
	vector<string> *rem=new vector<string>;
	vector<string> ret;
	if(getenv("OPTIONAL")&& strcmp(getenv("OPTIONAL"),"")){//User passed some add remove stuff for optional deps
		*temp=loadLocation(getenv("OPTIONAL"));
		for(int i=0;i<temp->size();i++){
			if(!strncmp( (*temp)[i].c_str(),"+",1))
				add->push_back( (*temp)[i].substr(1));
			else if(!strncmp( (*temp)[i].c_str(),"-",1))
				rem->push_back( (*temp)[i].substr(1));
		}
	}
	*temp=loadLocation(packs);
	if(Config::getOptionalDep()==RECOMMENDED){
		for(int i=0;i<temp->size();i++){
			if(!strncmp( (*temp)[i].c_str(),"!",1))
				ret.push_back( (*temp)[i].substr(1) );
		}
	}
	else if(Config::getOptionalDep()==ALL)
		ret=*temp;

	delete temp;
	for(int i=0;i<add->size();i++)
		ret.push_back( (*add)[i] );

	for(int i=0;i<ret.size();i++){
		for(int j=0;j<rem->size();j++){
			if(!strcmp(ret[i].c_str(), (*rem)[j].c_str())){
				ret.erase(ret.begin()+i);
				i--;
				break;
			}
		}
	}
	delete add,remove;
	return ret;
}

//Get and return a package installation from file
packinst getPackage(string location){
	packinst pack;
	vector<string> file=read(location);
	if(file.size()<=0)
		return pack;
	
	vector<int> line,pos;
	for(int i=0;i<file.size();i++){
		file[i]=file[i].substr(0,file[i].find_first_of("#"));
		if( file[i].find_first_of(":")+1>0  ){
			pos.push_back(file[i].find_first_of(":"));
			line.push_back(i);
		}
	}
	string command,value;
	line.push_back(file.size());
	for(int i=0;i<pos.size();i++){
		command=file[line[i]].substr(0,pos[i]);
		value=file[line[i]].substr(pos[i]+1);

		//Do a mini loop to get all of the lines in between the commands

		for(int j=line[i]+1;j<line[i+1];j++){
			if(file[j].length()>0)
				value=value+"\n"+file[j];
		}

		//Do we set anything?

		if(!strcmp(command.c_str(),"config"))
			pack.setConfig(value);
		else if(!strcmp(command.c_str(),"make"))
			pack.setMake(value);
		else if(!strcmp(command.c_str(),"makeinst"))
			pack.setMakeInst(value);
		else if(!strcmp(command.c_str(),"deps"))
			pack.setDeps(loadLocation(value));
		else if(!strcmp(command.c_str(),"location"))
			pack.setWget(value);
		else if(!strcmp(command.c_str(),"preinstall"))
			pack.setPreInstall(value);
		else if(!strcmp(command.c_str(),"postinstall"))
			pack.setPostInstall(value);
		else if(!strcmp(command.c_str(),"conffile"))
			pack.setConfFile(value);
		else if(!strcmp(command.c_str(),"conf"))
			pack.setConf(value);
		else if(!strcmp(command.c_str(),"meta"))
			pack.setMeta(1);
		else if(!strcmp(command.c_str(),"optional"))
			pack.addDeps(optional(value));
		else if(!strcmp(command.c_str(),"patches"))
			pack.setPatches(loadLocation(value));
	}
	return pack;
}
