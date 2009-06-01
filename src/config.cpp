#include <cstring>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include "config.hpp"
#include "storage.hpp"
using namespace std;

string Config::installdir, Config::scriptdir, Config::packinstdir, Config::tarballdir, Config::packmandir, Config::packlistpath, Config::logdir,Config::lib;
int Config::quitlevel;


void Config::initialise()
{
    	Config::installdir="/var/bpack/";
		if(getenv("BPACK_DIR")&&strcmp(getenv("BPACK_DIR"),"")){	
			string env(getenv("BPACK_DIR"));							
			if(env[env.size()-1]=='/')
				Config::installdir=env;
			else
				Config::installdir=env+"/";
		}
    	scriptdir = installdir + "scripts/";
    	packinstdir = installdir + "packs/";
    	tarballdir = installdir + "tarballs/";
    	packmandir = installdir + "packman/";
    	packlistpath = installdir + "packs.list";
		logdir=installdir+"logs/";
		lib=installdir+"libs/hijack.so.1.0";
		Config::quitlevel=0;
		
		//Load the config
		vector<string> *file= new vector<string>;
		*file=read("/etc/bpack.conf");
		if(file->size()<=0){
			delete file;
			return;
		}
		vector<int> *line=new vector<int>;
		vector<int> *pos=new vector<int>;
		for(int i=0;i<file->size();i++){
			(*file)[i]=(*file)[i].substr(0,(*file)[i].find_first_of("#"));
			if( (*file)[i].find_first_of(":")+1>0  ){
				pos->push_back((*file)[i].find_first_of(":"));
				line->push_back(i);
			}
		}
		string command,value;
		line->push_back(file->size());
		for(int i=0;i<pos->size();i++){
			command=(*file)[(*line)[i]].substr(0,(*pos)[i]);
			value=(*file)[(*line)[i]].substr((*pos)[i]+1);
		
			//Do a mini loop to get all of the lines in between the commands
		
			for(int j=(*line)[i]+1;j<(*line)[i+1];j++){
				if((*file)[j].length()>0)
					value=value+"\n"+(*file)[j];
			}
		
			//Do we set anything?
		
			if(!strcmp(command.c_str(),"quitlevel")){
				stringstream ss;
				(ss) << value;
				if( ((ss) >> Config::quitlevel).fail() )
					Config::quitlevel=0;
				//delete ss;
			}
		}
		delete line,pos,file;
    	
}
