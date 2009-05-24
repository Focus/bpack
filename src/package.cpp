#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <vector>
#include "storage.hpp"
#include "version.hpp"
#include "packinst.hpp"
#include "package.hpp"
#include "config.hpp"

using namespace std;

package::package(const string pname)
{
                       name=pname;
                       scan=0;
}

package::package(const string pname,const vector<string> plocations)
{
                       name=pname;
                       locations=plocations;
                        scan=0;                       
}

package::package(const packinst inst)
{
                       name=inst.getName();
                       ver=inst.getVersion();
                        scan=0;                    
                       
}


bool package::write(){
     ofstream text(Config::getPacklistPath().c_str(),ios::app);
     if(!text){
               cerr<<"\nNo premission to write to package list!";
               return 0;;
     }
     text << "\n\n";
     if(scan)
             text << "!";
     
     text << "[";
     text << name;
     text <<";\n";
     text <<ver;
     text <<";\n";
     text << locations[0];
     for (int i=1;i<locations.size();i++){
         text << ",";
         text << locations[i];
         }
     text<<";]\n";
     text.close();
     return 1;
}

//Prints out a vector of packages   
void printPackages(const vector<package> packagelist)
{
     if(packagelist.size()==0)
     return;
     package *currentpack=new package;
     for(int i=0;i<packagelist.size();i++){
             *currentpack=packagelist[i];  
             cout<<"\nName of the package:\t"<<currentpack->getName()<<"\n";
                                cout<<"\nVersion of the package:\t"<<currentpack->getVersion()<<"\n";
                                cout<<"\nLocations of the package:\n"; 
           for(int ii=0; ii < (currentpack->getLocations()).size(); ii++)   
                                 { 
                                              cout << "\t"<<(currentpack->getLocations())[ii] << endl;   
                                 }
           }
     
     delete currentpack;
}

//Gets a list of installed stuff from the location given, format is [name;version;files/dirs;]
vector<package> getInstalledPackages(const char* location){
     vector<string> passy(3,"");
     ifstream textfile;
     vector<package> packagelist;
     package *currentpack=new package;
     
     textfile.open(location);

     if(!textfile){
               cerr<<"\nNo configuration here, I'm so lost!.\n";
               exit(1);
     }

     string x;
     string text;
     while(!textfile.eof())
     {
        textfile >> x;
        if(!textfile.eof())
        text +=x;
     }

     textfile.close();

     int start=0;
     while(start+1<text.length()){
                                 
                                start=separate(text,passy,start);
                                currentpack->setName(passy[0]);
                                currentpack->setVersion(passy[1]);
                                currentpack->setLocations(loadLocation(passy[2]));
                                start++;
                                packagelist.push_back(*currentpack);
                                }
     delete currentpack;

     return packagelist;
     }
