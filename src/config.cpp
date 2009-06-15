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

#include <cstring>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include "config.hpp"
#include "storage.hpp"
using namespace std;

string Config::installdir, Config::scriptdir, Config::packinstdir, Config::tarballdir, Config::packmandir, Config::packlistpath, Config::logdir,Config::lib,Config::coll,Config::cflags,Config::cxxflags;
int Config::quitlevel;


void Config::initialise()
{
	Config::installdir="/var/bpack/";
	if(getenv("BPACK_DIR")&&strcmp(getenv("BPACK_DIR"),"")){
		string env(getenv("BPACK_DIR"));
		if(env[env.size()-1]=='/')
			Config::installdir=env;
		else
			Config::installdir=env+"/";
	}
	coll="test";//Currently default, we need to change this to official soon!
    	scriptdir = installdir + "scripts/";
    	packinstdir = installdir + "packs/";
    	tarballdir = installdir + "tarballs/";
    	packmandir = installdir + "packman/";
    	packlistpath = installdir + "packs.list";
	logdir=installdir+"logs/";
	lib=installdir+"libs/hijack.so.1.0";
	Config::quitlevel=0;
	cflags="";
	cxxflags="";
	//Load the config
	vector<string> *file= new vector<string>;
	*file=read("/etc/bpack.conf");
	if(file->size()<=0){
		delete file;
		return;
	}
	vector<int> *line=new vector<int>;
	vector<int> *pos=new vector<int>;
	for(int i=0;i<file->size();i++){
		(*file)[i]=(*file)[i].substr(0,(*file)[i].find_first_of("#"));
		if( (*file)[i].find_first_of(":")+1>0  ){
			pos->push_back((*file)[i].find_first_of(":"));
			line->push_back(i);
		}
	}
	string command,value;
	line->push_back(file->size());
	for(int i=0;i<pos->size();i++){
		command=(*file)[(*line)[i]].substr(0,(*pos)[i]);
		value=(*file)[(*line)[i]].substr((*pos)[i]+1);
		//Do a mini loop to get all of the lines in between the commands
		for(int j=(*line)[i]+1;j<(*line)[i+1];j++){
			if((*file)[j].length()>0)
				value=value+"\n"+(*file)[j];
		}
		//Do we set anything?
		if(!strcmp(command.c_str(),"quitlevel")){
			stringstream ss;
			(ss) << value;
			if( ((ss) >> Config::quitlevel).fail() )
				Config::quitlevel=0;
			//delete ss;
		}
		else if(!strcmp(command.c_str(),"collection"))
			Config::coll=value;
		else if(!strcmp(command.c_str(),"CFLAGS") || !strcmp(command.c_str(),"cflags"))
			Config::cflags=value;
		else if(!strcmp(command.c_str(),"CXXFLAGS") || !strcmp(command.c_str(),"cxxflags"))
			Config::cxxflags=value;
	}
	delete line,pos,file;
    	
}
