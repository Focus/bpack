#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <vector>
#include <sys/stat.h> 
#include <sstream>
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
bool package::remove(){
	
	vector<string> *file = new vector<string>;
	*file=read(Config::getPacklistPath());
	int begin,end;
	begin=-1;
	for(int i=0;i<file->size();i++){
		if((*file)[i].find("["+name+";")!=string::npos){
			begin=i;
		}
		
		if(begin>0 && (*file)[i].find(";]")!=string::npos){
			end=i;
			break;
		}
	}
	ofstream text;
	text.open(Config::getPacklistPath().c_str(),ios::out);
    if(!text){
		delete file;
		return 0;
	}
	
	for(int i=0; i<begin;i++){
		text<<(*file)[i]+"\n";
	}
	
	for(int i=end+1; i<file->size(); i++){
		text<<(*file)[i]+"\n";
	}
	
	text.close();
	return 1;
	
}

string packSize(vector<string> locs){
	
	double num=0;
	struct stat st;
	for(int i=0;i<locs.size();i++){
		stat(locs[i].c_str(),&st);
		num=num+st.st_size;
	}
	string ret="";
	std::ostringstream ss;
	//Try GB then MB then KB
	if(num/1000000000>=1){
		ss<<(num/1000000000);
		ret=ss.str()+" GB";
	}
	else if(num/1000000>=1){
		ss<<(num/1000000);
		ret=ss.str()+" MB";
	}
	else{
		ss<<(num/1000);
		ret=ss.str()+" KB";
	}
	return ret;
	
}

//Prints out a vector of packages   
void printPackages(const vector<package> packagelist)
{
     if(packagelist.size()==0)
     return;
     package *currentpack=new package;
	 cout<<"\nInstalled packages:"<<endl;
     for(int i=0;i<packagelist.size();i++){
             *currentpack=packagelist[i];  
             cout<<"\t"<<currentpack->getName()<<"-"<<currentpack->getVersion()<<"\t"<<packSize(currentpack->getLocations())<<endl;

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
