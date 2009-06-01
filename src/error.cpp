#include "error.hpp"
void err(std::string message, int level,bool errnoset){
	
	std::cerr<<"\n"<<message;
	if(errnoset){
		std::cerr<<"\nReason: "<<strerror(errno)<<std::endl;
	}
	if(level==0)
		return;
	if(level==2)
		std::cerr<<"Fatal Error"<<std::endl;
	if(level<Config::getQuit())
		return;
	if( (Config::getQuit()<0 && -Config::getQuit()>=level) || level==1 ){
		std::string *input=new std::string;
		std::cout<<"Do you want to carry on? [y]es  [n]o : ";
		std::cin>>*input;
		if( (!strcmp( (input->substr(0,1)).c_str(),"y")) || (!strcmp(  (input->substr(0,1)).c_str(),"Y")))
			return;
		std::cerr<<"\nAborted"<<std::endl;
		exit(1);
	}
	exit(1);
}
