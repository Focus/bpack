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
#include "storage.hpp"
#include "error.hpp"
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

//Reads a file and the vector elements are the lines
vector<string> read(const string location){
	ifstream textfile;
	vector<string> ret;
	textfile.open(location.c_str());
	string text;
	if(!textfile)
		return ret;
	while(getline(textfile,text))
		ret.push_back(text);
	textfile.close();
	return ret;
}
     

//If statement macro
string macroif(vector<string> statement){
	
}

//Reads the macros
string macro(string command){

	string *wk=new string;
	string *temp=new string;
	vector<string> *statement=new vector<string>; 
	string ret;
	*temp=command;
	int pos;
	while( (pos=temp->find("{"))!=string::npos && pos!=temp->length()-1 ){
		*temp=temp->substr(pos+1);
		cout<<*temp<<endl;
		if(temp->find("}")==string::npos)
			err("The macro is bad");
		else{
			*wk=temp->substr(0,temp->find("}"));
			cout<<*wk<<endl;
			int i=0;
			//Get rid of any spaces after the {
			while((*wk)[i]==' ' && i<wk->length()){
				*wk=wk->substr(1);
				i++;
			}
			while( (i=wk->find(" "))!=string::npos && i+1<wk->length()){
				statement->push_back(wk->substr(0,i));
				*wk=wk->substr(i+1);
			}
			if(!strcmp( (*statement)[0].c_str(),"if"))
				cout<<macroif(*statement);
		}
	}
	return "";
}	
     
