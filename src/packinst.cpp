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
#include <iostream> //For testing

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
vector<string> optional(string packs, string name=NULL){
	vector<string> *temp = new vector<string>;
	vector<string> *add = new vector<string>;
	vector<string> *rem = new vector<string>;
	vector<string> ret;
	if(getenv("OPTIONAL")&& strcmp(getenv("OPTIONAL"),"")){//User passed some add remove stuff for optional deps
		*temp=com2vec(getenv("OPTIONAL"));
		for(int i=0;i<temp->size();i++){
			if(!strncmp( (*temp)[i].c_str(),"+",1))
				add->push_back( (*temp)[i].substr(1));
			else if(!strncmp( (*temp)[i].c_str(),"-",1))
				rem->push_back( (*temp)[i].substr(1));
		}
	}
	//Get the optional deps stored in the file
	vector<string> *stored = new vector<string>;
	if(name.length()>0)
		*stored=read(Config::getOptionDir()+name);
	if(stored->size()>0){
		string stone;
		for(int i=0;i<stored->size();i++)
			stone=stone+(*stored)[i];
		delete stored;
		vector<string> *past = new vector<string>;
		*past=com2vec(stone);
		for(int i=0;i<past->size();i++){
			if(!strncmp( (*past)[i].c_str(),"+",1))
				add->push_back( (*past)[i].substr(1));
			else if(!strncmp( (*temp)[i].c_str(),"-",1))
				rem->push_back( (*past)[i].substr(1));
		}
		delete past;
	}
	else
		delete stored;
	*temp=com2vec(packs);
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
packinst getPackage(string location,string name){
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
			pack.setDeps(com2vec(value));
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
			pack.addDeps(optional(value,name));
		else if(!strcmp(command.c_str(),"patches"))
			pack.setPatches(com2vec(value));
	}
	return pack;
}
int getPackageDeps(string location,vector<string>& plus,vector<string>& minus,string name){
	vector<string> file=read(location);
	string optionsline;
	if(file.size()<=0)
		return 0;

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
		for(int j=line[i]+1;j<line[i+1];j++){
			if(file[j].length()>0)
				value=value+"\n"+file[j];
		}
		if(!strcmp(command.c_str(),"optional")){
			optionsline=value;
			break;
		}
	}
	if(optionsline.length()<=0)
		return 1;
	vector<string> *options = new vector<string>;
	*options=com2vec(optionsline);
	plus=optional(optionsline,name);
	bool got;
	string curr;
	for(int i=0;i<options->size();i++){
		got=0;
		if((*options)[i][0]=='!')
			curr=(*options)[i].substr(1);
		else
			curr=(*options)[i];
		for(int j=0;j<plus.size();j++){
			if( curr==plus[j] ){
				got=1;
				break;
			}
		}
		if(!got)
			minus.push_back( (*options)[i]);
	}
	return 1;
}

