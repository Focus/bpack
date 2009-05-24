#include <iostream>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include "storage.hpp"
#include "version.hpp"
#include "install.hpp"
#include "config.hpp"
#include "remove.hpp"
#include "package.hpp"
using namespace std;

int makeinstall(string path){
	int *pos= new int;
	*pos=path.find_last_of("/");
	string *name=new string;
	version *ver=new version;
	if(*pos>0){
		*name=path.substr(*pos,path.size()-*pos-2);
		depVersion(*name,*ver);
	}
	erase("/tmp/hijack_log.txt");
	setenv("LD_PRELOAD",(Config::getLib()).c_str(),1);
	system("make install");
	vector<string> *locs=new vector<string>;
	*locs=read("/tmp/hijack_log.txt");
	if(locs->size() <= 0){
		erase("/tmp/hijack_log.txt");
		cout<<"\nThe log was empty. No tracking done!!\n"<<endl;
		setenv("LD_PRELOAD","",1);
		delete locs;
		delete pos;
		delete name;
		delete ver;
		return 0;
	}
	string *input=new string;
	cout<<"\nPlease enter the name of the package [default:"<<*name<<"]  :";
	getline(cin,*input);
	if(input!=NULL)
		*name=*input;
	cout<<"\nPlease enter the version of the package [default:"<<*ver<<"]  :";
	getline(cin,*input);
	if(input!=NULL)
		*ver=*input;
	package *pack=new package;
	pack->setName(*name);
	pack->setLocations(*locs);
	pack->setVersion(ver->asString());
	pack->write();
	delete pack;
	delete locs;
	delete input;
	delete pos;
	delete name;
	delete ver;
	return 1;
}

void terminal(){

	string command,path;
	command="";
	path=get_current_dir_name();
	cout<<"\nBpack terminal\nUse exit to quit. The command  make install  will get bpack to track down the calls"<<endl;
	while(strcmp("exit",command.c_str())){
		if(command.find("cd ")+1>0){
			if(chdir(command.substr(command.find("cd ")+3).c_str())!=0)
				cout<<strerror(errno)<<endl;
		}
		else if(strcmp(command.c_str(),"make install"))
			system(command.c_str());
		else{
			makeinstall(path);
		}
		path=get_current_dir_name();
		cout<<"[bpack : "<<path<<"] $ ";
		getline(cin,command);
		
	}
}
