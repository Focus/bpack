#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <cstdlib>

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
                exit(1);
             }
                                                                                                          
     }
     
     if((breaks[breaks.size()-2]>=breaks[breaks.size()-1]) || (breaks[breaks.size()-1]-breaks[breaks.size()-3]!=1))
      {
                cerr<<"\n2Breaks are not valid, please check the configuration (if needs be delete it and scan again).";
 
                exit(1);
             }
}

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
     
     
     
