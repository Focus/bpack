#include <vector>
#include <fstream>
#include <cstring>
#include <iostream>
#include <cstdlib>
using namespace std;
#include "version.hpp"
#include "package.hpp"
#include "storage.hpp"
#include "scan.hpp"
#include "config.hpp"
#include "storage.hpp"



string path;

void versionSet(package &pack)
{
     /*vector<string> *locations=new vector<string>;
     string *name=new string;
     cout << "\n\nTrying to guess version...";
     *locations=pack.getLocations();
     *name=pack.getName();
	version *ver=new version;
	for(int i=0;i<locations->size();i++){
		(*locations).[i]
	}
	
	delete ver;
	delete name;
	delete locations;*/
}


package getPack(const string pack)
{
        
        cout.flush();
      package curpack;
      curpack.setName(pack);
      string command;
      command="cd "+Config::getLogDir()+" && "+"find /usr/ -name "+pack+"* | tee curpack.log";        
      system(command.c_str());
      command=Config::getLogDir()+"curpack.log";
      ifstream textfile;
      textfile.open(command.c_str());
      //delete command;

      
      if(!textfile)
      {
           cerr<<"\n\nI couldn't open package directories!! \n\n";            
          exit(1);
      }
     string dir;
      while(!textfile.eof())
      {
          textfile >> dir;
          curpack.addLocation(dir);
      }
      //delete dir;
      textfile.close();
      versionSet(curpack);
      return curpack;
}


void scan()
{
     path =Config::getPackInstDir();
     cout<<"\n\nScanning filesystem for packages, note that this may return obscure directories.\n";
     cout<<"Please manually edit before removing or updating any of these!!";
     cout.flush();

     string *command=new string;          
     cout<<"\nGetting package list...";
     *command="cd "+path+" && rm -f "+Config::getLogDir()+"packlist.log && rm -f "+Config::getLogDir()+"curpack.log && dir > "+Config::getLogDir()+"packlist.log";
     system(command->c_str());
     /*if(!system(command->c_str()))
         {
         cerr<<"\n\nError: I can't use command line!\n\n";
         exit(1);               
         }*/
     *command=Config::getLogDir()+"packlist.log";
     ifstream textfile;
     textfile.open(command->c_str());
     delete command;
     if(!textfile)
     {
          cerr<<"\n\nI couldn't open packagelist.log!! \n\n"; 
          exit(1);
     }
	cout<<"Done!"<<endl;
     string pack;
     vector<package> *packlist=new vector<package>;
	package *temp=new package;
     while(!textfile.eof())
     {

         textfile >> pack;
	cout<<"\nChecking for package "<<pack<<"...";
	*temp=getPack(pack);
	 if((temp->getLocations()).size()>0){
		cout<<"\nPackage "<<pack<<" found!"<<endl; 
		packlist->push_back(*temp);
	}
     }

        
     delete temp;
     printPackages(*packlist);

	delete packlist;
     //delete pack;
     textfile.close();     
     
//     delete curpack;
}
        
                
                
