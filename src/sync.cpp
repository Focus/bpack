#include <cstring>
#include <vector>
#include "package.hpp"
#include "storage.hpp"
#include "search.hpp"
#include "error.hpp"
#include "install.hpp"
#include "version.hpp"


#include "sync.hpp"

using namespace std;

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
	}

	package *packg=new package;
	packg->setName(pack);
	packg->setVersion(ver);
	packg->setLocations(locations);
	packg->write();
	delete packg;
}

//Syncs up pacmans db with ours
void pacmanSync(){
	vector<package> installed=getInstalledPackages();
	vector<string> pacman=loadLocation(search("/var/lib/pacman/local/"));
	string pack;
	version ver;
	bool have;
	//Add packages pacman has and we haven't
	for(int i=0;i<pacman.size();i++){
		pack=pacman[i];
		depVersion(pack,ver);
		have=0;
		for(int j=0;j<installed.size();j++){
			//If we dont have the package
			if(!strcmp( (installed[j].getName()+"-"+installed[j].getVersion()).c_str(), 
						(pack+"-"+ver.asString()).c_str()))
				have=1;
		}
		if(!have)
			pacmanRead("/var/lib/pacman/local/"+pacman[i],pack,ver);
	}

}
			
