#include <iostream>
#include <string>
#include <vector>
#include <fstream>


using namespace std;
#include "version.hpp"
#include "package.hpp"
#include "storage.hpp"

//Valid breaks? Otherwise quit!
const void checkBreaks (vector<int> breaks){
     for(int i=0;i+2<breaks.size();i++){
             if((breaks[i]<=0 || breaks[i] > breaks[breaks.size()-1]))
             {
                cerr<<"\n1Breaks are not valid, please check the configuration (if needs be delete it and scan again).";
                system("pause");
                exit(1);
             }
                                                                                                          
     }
     if((breaks[breaks.size()-2]>=breaks[breaks.size()-1]) || (breaks[breaks.size()-1]-breaks[breaks.size()-3]!=1))
      {
                cerr<<"\n2Breaks are not valid, please check the configuration (if needs be delete it and scan again).";
                system("pause");
                exit(1);
             }
}
/*Load package from the text string, start seeking at start
int loadPackage(const string text,string &name,string &version,string &locations,const int start){
     vector<int> breaks(5,0);
     breaks[0]=text.find(";",start);
     breaks[1]=text.find(";",breaks[0]+1);
     breaks[2]=text.find(";",breaks[1]+1);
     breaks[3]=text.find("[",start);
     breaks[4]=text.find("]",breaks[2]);
     checkBreaks(breaks);
     //cout<<"\nThe brakes:\t"<<breaks[0]<<"  "<<breaks[1]<<"  "<<breaks[2]<<"  "<<breaks[3]<<"  "<<breaks[4]<<"\n";
     name=text.substr(breaks[3]+1,breaks[0]-breaks[3]-1);
     version=text.substr(breaks[0]+1,breaks[1]-breaks[0]-1);
     locations=text.substr(breaks[1]+1,breaks[2]-breaks[1]-1);
     
    return breaks[4];
}*/

int separate(const string text,vector<string> &par,const int start){
     int numbreaks=par.size();
     vector<int> breaks(numbreaks+2,0);
    breaks[0]=text.find(";",start);
    for(int i=1;i<numbreaks;i++){
     breaks[i]=text.find(";",breaks[i-1]+1);   
        }
     breaks[numbreaks]=text.find("[",start);
     breaks[numbreaks+1]=text.find("]",breaks[2]);   
        checkBreaks(breaks);
     //if(text[breaks[numbreaks]-1]=="!")
     
     //cout<<"\nThe brakes:\t"<<breaks[0]<<"  "<<breaks[1]<<"  "<<breaks[2]<<"  "<<breaks[3]<<"  "<<breaks[4]<<"\n";
     par[0]=text.substr(breaks[numbreaks]+1,breaks[0]-breaks[numbreaks]-1);
     for(int i=1; i<numbreaks;i++){
     par[i]=text.substr(breaks[i-1]+1,breaks[i]-breaks[i-1]-1);
     }
     //par[numbreaks-1]=text.substr(breaks[numbreaks-2]+1,breaks[numbreaks-1]-breaks[numbreaks-2]-1)
    return breaks[numbreaks+1];
}

//Lets get the different locations for stuff
vector<string> loadLocation(const string locations)
{
        vector<string> locs;
        int index=0;
        int temp=index;
        while(index<locations.length())
        {
                temp=locations.find(",",index);
                
                if(temp<=index)
                temp=locations.length();
                
                locs.push_back(locations.substr(index,temp-index));
                index=temp+1;
                                       
                                       
        }
              return locs;
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
     
     
     
