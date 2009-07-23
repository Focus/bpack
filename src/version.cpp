/*
 * 		Copyright 2009 (c) Bpack- Bati Sengul <batieatworld@gmail.com>
 * 							      Mike Bush <mpbush@gmail.com>
 * 
 * 					http://www.bpack.co.uk
 * 
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */


#include "version.hpp"
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
	if(vecver.size()!=numbers.size())
		return 0;
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
	vector<int> numberst=numbers;
	while(vecver.size()>numbers.size())
		numberst.push_back(0);
	while(vecver.size()<numbers.size())
		vecver.push_back(0);
	for(int i=0;i<vecver.size();i++){
		if(vecver[i]>numberst[i])
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
}

