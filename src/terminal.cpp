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
#include <cstring>
#include <errno.h>
#include "storage.hpp"
#include "version.hpp"
#include "install.hpp"
#include "config.hpp"
#include "remove.hpp"
#include "package.hpp"
#include "error.hpp"

extern "C"{
#include "qhttp.h"
}

using namespace std;

int makeinstall(string path,string command="make install"){
	int *pos= new int;
	*pos=path.find_last_of("/");
	string name,input,input2;
	input="";
	version ver;
	if(*pos>0){
		name=path.substr(*pos,path.size()-*pos-2);
		depVersion(name,ver);
	}
	erase("/tmp/hijack_log.txt");
	//Set compiler flags
	if(Config::getCflags().length()>0)
		setenv("CFLAGS",Config::getCflags().c_str(),1);
	if(Config::getCxxflags().length()>0)
		setenv("CXXFLAGS",Config::getCxxflags().c_str(),1);
	setenv("LD_PRELOAD",(Config::getLib()).c_str(),1);
	system(command.c_str());
	setenv("LD_PRELOAD","",1);
	vector<string> *locs=new vector<string>;
	*locs=read("/tmp/hijack_log.txt");
	if(locs->size() <= 0){
		erase("/tmp/hijack_log.txt");
		cout<<"\nThe log was empty. No tracking done!!\n"<<endl;
		delete locs;
		delete pos;
		return 0;
	}
	cout<<"\nPlease enter the name of the package [default:"<<name<<"]  :";
	getline(cin,input);
	cout<<input<<endl;
	if(strcmp(input.c_str(),""))
		name=input;
	cout<<"\nPlease enter the version of the package [default:"<<ver.asString()<<"]  :";
	getline(cin,input);
	if(strcmp(input.c_str(),""))
		ver=input;
	package *pack=new package;
	pack->setName(name);
	pack->setLocations(*locs);
	pack->setVersion(ver.asString());
	pack->save();
	delete pack;
	delete locs;
	delete pos;
	return 1;
}

void terminal(){

	string command,path;
	command="";
	path=get_current_dir_name();
	cout<<"\nBpack terminal\nUse exit to quit. The command  make install  will get bpack to track down the calls"<<endl;
	while(strcmp("exit",command.c_str())){
		if(command.find("cd ")+1>0){
			if(chdir(command.substr(command.find("cd ")+3).c_str())!=0)
				cout<<strerror(errno)<<endl;
		}
		else if(command.find("trac ")!=string::npos)
			makeinstall(path,command.substr(command.find("trac")+5));
		else if(command.find("dl ")!=string::npos)
			wget(command.substr(command.find("dl")+3).c_str(),path.c_str(),NULL,LOGNONE,1);
		else if(strcmp(command.c_str(),"make install"))
			system(command.c_str());
			
		else
			makeinstall(path);
		
		path=get_current_dir_name();
		cout<<"[bpack : "<<path<<"] $ ";
		getline(cin,command);
		
	}
}
