#include <iostream>
#include <string>
#include "config.hpp"


 std::string Config::installdir, Config::scriptdir, Config::packinstdir, Config::tarballdir, Config::packmandir, Config::packlistpath, Config::logdir;

void Config::initialise(char* argv0)
{
    std::string apppath(argv0);


    if (apppath.find('/') == 0) // absolute path 
        Config::installdir = apppath.substr(0, apppath.rfind('/')-1);
    else                        // relative path
        Config::installdir = getenv("PWD") + std::string("/") + apppath.substr(0, apppath.rfind('/')+1);
                                // i don't think this will work if run through $PATH
    
    std::cout << installdir << "\n";

    scriptdir = installdir + "scripts/";
    packinstdir = installdir + "packs/";
    tarballdir = installdir + "tarballs/";
    packmandir = installdir + "packman/";
    packlistpath = installdir + "packs.list";
    //website="http://bpack.googlecode.com/files/";
}
