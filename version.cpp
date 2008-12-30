#include <vector>
#include <string>
#include "version.hpp"
#include <sstream>

using namespace std;
//Default is to set 0.0.0
version::version()
{
      numbers.push_back(0);
      numbers.push_back(0);
      numbers.push_back(0);
      
}

version::version(const string ver )
{
      setVersion(ver);
}

version::~version(){
}

//Adds a version from a string by finding the . and taking integers before it
void version::setVersion(string ver)
{
      vector<int> breaks;
      int temp;
      temp=ver.find(".",0);
      while(temp>0){
                    breaks.push_back(temp);
                    temp=ver.find(".",temp+1);                    
                    }
      for(int i=0;i<breaks.size();i++){
              ver.replace(breaks[i],1," ");
              }
      istringstream strin(ver,istringstream::in);
      
      for(int i=0;i<=breaks.size();i++){
              strin >> temp;
              numbers.push_back(temp);
              }
}

bool version::operator== (const version ver) const
{
     vector<int> vecver=ver.asVector();
     int size;
     if(vecver.size()>=numbers.size())
         size=numbers.size();
     else
         size=vecver.size();
         
     for(int i=0;i<size;i++){
             if(vecver[i]!=numbers[i])
                  return 0;
             }
     return 1;
}

bool version::operator== (const string str) const{
     version ver(str);
     return ver==*this;
}

bool version::operator< (const version ver) const{
     vector<int> vecver=ver.asVector();
     int size;
     if(vecver.size()>=numbers.size())
         size=numbers.size();
     else
         size=vecver.size();
         
     for(int i=0;i<size;i++){
             if(vecver[i]>numbers[i])
             return 1;
             }
     return 0;
}

bool version::operator< (const string str) const{
     version ver(str);
     return *this<ver;
}

bool version::operator> (const version ver) const{
     return ver<*this;
}

bool version::operator> (const string str) const{
     version ver(str);
     return *this>ver;     
}

bool version::operator<= (const version ver) const{
     return (*this < ver)||(*this==ver);
}

bool version::operator<= (const string str) const{
     return (*this <str)||(*this==str);
}

bool version::operator>= (const version ver) const{
     return(*this > ver)||(*this==ver);
}

bool version::operator>= (const string str) const{
     return (*this > str)||(*this==str);
}

void version::operator= (const string str){
     this->setVersion(str);
}
