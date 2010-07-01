#include <errno.h>
#include <iostream>
#include <cstdlib>

#include "config.hpp"

#include "error.hpp"


void err(std::string message, int level,bool errnoset){
	
	std::cerr<<"\n** "<<message<<std::endl;
	if(errnoset){
		std::cerr<<"\n** Reason: "<<strerror(errno)<<std::endl;
	}
	if(level==0)
		return;
	if(level==2)
		std::cerr<<"\n** Fatal Error"<<std::endl;
	if(level<Config::getQuit())
		return;
	if( (Config::getQuit()<0 && -Config::getQuit()>=level) || level==1 ){
		std::string *input=new std::string;
		std::cout<<"Do you want to carry on? [y]es  [n]o : ";
		std::cin>>*input;
		if( (!strcmp( (input->substr(0,1)).c_str(),"y")) || (!strcmp(  (input->substr(0,1)).c_str(),"Y"))){
			delete input;
			return;
		}
		delete input;
		std::cerr<<"\n** Aborted"<<std::endl;
		exit(1);
	}
	exit(1);
}

bool ask(std::string question){
	std::cout<<question<<" [y]es  [n]o : ";
	std::string *input=new std::string;
	std::cin>>*input;
	std::cout<<std::endl;
	if( (!strcmp( (input->substr(0,1)).c_str(),"y")) || (!strcmp(  (input->substr(0,1)).c_str(),"Y"))){
		delete input;
		return 1;
		}
	delete input;
	return 0;
}
