#include <iostream>
#include <fstream>
#include <string>
#include <vector>
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
               exit(1);
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
     
}
