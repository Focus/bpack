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



//This has become a sort of tools library, many of the functions are misnamed and too many headers
//TODO: See above

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include "version.hpp"
#include "package.hpp"
#include "storage.hpp"
#include "error.hpp"
#define MIN(x,y) x<y ? x:y
using namespace std;

//Valid breaks? Otherwise quit!
const void checkBreaks (vector<int> breaks){
	for(int i=0;i+2<breaks.size();i++){
		if((breaks[i]<=0 || breaks[i] > breaks[breaks.size()-1]))
			err("Breaks are not valid",2);
	}
	if((breaks[breaks.size()-2]>=breaks[breaks.size()-1]) || (breaks[breaks.size()-1]-breaks[breaks.size()-3]!=1))
		err("Breaks are not valid",2);
}

//Seperates the packs.list into bits seperated by ;
//the start parameter is the line number where it should start
int separate(const string text,vector<string> &par,const int start){
	int numbreaks=par.size();
	vector<int> breaks(numbreaks+2,0);
	breaks[0]=text.find(";",start);
	for(int i=1;i<numbreaks;i++)
		breaks[i]=text.find(";",breaks[i-1]+1);
	breaks[numbreaks]=text.find("[",start);
	breaks[numbreaks+1]=text.find("]",breaks[2]);
	checkBreaks(breaks);
	par[0]=text.substr(breaks[numbreaks]+1,breaks[0]-breaks[numbreaks]-1);
	for(int i=1; i<numbreaks;i++)
		par[i]=text.substr(breaks[i-1]+1,breaks[i]-breaks[i-1]-1);
	return breaks[numbreaks+1];
}

//TODO: Stupid name
//Seperates things like something,something,something
vector<string> loadLocation(const string locations){
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

//Writes a string to a file
bool write(const string content, const string location,bool overwrite){
	ofstream text;
	if(overwrite)
		text.open(location.c_str(),ios::trunc);
	else
		text.open(location.c_str(),ios::app);
	if(!text)
		return 0;
	text<<content;
	text.close();
	return 1;
}




bool write (const vector<string> p1,const string p2,bool p3){
	string temp="";
	for(int i=0;i<p1.size();i++)
		temp=temp+p1[i]+"\n";
	return write(temp,p2,p3);
}


vector<string> read(const string location){
	ifstream textfile;
	vector<string> ret;
	textfile.open(location.c_str());
	string text;
	if(!textfile){
		return ret;
	}
	while(getline(textfile,text)){
		ret.push_back(text);
	}
	textfile.close();
	return ret;
}

//Reads the macros
void macro(string& config){
	string temp,macro;
	vector<package> *installed=new vector<package>;
	*installed=getInstalledPackages(Config::getPacklistPath().c_str());
	temp=config;
	int pos,pp;
	bool found;
	while( (pos=temp.find("{")) !=string::npos){
		temp=temp.substr(temp.find("{")+1);
		found=0;
		if(temp.find("}")==string::npos || (temp.find("{")!=string::npos && temp.find("{")<temp.find("}")))
			err("The macro is bad. Check the package and correct it",2);
		macro=temp.substr(0,temp.find("}"));
		while(macro.find(" ")!=string::npos)
			macro=macro.substr(0,macro.find(" "))+macro.substr(MIN(macro.find(" ")+1,macro.length()));
		for(int i=0;i<installed->size();i++){
			if(!strcmp( (*installed)[i].getName().c_str(),macro.c_str() )){
				found=1;
				break;
				cout<< (*installed)[i].getName()<<endl;
			}
		}
		if(found)
			config=config.substr(0,pos)+config.substr(pos+temp.find("}")+2);
		else{
			pp=temp.find("}")+2+pos;
			while(config[pp]==' ')
				pp++;
			pp+=(config.substr(pp)).find(" ");
			config=config.substr(0,pos)+config.substr(MIN(pp+1,config.length()));
		}
		temp=config;
	}	
	

	delete installed;
}
