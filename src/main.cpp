/*********************************************************************************

Terminal getenv("PWD") segfaults when you run as sudo

*********************************************************************************/

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
              
         else if(!strcmp(argv[1],"remove")){
              for(int i=2;i<argc;i++){
				  
				remove(argv[i]);
				
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
