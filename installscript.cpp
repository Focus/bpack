#include <string>
#include <iostream>
#include <fstream>

using namespace std;
#include "packinst.hpp"
#include "package.hpp"
#include "config.hpp"


bool installScript(packinst inst)
{
     // get variables
     string scriptname, scriptpath, tardir, installdir, logroot;
     scriptname = inst.getName() + "-" + inst.getVersion() + ".bis";
     scriptpath = Config::getScriptDir() + scriptname;
     tardir = Config::getTarballDir();
     installdir = Config::getPackmanDir() + inst.getName();
     logroot = Config::getLogDir();
     
     int result;
     
     // Check if script exists
     ifstream testscript(scriptpath.c_str());
     if(testscript.fail()){
         cout << "Script " + scriptname + " not found, here is where we look online\n";
     } else
         result = system(("bash -e " + scriptpath + " " + tardir + " " + installdir + " " + logroot + " \"" + inst.getConfig() + "\" \""
                    + inst.getMake() + "\" \"" + inst.getMakeInst() + "\"").c_str());
     testscript.close();
     
     
     // check if installed correctly
     if(result)
         return false;  // don't know if this works
     
     package pack(inst);

     pack.addLocation(installdir);


     
     pack.write();
     
     return true;
}
