#include <vector>
#include <cstring>
#include <ostream>
#include <fstream>
#include "version.hpp"
#include <sstream>
//For testing:
#include <iostream>
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
     numbers.clear();
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

string version::asString() const{
     string str="";
     ostringstream temp;
     temp <<(this->asVector())[0];
     str=temp.str();
     for(int i=1;i<(this->asVector()).size();i++){
             temp.str("");
             temp<<(this->asVector())[i];
             str=str+"."+temp.str();
             }
     return str;
}

ostream& operator<< (ostream& ioo, version& ver){
     string str=ver.asString();
     ioo<<str;
     //return ioo;
}
ofstream& operator<< (ofstream& ioo, version& ver){
     string str=ver.asString();
     ioo<<str;
     //return ioo;
}