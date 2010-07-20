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

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
using namespace std;
#include "version.hpp"
#include "package.hpp"
#include "storage.hpp"
#include "scan.hpp"
#include "config.hpp"
#include "install.hpp"
#include "remove.hpp"
#include "search.hpp"
#include "terminal.hpp"
#include "update.hpp"
#include "sync.hpp"
extern "C"{
#include "qhttp.h"
}


#define VERSION 0.2
#define MIN(x,y) x<y? x:y

//List of functions we need.
void help();
void mrt();


int main(int argc, char *argv[]){
	if(!strcmp(argv[0],"versioncmp")){
		if(argc < 3){
			cout<<"versioncmp [ver1] [ver2]"<<endl;
			return 1;
		}
		version ver1,ver2;
		string stringy;
		stringy=argv[1];
		ver1=stringy;
		stringy=argv[2];
		ver2=stringy;
		if(ver1==ver2)
			cout<<"0"<<endl;
		else if(ver1>ver2)
			cout<<"1"<<endl;
		else
			cout<<"-1"<<endl;
		return 0;
	}
	Config::initialise();
	if(argc < 2){
		cerr<<"Usage:\n bpack [action] [parameters]\nUse --help for more details\n";
		return 0;
	}    
	else if(!strcmp(argv[1], "install")){
		string name;
		version ver;
		for(int i=2;i<argc;i++){
			install(argv[i],0);
			name=argv[i];
			sepVer(name,ver);
			write(name+"\n",Config::getInstallDir()+"list",0);
		}

	}    
	else if(!strcmp(argv[1],"scan"))
		scan();
	else if(!strcmp(argv[1],"terminal"))
		terminal();
	else if(!strcmp(argv[1],"update"))
		update();
	else if(!strcmp(argv[1],"clean")){
		//Clean all
		if(argc==2){
			cout<<"Cleaning up tarballs..."<<endl;
			vector<string> tars=com2vec(search(Config::getTarballDir()));
			for(int i=0;i<tars.size();i++)
				tars[i]=Config::getTarballDir()+tars[i];
			erase(tars);
			cout<<"Cleaning up packs..."<<endl;
			vector<string> packs=com2vec(search(Config::getPackInstDir()));
			for(int i=0;i<packs.size();i++)
				packs[i]=Config::getPackInstDir()+packs[i];
			erase(packs);
		}
		else if(!strcmp(argv[2],"cache")){
			cout<<"Cleaning up tarballs..."<<endl;
			vector<string> tars=com2vec(search(Config::getTarballDir()));
			for(int i=0;i<tars.size();i++)
				tars[i]=Config::getTarballDir()+tars[i];
			erase(tars);
		}
		else if(!strcmp(argv[2],"packs")){
			cout<<"Cleaning up packs..."<<endl;
			vector<string> packs=com2vec(search(Config::getPackInstDir()));
			for(int i=0;i<packs.size();i++)
				packs[i]=Config::getPackInstDir()+packs[i];
			erase(packs);
		}
	}

	else if(!strcmp(argv[1],"remove")){
		for(int i=2;i<argc;i++)
			removePack(argv[i]);
	}
	else if(!strcmp(argv[1],"upgrade")){
		if(argc>2 && !strcmp(argv[2],"all")){
			vector<string> *packs = new vector<string>;
			string name;
			version ver;
			*packs=com2vec(search(Config::getPacklistPath()));
			for(int i=0;i<packs->size();i++){
				name=(*packs)[i];
				sepVer(name,ver);
				install(name,1);
			}
			delete packs;
		}
		else{
			for(int i=2;i<argc;i++)
				install(argv[i],1);
		}
	}
	else if(!strcmp(argv[1],"get")){
		if(argc<4){
			cout<<"Usage:\n\t bpack get [url] [save directory]"<<endl;
			return 0;
		}
		return wget(argv[2],argv[3],NULL,LOGNONE,1);
	}
	else if(!strcmp(argv[1],"search")){
		for(int i=2;i < argc;i++){
			string pack(argv[i]);
			vector <string> partial = com2vec(search(Config::getPackInstDir(),"*"+pack+"*"));
			sort(partial.begin(),partial.end());
			version ver;
			string name,hold;
			cout<<"Packages starting with the name "<<pack<<":";
			for(int j=0; j<partial.size();j++){
					name=partial[j];
					sepVer(name,ver);
					if(name!=hold){
							cout<<"\n\n  "<<name<<"\n    "<<ver.asString();
							hold=name;
					}
					else
							cout<<" , "<<ver.asString();
			}
			cout<<"\n\n";
		}
	}
	else if(!strcmp(argv[1],"--help"))
		help();
	else if(!strcmp(argv[1],"list"))
		printPackages();
	else if(!strcmp(argv[1],"sizeof")){
		package pack;
		string name;
		for(int i=2;i<argc;i++){
			name=argv[i];
			pack=getInstalledPackage(name);
			if(pack.getName().length()<=0){
				cout<<"Package "<<argv[i]<<" not found"<<endl;
				return 0;
			}
			cout<<pack.getName()+"-"+pack.getVersion()<<"\t"<<packSize(pack.getLocations())<<endl;
		}
	}
	else if(!strcmp(argv[1],"mrt"))
		mrt();
	else if(!strcmp(argv[1],"sync")){
		if(argc<3){
			cerr<<"\nSync what?\n";
			return 0;
		}
		if(!strcmp(argv[2],"pacman"))
			pacmanSync();
	}
	else if(!strcmp(argv[1],"track")){
		if(argc<3){
			cerr<<"\nTrack what?\n";
			return 0;
		}
		string stuff=argv[2];
		string path=get_current_dir_name();
		makeinstall(path,stuff);
	}
	else if(!strcmp(argv[1],"pretend")){
		string pack;
		cout<<"Checking dependencies...\n\n";
		for(int i=2;i<argc;i++){
			pack=argv[i];
			pretend(pack,0);
		}
	}		
	else
		cerr<<"\nUsage:\n bpack [action] [parameters]\nUse --help for more details!\n";


	return 0;
}

