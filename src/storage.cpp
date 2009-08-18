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
#include <sys/stat.h>

#include "search.hpp"
#include "version.hpp"
#include "package.hpp"
#include "storage.hpp"
#include "error.hpp"
#define MIN(x,y) x<y ? x:y
using namespace std;

#define BUFFER 5012

int cp(vector<string>, string);
int cp(string src,string dest,vector<string> *track){
	struct stat buf;
	string name;
	if(src.find_last_of("/")==string::npos)
		name=src;
	else
		name=src.substr(src.find_last_of("/")+1);
	if(dest[dest.length()-1]!='/')
		dest=dest+"/";
	if(stat(src.c_str(),&buf)==-1){
		err("Cannot stat "+src,0,1);
		return 0;
	}
	if((buf.st_mode & S_IFMT)==S_IFDIR){
		mkdir((dest+name).c_str(),buf.st_mode);
		return cp(com2vec(search(src,"")),dest+name,track);
	}
	FILE* srcfile=fopen(src.c_str(),"r");
	if(!srcfile){
		err("Cannot open "+src+" for reading.",0,1);
		return 0;
	}
	FILE* destfile=fopen((dest+name).c_str(),"w");
	if(!destfile){
		err("Cannot open "+dest+name+" for reading.",0,1);
		return 0;
	}
	int read,left;
	char* buffer=(char*)malloc(BUFFER);
	while(left<buf.st_size){
		read=fread(buffer,1,BUFFER, srcfile);
		fwrite(buffer,1,read,destfile);
		left=left+read;
	}
	if(track!=NULL)
		track->push_back(dest+name);
	free(buffer);
	fclose(srcfile);
	fclose(destfile);
	chmod((dest+name).c_str(),buf.st_mode);
	return 1;
}

int cp(vector<string> src,string dest,vector<string> *track){
	int ret;
	for(int i=0;i<src.size();i++)
		ret=ret && cp(src[i],dest,track);
	return ret;
}



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

//Seperates things like something,something,something
vector<string> com2vec(const string locations){
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

void removeLine(string line,string file){
	vector<string> content=read(file);
	for(int i=0;i<content.size();i++){
		if(!strcmp(content[i].c_str(),line.c_str())){
			content.erase(content.begin()+i);
			i--;
		}
	}
	write(content,file,1);
}

//Reads the macros
void macro(string& config){
	string temp,macro,name;
	version ver;
	vector<string> *installed=new vector<string>;
	*installed=com2vec(search(Config::getPacklistPath()));
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
			name=(*installed)[i];
			sepVer(name,ver);
			if(!strcmp(name.c_str(),macro.c_str())){
				found=1;
				break;
			}
			ver="0.0.0";
		}
		if(found)
			config=config.substr(0,pos)+config.substr(pos+temp.find("}")+2);
		else{
			pp=config.find("}")+1;
			while(config[pp]==' ')
				pp++;
			if(config.substr(pp).find(" ")!=string::npos)
				pp+=(config.substr(pp)).find(" ");
			else
				pp=config.length();
			config=config.substr(0,pos)+config.substr(pp);
		}
		temp=config;
	}	
	

	delete installed;
}
