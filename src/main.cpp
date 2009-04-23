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
//List of functions we need.
void help();
void mrt();



int main(int argc, char *argv[]){
	Config::initialise(argv[0]);
	cout<<Config::getPackInstDir()<<endl;
    if(argc < 2){
        cerr<<"Usage:\n\n bpack [action] [parameters]\nUse --help for more details\n";
        return 0;
    }    
         else if(!strcmp(argv[1], "install")){
              
              preinstall(argv,argc);
              
         }
	else if(!strcmp(argv[1], "-i")){
              
              internetinstall(argv,argc);
              
         }     
         else if(!strcmp(argv[1],"scan")){
              scan();
              }
         else if(!strcmp(argv[1],"update"))
              cout<<"\nUpdate called";
              
         else if(!strcmp(argv[1],"remove")){
              if(argc > 2)
		removePack(argv[1]);
         
              else{
                     cerr<<"\nUsage:\n\n bpack [action] [parameters]\nUse --help for more details\n";
                     return 0;
               }
              
              }
         else if(!strcmp(argv[1],"upgrade"))
              cout<<"\nUpgrade called";
              
         else if(!strcmp(argv[1],"--help"))
              help();
         else if(!strcmp(argv[1],"list")){
              
              printPackages(getInstalledPackages(Config::getPacklistPath().c_str()));
              //cout << "list not aviliable\n";
              }
         else if(!strcmp(argv[1],"mrt"))
              mrt();
         else
              cerr<<"\nUsage:\n\n bpack [action] [parameters]\nUse --help for more details!\n";
              
         
    return 0;
}
void help()
{
     cout<<"\nHelp file...\nUsage: \n\n bpack [action] [parameters]\n\nactions:";
     cout<<"\n  install [package] [arguments] [package] [arguments]....    in";
     cout<<"\n  remove [package]    removes [package]";
     cout<<"\n  upgrade [package]    upgrades [package] to the lastest version";
     cout<<"\n  scan    scans the computer for installed packages";
     cout<<"\n  list    lists the installed packages that bpack has stored\n";
	
	cout<<"\n\ninstall arguments:\n";
	cout<<"    --config=\"config arguments\"    passes config arguments to ./configure (e.g. --config=\"--prefix=/usr\")"<<endl;
	cout<<"    --make=\"make arguments\"    passes make arguments to make (e.g. --make=\"DESTDIR=/usr\")"<<endl;
	cout<<"    --makeinst=\"make install arguments\"    passes make install arguments to make install (e.g. --makeinst=\"DESTDIR=/usr\")"<<endl;
	cout<<"    Bpack will use defaults, if you do not wish bpack to supply any arguments leave a \"\""<<endl;
     
     cout<<"\n\nThis bpack has Mr.T powers!!\n\n";

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
