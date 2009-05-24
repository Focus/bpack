#include <cstring>
#include <cstdlib>
#include <fstream>
#include "config.hpp"
#include "storage.hpp"
using namespace std;

string Config::installdir, Config::scriptdir, Config::packinstdir, Config::tarballdir, Config::packmandir, Config::packlistpath, Config::logdir,Config::lib;


void Config::initialise()
{
    	
	
<<<<<<< .mine
    	Config::installdir="/var/bpack/";
		if(getenv("BPACK_DIR")&&strcmp(getenv("BPACK_DIR"),"")){	
			string env(getenv("BPACK_DIR"));							
			if(env[env.size()-1]=='/')
				Config::installdir=env;
			else
				Config::installdir=env+"/";
		}
=======
    	if (apppath.find('/') == 0) // absolute path 
        	Config::installdir = apppath.substr(0, apppath.rfind('/'));
    	else                        // relative path
        	Config::installdir = getenv("PWD") + apppath.substr(1, apppath.rfind('/'));
                                // i don't think this will work if run through $PATH

>>>>>>> .r81
<<<<<<< .mine
    	scriptdir = installdir + "scripts/";
    	packinstdir = installdir + "packs/";
    	tarballdir = installdir + "tarballs/";
    	packmandir = installdir + "packman/";
    	packlistpath = installdir + "packs.list";
		logdir=installdir+"logs/";
		lib=installdir+"libs/hijack.so.1.0";
=======
    	scriptdir = installdir + "/scripts/";
    	packinstdir = installdir + "/packs/";
    	tarballdir = installdir + "/tarballs/";
    	packmandir = installdir + "/packman/";
    	packlistpath = installdir + "/packs.list";
	logdir=installdir+"/logs/";
	lib=installdir+"/libs/hijack.so.1.0";
>>>>>>> .r81

		vector<string> config=read("/etc/bpack.conf");
		if(config.size()<=0){
			return;	
		}
    	
}
