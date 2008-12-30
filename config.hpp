#ifndef _CONFIG_H
#define _CONFIG_H
class Config{
      
    private:
        static std::string installdir, scriptdir, packinstdir, tarballdir, packmandir, packlistpath, logdir;


    public:

        // gets the directory containing bpack
        static std::string getInstallDir() {return installdir;}
        // gets the install script directory
        static std::string getScriptDir() {return scriptdir;}
        // gets the pack instruction directory
        static std::string getPackInstDir() {return packinstdir;}
        // gets the tarball directory
        static std::string getTarballDir() {return tarballdir;}
        // gets the directory used for temporary installation
        static std::string getPackmanDir() {return packmandir;}
        // gets log directory
        static std::string getLogDir() {return logdir;}

        // gets the path of the packlist
        static std::string getPacklistPath() {return packlistpath;}
        
        static void initialise(char* argv0);



};

#endif
