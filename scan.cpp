#include <vector>
#include <fstream>
#include <string>
#include <iostream>
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
     vector<string> *locations=new vector<string>;
     string *name=new string;
     cout << "\n\nTrying to guess version...";
     *locations=pack.getLocations();
     *name=pack.getName();
     
     
}


package getPack(const string pack)
{
        cout<<"Pi\n";
        cout.flush();
      package curpack;
      curpack.setName(pack);
      string command;
      command="cd "+path+" && "+"find /usr/ -name "+pack+"* | tee curpack.log";        
      system(command.c_str());
      command=path+"curpack.log";
      ifstream textfile;
      textfile.open(command.c_str());
      //delete command;
cout<<"Pi\n";
        cout.flush();
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
     *command="cd "+path+" && rm -f ./packlist.log && rm -f ./curpack.log && dir > packlist.log";
     system(command->c_str());
     /*if(!system(command->c_str()))
         {
         cerr<<"\n\nError: I can't use command line!\n\n";
         exit(1);               
         }*/
     *command=path+"packlist.log";
     ifstream textfile;
     textfile.open(command->c_str());
     delete command;
     if(!textfile)
     {
          cerr<<"\n\nI couldn't open packagelist.log!! \n\n";            
          exit(1);
     }
     string pack;
     vector<package> *packlist=new vector<package>;
     while(!textfile.eof())
     {
        
         textfile >> pack;
cout<<"Pi\n";
        cout.flush();
         packlist->push_back(getPack(pack));
     }

        
     
     printPackages(*packlist);


     //delete pack;
     textfile.close();     
     
//     delete curpack;
}
        
                
                
