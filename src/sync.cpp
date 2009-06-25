#include <cstring>
#include <vector>
#include <sys/stat.h>


#include "package.hpp"
#include "search.hpp"
#include "error.hpp"
#include "install.hpp"
#include "version.hpp"

#include "storage.hpp"

#include "sync.hpp"



using namespace std;

int partition(int low, int high, vector<int>& arr){
	int it=arr[low];
	int top;
	while(high>low){
		top=arr[high];
		while(it>top && high>low){
			high--;
			top=arr[high];
		}
		arr[low]=top;
		while(it<top && high>low){
			low++;
			top=arr[low];
		}
		arr[high]=top;
	}
	arr[low]=it;
	return low;
}
			
void quicksort(int low, int high, vector<int>& arr){
	if(low<high){
		int index=partition(low,high,arr);
		quicksort(low,index-1,arr);
		quicksort(index+1,high,arr);
	}
}

//Makes a pacman package from a bpack one
void pacmanWrite(package pack){
	string path="/var/lib/pacman/local/"+pack.getName()+"-"+pack.getVersion()+"/";
	mkdir(path.c_str(),666);
	packinst *dep=new packinst;
	*dep=getPackage(Config::getPackInstDir()+pack.getName()+"-"+pack.getVersion());
	string buffer;
	if(dep->getName().length()>0){
		buffer="%DEPENDS%\n";
		vector<string> *deps=new vector<string>;
		*deps=dep->getDeps();
		for(int i=0;i<deps->size();i++)
			buffer=buffer+(*deps)[i]+"\n";
		delete deps;
		write(buffer,path+"depends");
	}
	else
		write("",path+"depends");
	delete dep;



}

//Makes a bpack package from a pacman one
void pacmanRead(string location,string pack,version ver){

	//vector<string> *locations=new vector<string>;
	vector<string> locations=read(location+"/files");
	if(locations.size()<=0){
		err("Invalid package from pacman");
		return ;
	}
	//Pacman lists a lot of folders lik usr/bin/ which obviously we dont want
	locations.erase(locations.begin());
	for(int i=0;i<locations.size();i++){
		if(!strcmp(locations[i].c_str(),"") || locations[i][locations[i].length()-1]=='/' 
				|| locations[i].find(" ")!=string::npos || locations[i].find("/")==string::npos){
			locations.erase(locations.begin()+i);
			i--;
		}
		else
			locations[i]="/"+locations[i];
	}

	package *packg=new package;
	packg->setName(pack);
	packg->setVersion(ver);
	packg->setLocations(locations);
	packg->save();
	delete packg;
}

//Syncs up pacmans db with ours
void pacmanSync(){
	vector<package> installed=getInstalledPackages();
	vector<string> pacman=loadLocation(search("/var/lib/pacman/local/"));
	string pack;
	version ver;
	vector<int> pl;
	bool have;
	//Add packages pacman has and we haven't
	for(int i=0;i<pacman.size();i++){
		pack=pacman[i];
		depVersion(pack,ver);
		have=0;
		for(int j=0;j<installed.size();j++){
			//If we dont have the package
			if(!strcmp( (installed[j].getName()+"-"+installed[j].getVersion()).c_str(), 
						(pack+"-"+ver.asString()).c_str())){
				have=1;
				pl.push_back(j);
			}
		}
		if(!have)
			pacmanRead("/var/lib/pacman/local/"+pacman[i],pack,ver);
	}
	quicksort(0,pl.size()-1,pl);
	//Now give pacman what we have and it doesn't
	for(int i=0;i<pl.size();i++)
		installed.erase(installed.begin()+pl[i]);
	for(int i=0;i<installed.size();i++)
		pacmanWrite(installed[i]);

}
			
