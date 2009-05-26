#include <cstring>
#include <iostream>
#include <dirent.h>
#include <cstdlib>
#include <sys/stat.h>
#include <ftw.h>
using namespace std;

string tree;

//Searches the directory for a file, if left empty returns whats in the directory with ,
string search(string dir, string name=""){
	int pos=0;
	if(!strcmp(name.c_str(),""))
		pos=-1;
	else{ //Needed otherwise there is no substring!
	//Find if there is a * so we can look gcc* would give true for gcc-4.3
	if(!strcmp((name.substr(0,1)).c_str(),"*")){
		name= name.substr(1);
		pos=1;
	}
	if(!strcmp((name.substr(name.size()-1,1)).c_str(),"*")){
		name= name.substr(0,name.size()-1);
		pos=2;
	}
	}
	//Begin the search
	static DIR *dp=NULL;
	struct dirent *d;
	dp=opendir(dir.c_str());
	if(dp==NULL){
		cerr<<"\nCannot search "<<dir<<endl;
		exit(1);
	}
	string ret,rety;
	ret="";
	rety="";
	//string *temp=new string;
	
	//Loop for searching
	if(pos>=0){
	int findy=0;
	while(d=readdir(dp)){
		ret=d->d_name;
		if(strcmp(ret.c_str(),".") && strcmp(ret.c_str(),"..")){
		if((findy=ret.find(name))>=0){
			if(findy>0 && pos==1){
				rety=rety+ret+",";
				}
			if(findy==0 && pos==0 && ret.size()==name.size()){
				closedir(dp);dp=NULL;
				return ret;
				}
			if(findy==0 && pos==2){
				rety=rety+ret+",";
				}
		}
	}
	}
	return rety;
	}
	
	//Loop for listing
	if(pos<0){
		while(d=readdir(dp)){
		if(strcmp(d->d_name,".")&&strcmp(d->d_name,"..")){		
			ret=ret+d->d_name;
			ret=ret+",";
			}
		}
	closedir(dp);dp=NULL;
	return ret.substr(0,ret.size()-1);
	}
	
	closedir(dp);
	dp=NULL;
	return "";
	

}

//We need this for the directory tree (I hate C strings)
int list(const char* name, const struct stat *sp, int type){
	if(type==FTW_D)
		tree=tree+name+",";

	return 0;

}

string dirTree(const string path){
	tree="";
	ftw(path.c_str(),list,12);
	return tree.substr(0,tree.size()-1);
}