//This could do with a bit of work...DONE
void help()
{
	cout<<"Bpack - Basic Package Manager, version "<<VERSION<<endl;
	cout<<"Copyright (C) 2009  Bati Sengul <batieatworld@gmail.com>"<<endl;
	cout<<"                    Mike Bush <mpbush@gmail.com>"<<endl;
	cout<<"\n\tHomepage <http://www.bpack.co.uk/>"<<endl;

	cout<<"\nHelp file...\nUsage: \n\n bpack [action] [parameters]\n\nactions:";
	cout<<"\n  install [packages]";
	cout<<"\n  remove [package]";
	cout<<"\n  upgrade [packages]";
	cout<<"\n  update";
	cout<<"\n  search [package (maybe partial) names]";
	cout<<"\n  track [command (use quote marks if you have escape characters)]";
	cout<<"\n  list";
	cout<<"\n  sizeof [package]";
	cout<<"\n  clean";
	cout<<"\n  clean cache";
	cout<<"\n  clean packs";
	cout<<"\n  sync [package manager]";
	cout<<"\n  get [url] [save directory]";
	cout<<"\n  --help";


	cout<<"\n\tThis bpack has Mr.T powers!!\n";

}
void mrt()
{
	cout<<"\n\n";
	cout<<"                 ___         "<<endl;
	cout<<"                |___|        "<<endl;
	cout<<"	       /  o o \\      "<<endl;
	cout<<"	      [c-_  __|      "<<endl;
	cout<<"	       \\__-(--)      "<<endl;
	cout<<"	     ( \\=====/ )     "<<endl;
	cout<<"	     ( )\\===/( )     "<<endl;
	cout<<"	    (--) --- (--)    "<<endl;
	cout<<""<<endl;
	cout<<"I PITY THE FOOL THAT DOESN'T BPACK!!"<<endl;
}
