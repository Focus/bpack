#include <cstring>
#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;
#include "packinst.hpp"
#include "version.hpp"
#include "package.hpp"
#include "config.hpp"


bool installScript(packinst inst)
{
     // get variables
  string scriptname, scriptpath, tardir, installdir, logroot,hijack;
     scriptname = inst.getName() + "-" + inst.getVersion() + ".bis";
     scriptpath = Config::getScriptDir() + scriptname;
     tardir = Config::getTarballDir();
     installdir = Config::getPackmanDir() + inst.getName();
     logroot = Config::getLogDir();
     hijack=Config::getLib();
     int result;
     
     // Check if script exists
     ifstream testscript(scriptpath.c_str());
     if(testscript.fail()){
         cout << "Script " + scriptname + " not found, here is where we look online\n";
     } else{
       //Preload the hijacker!
       setenv("LD_PRELOAD",hijack.c_str(),1);
       result = system(("bash -e " + scriptpath + " " + tardir + " " + installdir + " " + logroot + " \"" + inst.getConfig() + "\" \""
			  + inst.getMake() + "\" \"" + inst.getMakeInst() + "\"").c_str());
     }
     testscript.close();
     
     
     // check if installed correctly
	cout<<result<<endl;
     if(!result)
         return false;  // don't know if this works
     
     package pack(inst);

     pack.addLocation(installdir);


     
     pack.write();
     
     return true;
}
