#include "packinst.hpp"
using namespace std;
//Get the next dependency
bool packinst::getNextDep(string &dep)
{
     if(loc>=deps.size())
     return 0;
     else
     {
     dep=deps[loc];
     loc++;
     return 1;
     }
}

//Analyses each line of the config
void lineAnalyse(string line, packinst &pack){
	
	//Strip comments
	if(line.find_first_of("#")>=0)
		line=line.substr(0,line.find_first_of("#"));
	if(line.find_first_of("#")==0)
		return;
	int pos=line.find_first_of(":");
	//If there is no command (i.e. nothing of the form config:)
	if(pos<=0)
		return;
	string command=line.substr(0,pos);
	string value=line.substr(pos+1);
	

	//Whole load of options!
	if(!strcmp(command.c_str(),"config"))
		pack.setConfig(value);
	if(!strcmp(command.c_str(),"make"))
		pack.setMake(value);
	if(!strcmp(command.c_str(),"makeinst"))
		pack.setMakeInst(value);
	if(!strcmp(command.c_str(),"deps"))
		pack.setDeps(loadLocation(value));
	if(!strcmp(command.c_str(),"location"))
		pack.setWget(value);
		
}

//Get and return a package installation from file
//TODO: Check file opened up
packinst getPackage(string location){
	ifstream textfile;
	textfile.open(location.c_str());
	packinst pack;
	string text;
	while(getline(textfile,text)){
		lineAnalyse(text,pack);
	}
	textfile.close();
	return pack;
}
