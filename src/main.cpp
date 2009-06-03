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
extern "C"{
#include "qhttp.h"
}



//List of functions we need.
void help();
void mrt();



int main(int argc, char *argv[]){
	Config::initialise();
    if(argc < 2){
        cerr<<"Usage:\n bpack [action] [parameters]\nUse --help for more details\n";
        return 0;
    }    
         else if(!strcmp(argv[1], "install")){
              for(int i=2;i<argc;i++){
				  
				install(argv[i],0);
				
			}
              
         }    
         else if(!strcmp(argv[1],"scan")){
              scan();
              }
		 else if(!strcmp(argv[1],"terminal")){
              terminal();
              }
         else if(!strcmp(argv[1],"update"))
              update();
			  
		 else if(!strcmp(argv[1],"clean")){
			 //Clean all
			 if(argc==2){
				 cout<<"Cleaning up tarballs..."<<endl;
				 vector<string> tars=loadLocation(search(Config::getTarballDir()));
				 for(int i=0;i<tars.size();i++)
					 tars[i]=Config::getTarballDir()+tars[i];
				 erase(tars);
				 cout<<"Cleaning up packs..."<<endl;
				 vector<string> packs=loadLocation(search(Config::getPackInstDir()));
				 for(int i=0;i<packs.size();i++)
					packs[i]=Config::getPackInstDir()+packs[i];
				 erase(packs);
			 }
			 else if(!strcmp(argv[2],"cache")){
				 cout<<"Cleaning up tarballs..."<<endl;
				 vector<string> tars=loadLocation(search(Config::getTarballDir()));
				 for(int i=0;i<tars.size();i++)
					 tars[i]=Config::getTarballDir()+tars[i];
				 erase(tars);
			 }
			 else if(!strcmp(argv[2],"packs")){
				 cout<<"Cleaning up packs..."<<endl;
				 vector<string> packs=loadLocation(search(Config::getPackInstDir()));
				 for(int i=0;i<packs.size();i++)
					packs[i]=Config::getPackInstDir()+packs[i];
				 erase(packs);
			 }
		 }
              
         else if(!strcmp(argv[1],"remove")){
              for(int i=2;i<argc;i++){
				  
				removePack(argv[i]);
				
			  }
         }
         else if(!strcmp(argv[1],"upgrade"))
              for(int i=2;i<argc;i++){
				  
				install(argv[i],1);
				
			  }
              
         else if(!strcmp(argv[1],"--help"))
              help();
         else if(!strcmp(argv[1],"list")){
              
              printPackages(getInstalledPackages(Config::getPacklistPath().c_str()));
              //cout << "list not aviliable\n";
              }
         else if(!strcmp(argv[1],"mrt"))
              mrt();
         else
              cerr<<"\nUsage:\n bpack [action] [parameters]\nUse --help for more details!\n";
              
         
    return 0;
}

//This could do with a bit of work...DONE
void help()
{
	 cout<<"Bpack - Basic Package Manager, version 0.1(alpha)"<<endl;
	 cout<<"Copyright (C) 2009  Bati Sengul <batieatworld@gmail.com>"<<endl;
	 cout<<"                    Mike Bush <mpbush@gmail.com>"<<endl;
	 cout<<"\n\tHomepage <http://www.bpack.co.uk/>"<<endl;

     cout<<"\nHelp file...\nUsage: \n\n bpack [action] [parameters]\n\nactions:";
     cout<<"\n  install [packages]";
     cout<<"\n  remove [package]";
     cout<<"\n  upgrade [packages]";
     cout<<"\n  update";
     cout<<"\n  list";
	 cout<<"\n	clean";
	 cout<<"\n	clean cache";
	 cout<<"\n	clean packs";
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
